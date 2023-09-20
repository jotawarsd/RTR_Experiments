//Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>
#include <stdlib.h>

//OpenGL Header files
#include <C:/glew/include/GL/glew.h>	//this must be above gl.h
#include <GL/gl.h>
#include "vmath.h"
using namespace vmath;

//CUDA headers
#include <cuda.h>

//macros
#define X GetSystemMetrics(SM_CXSCREEN)
#define Y GetSystemMetrics(SM_CYSCREEN)
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define MANDELBROT_WIDTH 6000
#define MANDELBROT_HEIGHT 6000

//OpenGL Libraries
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")

//Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
BOOL gbFullscreen = FALSE;
FILE *gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HGLRC ghrc = NULL;

//Programmable pipeline related global variables
GLuint shaderProgramObject;

enum
{
	SDJ_ATTRIBUTE_POSITION = 0,
	SDJ_ATTRIBUTE_COLOR,
	SDJ_ATTRIBUTE_NORMAL,
	SDJ_ATTRIBUTE_TEXTURE0
};

GLuint vao;
GLuint vbo_position;
GLuint vbo_texcoord;

GLint CheckerBoard[MANDELBROT_WIDTH][MANDELBROT_HEIGHT][4];
GLubyte Mandelbrot_Host[MANDELBROT_WIDTH][MANDELBROT_HEIGHT][4];
unsigned char *Mandelbrot_Device = NULL;
GLuint texture_Mandelbrot;

int numIterations = 0;
BOOL iterChanged = TRUE;

GLuint mvpMatrixUniform;
GLuint textureSamplerUniform;

mat4 perspectiveProjectionMatrix;

//cuda kernel
__global__ void mandelbrotKernel(unsigned char *pixel, int width, int height, int maxIter)
{
	//code
	unsigned int i = (blockIdx.x * blockDim.x) + threadIdx.x;
	unsigned int j = (blockIdx.y * blockDim.y) + threadIdx.y;

	float u = (float)i / (float)width;
	float v = (float)j / (float)height;

	u = u * 3.0f - 2.4f;
	v = v * 3.0f - 1.5f;

	float x = 0.0f, y = 0.0f;
	double iter = 0.0;
	while(x * x + y * y <= 4.0f && iter < maxIter)
	{
		float xtemp = x * x - y * y + u;
		y = 2.0f * x * y + v;
		x = xtemp;
		iter++;
	}
	iter /= 10.0f;

	pixel[(j * width * 4) + i * 4 + 0] = iter * 0.3 * 255;
	pixel[(j * width * 4) + i * 4 + 1] = iter * 0.5 * 255;
	pixel[(j * width * 4) + i * 4 + 2] = iter * 255;
	pixel[(j * width * 4) + i * 4 + 3] = 0;
}

//Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Function Declarations
	int initialize(void);
	void display(void);
	//void update(void);	-----no animation. hence update call not required
	void uninitialize(void);

	//Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");
	BOOL bDone = FALSE;
	int iRetVal = 0;

	//code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Creation of Log File Failed. Exiting now!"), TEXT("File I/O Error"), MB_OK | MB_ICONEXCLAMATION);
		exit(0);
	}
	else
		fprintf(gpFile, "Log File Created Successfully!\n");

	//Initialization of WNDCLASSEX structure
	wndclass.cbSize = sizeof(WNDCLASSEX);    //not included in WNDCLASS (only WNDCLASSEX)
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	//Client area color (Canvas)
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));    //not included in WNDCLASS

	//Registering Above class
	RegisterClassEx(&wndclass);

	//Creating the window
	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,																		//Extended style for fullscreen (window that remains above taskbar always)
		szAppName,																				//Name of Wnd class
		TEXT("Shaunak Jotawar OpenGL"),															//Title bar text
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,					//style of window 
																								//(3 additional styles ensure fullscreen window stays in focus above all children and sibling windows)
		X / 2 - (WIN_WIDTH / 2),																//x coordinate of top left corner
		Y / 2 - (WIN_HEIGHT / 2),																//y coordinate of top left corner
		WIN_WIDTH,																				//width
		WIN_HEIGHT,																				//height
		NULL,																					//parent window (NULL = Desktop)
		NULL,																					//Handle to Menu (NULL = no menu)
		hInstance,																				//handle to instance
		NULL);																					//Extra info for window

	ghwnd = hwnd;

	//Initialize
	iRetVal = initialize();
	if (iRetVal == -1)
	{
		fprintf(gpFile, "Choose Pixel Format failed\n");
		uninitialize();
	}
	else if (iRetVal == -2)
	{
		fprintf(gpFile, "Set Pixel Format failed\n");
		uninitialize();
	}
	else if (iRetVal == -3)
	{
		fprintf(gpFile, "Create OpenGL context failed\n");
		uninitialize();
	}
	else if (iRetVal == -4)
	{
		fprintf(gpFile, "Making OpenGL as current context failed\n");
		uninitialize();
	}
	else if (iRetVal == -5)
	{
		fprintf(gpFile, "Initialization of GLEW failed\n");
	}
	else if (iRetVal == -6)
	{
		fprintf(gpFile, "LoadGLTexture for Texture_Stone Failed\n");
		uninitialize();
	}
	else
		fprintf(gpFile, "Initialization successful!\n");

	//Show Window
	ShowWindow(hwnd, iCmdShow);

	//Foregrounding and focusing the window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//Game loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == TRUE)
			{
				//Rendering the scene
				display();

				//updating the scene
				//update();
			}
		}
	}

	uninitialize();
	return ((int)msg.wParam);
}

//Callback Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Function Prototypes
	void ToggleFullscreen(void);
	void resize(int, int);

	//code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;

	case WM_ERASEBKGND:
		return 0;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'F':
		case 'f':
			ToggleFullscreen();
			break;
		default:
			break;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			numIterations += 10;
			iterChanged = TRUE;
			break;
		case VK_DOWN:
			numIterations -= 10;
			iterChanged = TRUE;
			break;
		case 27:
			DestroyWindow(hwnd);
			break;
		default:
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

void ToggleFullscreen(void)
{
	//variable declarations
	static DWORD dwStyle;
	static WINDOWPLACEMENT wp;
	MONITORINFO mi;
	LONG left, top, width, height;

	//code
	wp.length = sizeof(WINDOWPLACEMENT);

	if (gbFullscreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi.cbSize = sizeof(MONITORINFO);

			if (GetWindowPlacement(ghwnd, &wp) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);

				left = mi.rcMonitor.left;
				top = mi.rcMonitor.top;
				width = mi.rcMonitor.right - mi.rcMonitor.left;
				height = mi.rcMonitor.bottom - mi.rcMonitor.top;

				SetWindowPos(ghwnd, HWND_TOP, left, top, width, height, SWP_NOZORDER | SWP_FRAMECHANGED);
			}

			ShowCursor(FALSE);
			gbFullscreen = TRUE;
		}
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOSIZE | SWP_FRAMECHANGED);

		ShowCursor(TRUE);
		gbFullscreen = FALSE;
	}
}

int initialize(void)
{
	//Function Declarations
	void uninitialize(void);
	void resize(int, int);
	void printGLInfo(void);
	void loadGLTexture(void);

	//variable declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;

	//code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	//initializing PIXELFORMATDESCRIPTOR structure
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	//get dc
	ghdc = GetDC(ghwnd);

	//Choose Pixel Format
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
		return -1;

	//Set the chosen pixel format
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
		return -2;

	//Create OpenGL rendering context
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
		return -3;

	//Make the rendering context as current context
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
		return -4;

	//GLEW initialization
	if (glewInit() != GLEW_OK)
		return -5;

	//Vertex Shader
	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 a_position;" \
		"in vec2 a_texcoord;" \
		"uniform mat4 u_mvpMatrix;" \
		"out vec2 a_texcoord_out;" \
		"void main(void)" \
		"{" \
		"gl_Position = u_mvpMatrix * a_position;" \
		"a_texcoord_out = a_texcoord;" \
		"}";

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
	glCompileShader(vertexShaderObject);

	GLint status;
	GLint infoLogLength;
	char *log = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char *)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "\n\nVertex Shader Compilation Log : %s\n\n", log);
				free(log);
				log = NULL;
				uninitialize();
			}
		}
	}

	//Fragment Shader
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec2 a_texcoord_out;" \
		"uniform sampler2D u_textureSampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = texture(u_textureSampler, a_texcoord_out);" \
		"}";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "\n\nFragment Shader Compilation Log : %s", log);
				free(log);
				log = NULL;
				uninitialize();
			}
		}
	}

	//Shader Program object
	shaderProgramObject = glCreateProgram();
	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject, SDJ_ATTRIBUTE_POSITION, "a_position");
	glBindAttribLocation(shaderProgramObject, SDJ_ATTRIBUTE_TEXTURE0, "a_texcoord");

	glLinkProgram(shaderProgramObject);


	//ERROR Checking
	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char*)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, log);
				fprintf(gpFile, "\n\nShader Program Link log : %s", log);
				free(log);
				log = NULL;
				uninitialize();
			}
		}
	}

	//post link steps
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");
	textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_textureSampler");

	//declarations of vertex data arrays
	const GLfloat position[] = 
	{
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

	const GLfloat texCoords[] =
	{
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};

	//vao and vbo related code
	//vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//vertex buffer object for ___position___
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(position), position, GL_STATIC_DRAW);
	glVertexAttribPointer(SDJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SDJ_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//vertex buffer object for ___texcoords___
	glGenBuffers(1, &vbo_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);

	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(SDJ_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SDJ_ATTRIBUTE_TEXTURE0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//unbind __vao__
	glBindVertexArray(0);

	//Depth enabling and clear color functions
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//load texture
	loadGLTexture();

	glEnable(GL_TEXTURE_2D);

	//clear color
	glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

	//initializing perspective projection matrix
	perspectiveProjectionMatrix = mat4::identity();
	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void loadGLTexture(void)
{
	//Function Declarations
	void makeMandelbrot(void);

	//code
	makeMandelbrot();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &texture_Mandelbrot);
	glBindTexture(GL_TEXTURE_2D, texture_Mandelbrot);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, MANDELBROT_WIDTH, MANDELBROT_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, Mandelbrot_Host);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void makeMandelbrot(void)
{
	//function prototypes
	void uninitialize(void);

	//variable declarations
	cudaError_t result = cudaSuccess;

	//code
	//Device memory allocation
	result = cudaMalloc((void **)&Mandelbrot_Device, MANDELBROT_WIDTH * MANDELBROT_HEIGHT * 4 * sizeof(int));
	if (result != cudaSuccess)
	{
		fprintf(gpFile, "\nDevice Memory Allocation Failed for Mandelbrot_Device\n");
		uninitialize();
	}

	//kernel launch
	dim3 block(32, 32, 1);
	dim3 grid(MANDELBROT_WIDTH / block.x, MANDELBROT_HEIGHT / block.y, 1);

	//kernel call
	mandelbrotKernel <<<grid, block>>> (Mandelbrot_Device, MANDELBROT_WIDTH, MANDELBROT_HEIGHT, numIterations);

	//copy result from device to host
	result = cudaMemcpy(Mandelbrot_Host, Mandelbrot_Device, MANDELBROT_WIDTH * MANDELBROT_HEIGHT * 4 * sizeof(char), cudaMemcpyDeviceToHost);
	if (result != cudaSuccess)
	{
		fprintf(gpFile, "\nDevice to Host Memory Copy Failed for Mandelbrot_Host\n");
		uninitialize();
	}
}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(
		45.0, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f
	);
}

void display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//use shader program object
	glUseProgram(shaderProgramObject);

	//transformations
	mat4 translationMatrix = mat4::identity();
	mat4 modelViewMatrix = mat4::identity();
	mat4 modelViewProjectionMatrix = mat4::identity();
	
	translationMatrix = translate(0.0f, 0.0f, -2.0f);

	modelViewMatrix = translationMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//texture binding
	if (iterChanged)
	{
		loadGLTexture();
	
		TCHAR str[255];
		wsprintf(str, TEXT("Mandelbrot Set : %d iterations"), numIterations);
		SetWindowText(ghwnd, str);
		
		iterChanged = FALSE;
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_Mandelbrot);
	glUniform1i(textureSamplerUniform, 0);

	//Draw
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);

	//unuse the shader program object
	glUseProgram(0);

	SwapBuffers(ghdc);
}

//void update(void)
//{
//	//code
//}

void uninitialize(void)
{
	//Function declarations
	void ToggleFullscreen(void);

	//code
	if (gbFullscreen)
	{
		ToggleFullscreen();
	}

	if (Mandelbrot_Device)
	{
		cudaFree(Mandelbrot_Device);
		Mandelbrot_Device = NULL;
	}

	//deletion and uninitialization of texture
	if (texture_Mandelbrot)
	{
		glDeleteTextures(1, &texture_Mandelbrot);
		texture_Mandelbrot = 0;
	}

	//deletion and uninitialization of vbo
	if (vbo_texcoord)
	{
		glDeleteBuffers(1, &vbo_texcoord);
		vbo_texcoord = 0;
	}

	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	//shader uninitialization
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);
		GLsizei numAttachedShaders;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);
		GLuint *shaderObjects = NULL;
		shaderObjects = (GLuint *)malloc(numAttachedShaders * sizeof(GLuint));

		//filling empty buffer with shader objects
		glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);

		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}

		free(shaderObjects);
		shaderObjects = NULL;
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}

	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
		fprintf(gpFile, "Current context initialized to NULL\n");
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		fprintf(gpFile, "Current context deleted\n");
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
		fprintf(gpFile, "Device context released\n");
	}

	if (ghwnd)
	{
		fprintf(gpFile, "Window destroyed\n");
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log File closed successfully!\n");
		fclose(gpFile);
		gpFile = NULL;
	}

	exit(0);
}
