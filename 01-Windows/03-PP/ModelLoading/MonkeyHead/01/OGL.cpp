//Header Files
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

//local headers
#include "OGL.h"
#include "MeshLoader.h"

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
BOOL gbActiveWindow = FALSE;
HGLRC ghrc = NULL;
FILE *gpFile = NULL;

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
GLuint vbo_position;		//vertex buffer object
GLuint vbo_normal;
GLuint vbo_indices_position;
GLuint vbo_indices_normal;

//uniforms
GLuint modelMatrixUniform;
GLuint viewMatrixUniform;
GLuint projectionMatrixUniform;

GLuint laUniform;
GLuint ldUniform;
GLuint lsUniform;
GLuint lightPositionUniform;

GLuint kaUniform;
GLuint kdUniform;
GLuint ksUniform;
GLuint materialShininessUniform;

GLuint lightingEnabledUniform;

//light array definitions
GLfloat lightAmbient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat lightDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat lightSpecular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat lightPosition[] = { 20.0f, 0.0f, 20.0f, 1.0f };

GLfloat materialAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat materialDiffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat materialSpecular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat materialShininess = 128.0f;

BOOL bLight = FALSE;

GLuint mvpMatrixUniform;

mat4 perspectiveProjectionMatrix;

//model loading related variables
FILE *gpMeshFile = NULL;
FILE *gpMeshErr = NULL;

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];
struct vecFloat *vertices, *texCoords, *normals;
struct vecInt *vertexIndices, *textureIndices, *normalIndices;

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

	if (fopen_s(&gpMeshErr, "MeshLog.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Creation of MeshLog File Failed. Exiting now!"), TEXT("File I/O Error"), MB_OK | MB_ICONEXCLAMATION);
		exit(0);
	}
	else
		fprintf(gpMeshErr, "MeshLog File Created Successfully!\n");

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
		case 'L':
		case 'l':
			if (bLight == FALSE)
				bLight = TRUE;
			else
				bLight = FALSE;
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
	void loadMesh(void);

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
		"uniform mat4 u_modelMatrix;" \
		"uniform mat4 u_viewMatrix;" \
		"uniform mat4 u_projectionMatrix;" \
		"uniform vec4 u_lightPosition;" \
		"uniform int u_lightingEnabled;" \
		"out vec3 transformedNormals;" \
		"out vec3 lightDirection;" \
		"out vec3 viewerVector;" \
		"void main(void)" \
		"{" \
		"if (u_lightingEnabled == 1)" \
		"	{" \
		"vec4 eyeCoordinates = u_viewMatrix * u_modelMatrix * a_position;" \
		"mat3 normalMatrix = mat3(u_viewMatrix * u_modelMatrix);" \
		"transformedNormals = normalMatrix * a_normal;" \
		"lightDirection = vec3(u_lightPosition) - eyeCoordinates.xyz;" \
		"viewerVector = -eyeCoordinates.xyz;" \
		"	}" \
		"gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * a_position;" \
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
		"uniform int u_lightingEnabled;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"vec3 phong_ads_color;"
		"if (u_lightingEnabled == 1)"
		"{" \
		"vec3 ambient = u_la * u_ka;" \
		"vec3 normalized_transformed_normals = normalize(transformedNormals);" \
		"vec3 normalized_light_direction = normalize(lightDirection);" \
		"vec3 diffuse = u_ld * u_kd * max(dot(normalized_light_direction, normalized_transformed_normals), 0.0);" \
		"vec3 reflectionVector = reflect(-normalized_light_direction, normalized_transformed_normals);" \
		"vec3 normalized_viewer_vector = normalize(viewerVector);" \
		"vec3 specular = u_ls * u_ks * pow(max(dot(reflectionVector, normalized_viewer_vector), 0.0), u_materialShininess);" \
		"phong_ads_color = ambient + diffuse + specular;" \
		"}" \
		"else" \
		"{" \
		"phong_ads_color = vec3(1.0, 1.0, 1.0);" \
		"}" \
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
	glBindAttribLocation(shaderProgramObject, SDJ_ATTRIBUTE_POSITION, "a_normal");

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
	lightingEnabledUniform = glGetUniformLocation(shaderProgramObject, "u_lightingEnabled");

	//declarations of vertex data arrays
	loadMesh();

	//vao and vbo related code
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo_position);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
		glBufferData(GL_ARRAY_BUFFER, vertices->size * sizeof(float), (GLfloat *)vertices->pf, GL_STATIC_DRAW);
		glVertexAttribPointer(SDJ_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(SDJ_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);
	glBufferData(GL_ARRAY_BUFFER, normals->size * sizeof(float), (GLfloat *)normals->pf, GL_STATIC_DRAW);
	glVertexAttribPointer(SDJ_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(SDJ_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_indices_position);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices_position);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices->size * sizeof(float), (GLfloat *)vertexIndices->p, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_indices_normal);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices_normal);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, normalIndices->size * sizeof(float), (GLfloat *)normalIndices->p, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

void loadMesh(void)
{
	//function declarations
	void uninitialize(void);

	//variable declarations
	char *space = " ", *slash = "/", *firstToken = NULL, *token = NULL;
	char *fEntries[3] = { NULL, NULL, NULL };
	int numVertices = 0, numTexcoords = 0, numNormals = 0, numFaces = 0;
	int i, vi;

	//code
	gpMeshFile = fopen("MonkeyHead.OBJ", "r");
	if (gpMeshFile == NULL)
	{
		fprintf(gpMeshErr, "Error in opening obj file!!\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}

	vertices = createVecFloat(gpMeshErr, "v");
	texCoords = createVecFloat(gpMeshErr, "vt");
	normals = createVecFloat(gpMeshErr, "vn");

	vertexIndices = createVecInt(gpMeshErr, "iv");
	textureIndices = createVecInt(gpMeshErr, "it");
	normalIndices = createVecInt(gpMeshErr, "in");

	while (fgets(buffer, BUFFER_SIZE, gpMeshFile) != NULL)
	{
		firstToken = strtok(buffer, space);

		if (strcmp(firstToken, "v") == 0)
		{
			numVertices++;
			while ((token = strtok(NULL, space)) != NULL)
				pushBackVecFloat(vertices, atof(token), gpMeshErr, "v");
		}
		else if (strcmp(firstToken, "vt") == 0)
		{
			numTexcoords++;
			while ((token = strtok(NULL, space)) != NULL)
				pushBackVecFloat(texCoords, atof(token), gpMeshErr, "vt");
		}
		else if (strcmp(firstToken, "vn") == 0)
		{
			numNormals++;
			while ((token = strtok(NULL, space)) != NULL)
				pushBackVecFloat(normals, atof(token), gpMeshErr, "vn");
		}
		else if (strcmp(firstToken, "f") == 0)
		{
			numFaces++;
			for (i = 0; i < 3; i++)
				fEntries[i] = strtok(NULL, space);

			for (i = 0; i < 3; i++)
			{
				token = strtok(fEntries[i], slash);
				pushBackVecInt(vertexIndices, atoi(token) - 1, gpMeshErr, "iv");

				token = strtok(NULL, slash);
				pushBackVecInt(textureIndices, atoi(token) - 1, gpMeshErr, "it");

				token = strtok(NULL, slash);
				pushBackVecInt(normalIndices, atoi(token) - 1, gpMeshErr, "in");
			}
		}
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
	mat4 modelMatrix = mat4::identity();
	mat4 viewMatrix = mat4::identity();
	
	translationMatrix = translate(0.0f, 0.0f, -4.0f);
	modelMatrix = translationMatrix;
	
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	//__lighting uniforms__
	if (bLight == TRUE)
	{
		glUniform1i(lightingEnabledUniform, 1);
		glUniform3fv(laUniform, 1, lightAmbient);
		glUniform3fv(ldUniform, 1, lightDiffuse);
		glUniform3fv(lsUniform, 1, lightSpecular);
		glUniform4fv(lightPositionUniform, 1, lightPosition);

		glUniform3fv(kaUniform, 1, materialAmbient);
		glUniform3fv(kdUniform, 1, materialDiffuse);
		glUniform3fv(ksUniform, 1, materialSpecular);
		glUniform1f(materialShininessUniform, materialShininess);
	}
	else
		glUniform1i(lightingEnabledUniform, 0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//bind vao
	glBindVertexArray(vao);	

	//Draw
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices_position);
	glDrawElements(GL_TRIANGLES, vertexIndices->size, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

	if (gpMeshFile)
	{
		fprintf(gpMeshFile, "MeshLog File closed successfully!\n");
		fclose(gpMeshFile);
		gpMeshFile = NULL;
	}

	exit(0);
}
