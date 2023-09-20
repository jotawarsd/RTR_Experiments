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

//macros
#define X GetSystemMetrics(SM_CXSCREEN)
#define Y GetSystemMetrics(SM_CYSCREEN)
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

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

GLuint vao;		//vertex array object
GLuint vbo;		//vertex buffer object
GLuint mvpMatrixUniform;

mat4 perspectiveProjectionMatrix;

//tessellation shader related variables
GLuint numberOfSegmentsUniform;
GLuint numberOfStripsUniform;
GLuint lineColorUniform;
unsigned int uiNumberOfSegments;

//Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Function Declarations
	int initialize(void);
	void display(void);
	void update(void);
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
				update();
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
		case 27:
			DestroyWindow(hwnd);
			break;
		case VK_UP:
			if(uiNumberOfSegments < 30)
				uiNumberOfSegments++;
			break;
		case VK_DOWN:
			if (uiNumberOfSegments > 1)
				uiNumberOfSegments--;
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
		"in vec2 a_position;" \
		"void main(void)" \
		"{" \
		"gl_Position = vec4(a_position, 0.0, 1.0);" \
		"}";

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
	glCompileShader(vertexShaderObject);

	GLint status = 0;
	GLint infoLogLength = 0;
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

//###########################################################################################################################################################################################
	//tessellation control Shader
	const GLchar* tessellationControlShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"layout(vertices = 5)out;" \
		"uniform int u_numberOfSegments;" \
		"uniform int u_numberOfStrips;" \
		"void main(void)" \
		"{" \
		"gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" \
		"gl_TessLevelOuter[0] = float(u_numberOfStrips);" \
		"gl_TessLevelOuter[1] = float(u_numberOfSegments);" \
		"}";

	GLuint tessellationControlShaderObject = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(tessellationControlShaderObject, 1, (const GLchar**)&tessellationControlShaderSourceCode, NULL);
	glCompileShader(tessellationControlShaderObject);

	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetShaderiv(tessellationControlShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(tessellationControlShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char *)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(tessellationControlShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "\n\nVertex Shader Compilation Log : %s\n\n", log);
				free(log);
				log = NULL;
				uninitialize();
			}
		}
	}

//###########################################################################################################################################################################################

	//tessellation evaluation Shader
	const GLchar* tessellationEvaluationShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"layout(isolines)in;" \
		"uniform mat4 u_mvpMatrix;" \
		"void main(void)" \
		"{" \
		"vec3 p0 = gl_in[0].gl_Position.xyz;" \
		"vec3 p1 = gl_in[1].gl_Position.xyz;" \
		"vec3 p2 = gl_in[2].gl_Position.xyz;" \
		"vec3 p3 = gl_in[3].gl_Position.xyz;" \
		"vec3 p4 = gl_in[4].gl_Position.xyz;" \
		"float u = gl_TessCoord.x;" \
		"vec3 p = (p0 * (1-u)*(1-u)*(1-u)*(1-u)) + (p1 * 4*u * (1-u)*(1-u)*(1-u)) + (p2 * 6*u*u * (1-u)*(1-u)) + (p3 * 4*u*u*u * (u - 1)) + (p4 * u*u*u*u);" \
		"gl_Position = u_mvpMatrix * vec4(p, 1.0f);" \
		"}";

	GLuint tessellationEvaluationShaderObject = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(tessellationEvaluationShaderObject, 1, (const GLchar**)&tessellationEvaluationShaderSourceCode, NULL);
	glCompileShader(tessellationEvaluationShaderObject);

	status = 0;
	infoLogLength = 0;
	log = NULL;

	glGetShaderiv(tessellationEvaluationShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(tessellationEvaluationShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char *)malloc(infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(tessellationEvaluationShaderObject, infoLogLength, &written, log);
				fprintf(gpFile, "\n\nVertex Shader Compilation Log : %s\n\n", log);
				free(log);
				log = NULL;
				uninitialize();
			}
		}
	}

//###########################################################################################################################################################################################

	//Fragment Shader
	const GLchar* fragmentShaderSourceCode = 
		"#version 460 core" \
		"\n" \
		"uniform vec4 u_lineColor;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = u_lineColor;" \
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
	glAttachShader(shaderProgramObject, tessellationControlShaderObject);
	glAttachShader(shaderProgramObject, tessellationEvaluationShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);
	
	glBindAttribLocation(shaderProgramObject, SDJ_ATTRIBUTE_POSITION, "a_position");

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
	numberOfSegmentsUniform = glGetUniformLocation(shaderProgramObject, "u_numberOfSegments");
	numberOfStripsUniform = glGetUniformLocation(shaderProgramObject, "u_numberOfStrips");
	lineColorUniform = glGetUniformLocation(shaderProgramObject, "u_lineColor");

	//declarations of vertex data arrays
	const GLfloat vertices[] =
	{
		-1.0f, -1.0f,
		0.5, 0.5,
		0.0, 1.0,
		1.0f, -1.0f,
		1.0f, 1.0f
	};

	//vao and vbo related code
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);		//bind the buffer to a target (which stores array buffers) in the vram

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(SDJ_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SDJ_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Depth enabling and clear color functions
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//initializing orthographic projection matrix
	perspectiveProjectionMatrix = mat4::identity();
	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	uiNumberOfSegments = 1;		//our geometry has atleast one line

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
	
	translationMatrix = translate(0.0f, 0.0f, -4.0f);
	modelViewMatrix = translationMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glUniform1i(numberOfSegmentsUniform, uiNumberOfSegments);

	//update the window caption bar to show number of segments
	TCHAR str[255];
	wsprintf(str, TEXT("Shaunak Jotawar OpenGL | Number of Segments = %d"), uiNumberOfSegments);
	SetWindowText(ghwnd, str);

	glUniform1i(numberOfStripsUniform, 1);
	glUniform4fv(lineColorUniform, 1, vec4(1.0f, 1.0f, 0.0f, 1.0f));

	//bind vao
	glBindVertexArray(vao);
	
	//draw
	glPatchParameteri(GL_PATCH_VERTICES, 5);
	glDrawArrays(GL_PATCHES, 0, 4);

	//unbind vao
	glBindVertexArray(0);

	//unuse the shader program object
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	//code

}

void uninitialize(void)
{
	//Function declarations
	void ToggleFullscreen(void);

	//code
	if (gbFullscreen)
	{
		ToggleFullscreen();
	}

	//deletion and uninitialization of vbo
	if (vbo)
	{
		glDeleteBuffers(1, &vbo);
		vbo = 0;
	}

	//deletion and uninitialization of vao
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
