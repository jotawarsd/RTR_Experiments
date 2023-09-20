//Header Files
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

//local headers
#include "OGL.h"
#include "noise.h"

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
#define NOISE_TEXTURE_SIZE 128

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
GLuint vbo_normals;

//noise variables
GLuint noiseTexture;
float offset = 0.0f;

//uniforms
GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint projectionMatrixUniform;

GLuint offsetUniform;
GLuint textureSamplerUniform;

//lighting uniforms
GLuint laUniform;
GLuint ldUniform;
GLuint lsUniform;
GLuint lightPositionUniform;

GLuint kaUniform;
GLuint kdUniform;
GLuint ksUniform;
GLuint materialShininessUniform;

GLfloat lightAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { -1.0f, 2.0f, -10.0f, 1.0f };

GLfloat materialAmbient[] = { 0.06f, 0.06f, 0.3f, 1.0f };
GLfloat materialDiffuse[] = { 0.3f, 0.3f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 0.1f, 0.1f, 0.5f, 1.0f };
GLfloat materialShininess = 128;

const int meshWidth = 256;
const int meshHeight = 256;

vec3 position[meshWidth][meshHeight][6];
vec3 normals[meshWidth][meshHeight][6];
vec2 texCoords[meshWidth][meshHeight][6];

mat4 perspectiveProjectionMatrix;

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
	BOOL LoadGLTexture(GLuint *, TCHAR[]);

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
		"in vec3 a_normal;" \
		"in vec2 a_texcoord;" \
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform sampler3D u_textureSampler;" \
		"uniform float u_offset;" \
		"uniform vec4 u_lightPosition;" \
		"out vec2 a_texcoord_out;" \
		"out vec3 transformedNormals;" \
		"out vec3 lightDirection;" \
		"out vec3 viewerVector;" \

		"struct WaterAttribs" \
		"{" \
			"vec3 waterPosition;" \
			"vec3 waterNormals;" \
		"};" \
		
		"WaterAttribs Water(float posx, float posz, float noise, float time)" \
		"{" \
			"vec3 Di = vec3(1.0, -1.0, 0.0);" \
			"vec3 pos = vec3(posx, 0.0, posz);" \
			"float Qi = 0.0, Ai = 0.0;" \
			"vec3 x = vec3(posx, 1.0, 1.0);" \
			"vec3 y = vec3(1.0, a_position.y, 1.0);" \
			"vec3 z = vec3(1.0, 1.0, posz);" \
			"float wi = 0.0;" \
			"float speed = 6.0;" \
			"float phase = 0.0, lamda = 0.0;" \
			"float px = 0.0, py = 0.0, pz = 0.0;" \
			"float pix = 0.0, piy = 0.0, piz = 0.0;" \
			"float nix = 0.0, niy = 0.0, niz = 0.0;" \
			"float nx = 0.0, ny = 0.0, nz = 0.0;" \
			"float m = 3.0;" \
			
			"for (int i = 0; i < 8; i++)" \
			"{" \
				"lamda = noise * 50.0;" \
				"Ai = (lamda / 2000) * cos(8.0 * posz - 50.0 * time);" \
				"wi = 50.0 / lamda;" \
				"phase = noise;" \
				"Qi = 0.1;" \
				"pix = Ai * dot(Di, x) * cos((wi * dot(Di, pos)) + (phase * time));" \
				"piz = Ai * dot(Di, z) * cos((wi * dot(Di, pos)) + (phase * time));" \
				"piy = Ai * sin((wi * dot(Di, pos)) + (phase * time));" \
				"px += pix;" \
				"py += piy;" \
				"pz += piz;" \
				"nix = dot(Di, x) * wi * Ai * cos(wi * dot(Di, pos) + (phase * time));" \
				"niy = wi * Ai * sin((wi * dot(Di, pos)) + (phase * time));" \
				"niz = dot(Di, z) * wi * Ai * cos(wi * dot(Di, pos) + (phase * time));" \
				"nx += m * 2.0 * nix;" \
				"ny += m * niy;" \
				"nz += m * 2.0 * niz;" \
			"}" \
			
			"WaterAttribs temp;" \
			"temp.waterPosition = vec3(posx + px, py, posz + pz);" \
			"temp.waterNormals = vec3(nx, 1.0 - ny, nz);" \

			"return temp;" \
		"}" \
		
		"void main(void)" \
		"{" \
			"vec4 noiseTex = texture(u_textureSampler, vec3(a_texcoord.x - u_offset, a_texcoord.y - u_offset, u_offset * 0.2));" \
			"float noiseAmplitude = (noiseTex.r + noiseTex.g + noiseTex.b + noiseTex.a) * 0.1f;" \
			"WaterAttribs water;" \
			"water = Water(a_position.x, a_position.z, noiseAmplitude, u_offset * 0.5);" \

			"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(water.waterPosition.x, water.waterPosition.y + a_position.y, water.waterPosition.z, 1.0f);" \
			"a_texcoord_out = a_texcoord;" \

			"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * vec4(water.waterPosition, 1.0f);" \
			"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
			"transformedNormals = normalMatrix * water.waterNormals;" \
			"lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;" \
			"viewerVector = -eyeCoordinates.xyz;" \
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
		"in vec3 transformedNormals;" \
		"in vec3 lightDirection;" \
		"in vec3 viewerVector;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_materialShininess;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec3 phong_ads_color;" \
		"vec3 ambient = u_la * u_ka;" \
		"vec3 normalized_transformed_normals = normalize(transformedNormals);" \
		"vec3 normalized_light_direction = normalize(lightDirection);" \
		"vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction, normalized_transformed_normals), 0.0);" \
		"vec3 reflectionVector = reflect(-normalized_light_direction, normalized_transformed_normals);" \
		"vec3 normalized_viewer_vector = normalize(viewerVector);" \
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector, normalized_viewer_vector), 0.0), u_materialShininess);" \
		"phong_ads_color = ambient + diffuse + specular;" \

		"FragColor = vec4(phong_ads_color, 1.0);" \
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
	glBindAttribLocation(shaderProgramObject, SDJ_ATTRIBUTE_NORMAL, "a_normal");
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
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_modelMatrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_viewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "u_projectionMatrix");
	laUniform = glGetUniformLocation(shaderProgramObject, "u_la");
	ldUniform = glGetUniformLocation(shaderProgramObject, "u_ld");
	lsUniform = glGetUniformLocation(shaderProgramObject, "u_ls");
	lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_lightPosition");

	kaUniform = glGetUniformLocation(shaderProgramObject, "u_ka");
	kdUniform = glGetUniformLocation(shaderProgramObject, "u_kd");
	ksUniform = glGetUniformLocation(shaderProgramObject, "u_ks");
	materialShininessUniform = glGetUniformLocation(shaderProgramObject, "u_materialShininess");

	textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "u_textureSampler");
	offsetUniform = glGetUniformLocation(shaderProgramObject, "u_offset");

	//declarations of vertex data arrays
	for (unsigned int i = 0; i < meshWidth; i++)
	{
		for (unsigned int j = 0; j < meshHeight; j++)
		{
			float u = (float)i / (float)meshWidth;
			float v = (float)j / (float)meshHeight;

			float dw = 0.5f / (float)meshWidth;
			float dh = 0.5f / (float)meshHeight;

			position[i][j][0] = vec3(u - dw, sinf(u), v - dh);
			position[i][j][1] = vec3(u - dw, sinf(u), v + dh);
			position[i][j][2] = vec3(u + dw, sinf(u), v + dh);

			position[i][j][3] = vec3(u - dw, sinf(u), v - dh);
			position[i][j][4] = vec3(u + dw, sinf(u), v - dh);
			position[i][j][5] = vec3(u + dw, sinf(u), v + dh);

			texCoords[i][j][0] = vec2(u - dw, v - dh);
			texCoords[i][j][1] = vec2(u - dw, v + dh);
			texCoords[i][j][2] = vec2(u + dw, v + dh);

			texCoords[i][j][3] = vec2(u - dw, v - dh);
			texCoords[i][j][4] = vec2(u + dw, v - dh);
			texCoords[i][j][5] = vec2(u + dw, v + dh);

			normals[i][j][0] = vec3(u - dw, sinf(u), v - dh);
			normals[i][j][1] = vec3(u - dw, sinf(u), v + dh);
			normals[i][j][2] = vec3(u + dw, sinf(u), v + dh);

			normals[i][j][3] = vec3(u - dw, sinf(u), v - dh);
			normals[i][j][4] = vec3(u + dw, sinf(u), v - dh);
			normals[i][j][5] = vec3(u + dw, sinf(u), v + dh);
		}
	}

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

	//vertex buffer object for ___position___
	glGenBuffers(1, &vbo_normals);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);

	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(SDJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SDJ_ATTRIBUTE_NORMAL);

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

	glEnable(GL_TEXTURE_2D);

	unsigned char *pNoiseData = NULL;

	make3DNoise(NOISE_TEXTURE_SIZE, &pNoiseData);
	if (pNoiseData == NULL)
	{
		return -6;
	}

	glGenTextures(0, &noiseTexture);
	glBindTexture(GL_TEXTURE_3D, noiseTexture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, NOISE_TEXTURE_SIZE, NOISE_TEXTURE_SIZE, NOISE_TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, pNoiseData);
	glBindTexture(GL_TEXTURE_3D, 0);

	free(pNoiseData);
	pNoiseData = NULL;

	//clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//initializing perspective projection matrix
	perspectiveProjectionMatrix = mat4::identity();
	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

BOOL LoadGLTexture(GLuint *Texture, TCHAR ImageResourceID[])
{
	//Variable declarations
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	BOOL bResult = FALSE;

	//code
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), ImageResourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

	if (hBitmap)
	{
		bResult = TRUE;
		GetObject(hBitmap, sizeof(bmp), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);		//2nd param is changed from 4(in ffp) to 1 for better performance
		glGenTextures(1, Texture);
		glBindTexture(GL_TEXTURE_2D, *Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE, 0);
		DeleteObject(hBitmap);
	}

	return bResult;
}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	perspectiveProjectionMatrix = vmath::perspective(
		45.0, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f
	);
}

void display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//use shader program object
	glUseProgram(shaderProgramObject);

	//_____Pyramid_____
	//transformations
	mat4 translationMatrix = mat4::identity();
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	
	translationMatrix = translate(-0.5f, -0.4f, -1.7f);	

	modelMatrix = translationMatrix;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	glUniform3fv(laUniform, 1, lightAmbient);
	glUniform3fv(ldUniform, 1, lightDiffuse);
	glUniform3fv(lsUniform, 1, lightSpecular);
	glUniform4fv(lightPositionUniform, 1, lightPosition);

	glUniform3fv(kaUniform, 1, materialAmbient);
	glUniform3fv(kdUniform, 1, materialDiffuse);
	glUniform3fv(ksUniform, 1, materialSpecular);
	glUniform1f(materialShininessUniform, materialShininess);

	//noise texture binding
	glActiveTexture(noiseTexture);
	glUniform1i(textureSamplerUniform, 0);
	glBindTexture(GL_TEXTURE_3D, noiseTexture);
	glUniform1f(offsetUniform, offset);

	//bind vao
	glBindVertexArray(vao);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawArrays(GL_TRIANGLES, 0, meshWidth * meshHeight * 6);
	glBindVertexArray(0);

	//unuse the shader program object
	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	//code
	offset += 0.0003f;
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

	if (noiseTexture)
	{
		glDeleteTextures(1, &noiseTexture);
		noiseTexture = 0;
	}

	if (vbo_normals)
	{
		glDeleteBuffers(1, &vbo_normals);
		vbo_normals = 0;
	}

	if (vbo_texcoord)
	{
		glDeleteBuffers(1, &vbo_texcoord);
		vbo_texcoord = 0;
	}

	//deletion and uninitialization of vbo
	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
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
