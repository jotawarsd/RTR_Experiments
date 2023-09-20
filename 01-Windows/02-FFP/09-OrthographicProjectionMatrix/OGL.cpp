//Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>
#include <stdlib.h>

//OpenGL Header files
#include <GL/gl.h>
#include <GL/glu.h>

#define _USE_MATH_DEFINES 1

#include <math.h>		//for PI and trigonometric functions

#define X GetSystemMetrics(SM_CXSCREEN)
#define Y GetSystemMetrics(SM_CYSCREEN)
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//OpenGL Libraries
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")

//Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
BOOL gbFullscreen = FALSE;
FILE *gpFile = NULL;
BOOL gbActiveWindow = FALSE;
HGLRC ghrc = NULL;

float angleCube = 0.0f;

GLfloat IdentityMatrix[16];
GLfloat TranslationMatrix[16];
GLfloat ScaleMatrix[16];
GLfloat RotationMatrix_X[16];
GLfloat RotationMatrix_Y[16];
GLfloat RotationMatrix_Z[16];
GLfloat OrthoMatrix[16];


//Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Function Declarations
	int initialize(void);
	void display(void);
	void update(void);
	void resize(int, int);
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
		WS_EX_APPWINDOW,
		szAppName,																				//Name of Wnd class
		TEXT("Shaunak Jotawar Perspective"),													//Title bar text
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,					//style of window
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

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
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
	//Function Prototypes
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
	pfd.cDepthBits = 32;	//24 bits can also be used

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

	//Here starts OpenGL's code
	//Clear the Screen using black color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//Depth Related Functions for 3Dimensions
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//Initialization of matrix arrays********
	//IdentityMatrix
	IdentityMatrix[0] = 1.0f;
	IdentityMatrix[1] = 0.0f;
	IdentityMatrix[2] = 0.0f;
	IdentityMatrix[3] = 0.0f;
	IdentityMatrix[4] = 0.0f;
	IdentityMatrix[5] = 1.0f;
	IdentityMatrix[6] = 0.0f;
	IdentityMatrix[7] = 0.0f;
	IdentityMatrix[8] = 0.0f;
	IdentityMatrix[9] = 0.0f;
	IdentityMatrix[10] = 1.0f;
	IdentityMatrix[11] = 0.0f;
	IdentityMatrix[12] = 0.0f;
	IdentityMatrix[13] = 0.0f;
	IdentityMatrix[14] = 0.0f;
	IdentityMatrix[15] = 1.0f;

	//TranslationMatrix
	TranslationMatrix[0] = 1.0f;
	TranslationMatrix[1] = 0.0f;
	TranslationMatrix[2] = 0.0f;
	TranslationMatrix[3] = 0.0f;
	TranslationMatrix[4] = 0.0f;
	TranslationMatrix[5] = 1.0f;
	TranslationMatrix[6] = 0.0f;
	TranslationMatrix[7] = 0.0f;
	TranslationMatrix[8] = 0.0f;
	TranslationMatrix[9] = 0.0f;
	TranslationMatrix[10] = 1.0f;
	TranslationMatrix[11] = 0.0f;
	TranslationMatrix[12] = 0.0f;
	TranslationMatrix[13] = 0.0f;
	TranslationMatrix[14] = -4.0f;
	TranslationMatrix[15] = 1.0f;

	//ScalingMatrix
	ScaleMatrix[0] = 8.0f;
	ScaleMatrix[1] = 0.0f;
	ScaleMatrix[2] = 0.0f;
	ScaleMatrix[3] = 0.0f;
	ScaleMatrix[4] = 0.0f;
	ScaleMatrix[5] = 8.0f;
	ScaleMatrix[6] = 0.0f;
	ScaleMatrix[7] = 0.0f;
	ScaleMatrix[8] = 0.0f;
	ScaleMatrix[9] = 0.0f;
	ScaleMatrix[10] = 8.0f;
	ScaleMatrix[11] = 0.0f;
	ScaleMatrix[12] = 0.0f;
	ScaleMatrix[13] = 0.0f;
	ScaleMatrix[14] = 0.0f;
	ScaleMatrix[15] = 1.0f;


	resize(WIN_WIDTH, WIN_HEIGHT);
	return 0;
}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(IdentityMatrix);

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	if (width <= height)
	{
		/*
		glOrtho(
			-100.0f, 100.0f,
			(-100.0f * ((GLfloat)height / (GLfloat)width)),
			(100.0f * ((GLfloat)height / (GLfloat)width)),
			-100.0f, 100.0f
		);
		OrthoMatrix = {
			0.01f, 0, 0, 0,
			0, ((GLfloat)width / (100.0f * (GLfloat)height)), 0, 0,
			0, 0, -0.01f, 0,
			0, 0, 0, 1
		};*/
		OrthoMatrix[0] = 0.01f;
		OrthoMatrix[1] = 0.0f;
		OrthoMatrix[2] = 0.0f;
		OrthoMatrix[3] = 0.0f;
		OrthoMatrix[4] = 0.0f;
		OrthoMatrix[5] = ((GLfloat)width / (100.0f * (GLfloat)height));
		OrthoMatrix[6] = 0.0f;
		OrthoMatrix[7] = 0.0f;
		OrthoMatrix[8] = 0.0f;
		OrthoMatrix[9] = 0.0f;
		OrthoMatrix[10] = -0.01f;
		OrthoMatrix[11] = 0.0f;
		OrthoMatrix[12] = 0.0f;
		OrthoMatrix[13] = 0.0f;
		OrthoMatrix[14] = 0.0f;
		OrthoMatrix[15] = 1.0f;

		glMultMatrixf(OrthoMatrix);
	}
	else
	{
		/*
		glOrtho(
			(-100.0f * ((GLfloat)width / (GLfloat)height)),
			(100.0f * ((GLfloat)width / (GLfloat)height)),
			-100.0f, 100.0f, -100.0f, 100.0f
		);
		OrthoMatrix = {
			(GLfloat)height / (100.0f * (GLfloat)width), 0, 0, 0,
			0, 0.01f, 0, 0,
			0, 0, -0.01f, 0,
			0, 0, 0, 1
		};*/
		OrthoMatrix[0] = ((GLfloat)height / (100.0f * (GLfloat)width));
		OrthoMatrix[1] = 0.0f;
		OrthoMatrix[2] = 0.0f;
		OrthoMatrix[3] = 0.0f;
		OrthoMatrix[4] = 0.0f;
		OrthoMatrix[5] = 0.01f;
		OrthoMatrix[6] = 0.0f;
		OrthoMatrix[7] = 0.0f;
		OrthoMatrix[8] = 0.0f;
		OrthoMatrix[9] = 0.0f;
		OrthoMatrix[10] = -0.01f;
		OrthoMatrix[11] = 0.0f;
		OrthoMatrix[12] = 0.0f;
		OrthoMatrix[13] = 0.0f;
		OrthoMatrix[14] = 0.0f;
		OrthoMatrix[15] = 1.0f;

		glMultMatrixf(OrthoMatrix);
	}
}

void display(void)
{
	//variable declarations
	
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//PYRAMID********
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(IdentityMatrix);
	
	//CUBE********
	//glLoadIdentity();
	
	//glTranslatef(0.0f, 0.0f, -5.0f);
	glMultMatrixf(TranslationMatrix);

	//glScalef(0.75f, 0.75f, 0.75f);
	glMultMatrixf(ScaleMatrix);

	//glRotatef(angleCube, 1.0f, 1.0f, 1.0f);
	float angle = angleCube * M_PI / 180.0f;

	//X Rotation Matrix
	RotationMatrix_X[0] = 1.0f;
	RotationMatrix_X[1] = 0.0f;
	RotationMatrix_X[2] = 0.0f;
	RotationMatrix_X[3] = 0.0f;
	RotationMatrix_X[4] = 0.0f;
	RotationMatrix_X[5] = cosf(angle);
	RotationMatrix_X[6] = sinf(angle);
	RotationMatrix_X[7] = 0.0f;
	RotationMatrix_X[8] = 0.0f;
	RotationMatrix_X[9] = -sinf(angle);
	RotationMatrix_X[10] = cosf(angle);
	RotationMatrix_X[11] = 0.0f;
	RotationMatrix_X[12] = 0.0f;
	RotationMatrix_X[13] = 0.0f;
	RotationMatrix_X[14] = 0.0f;
	RotationMatrix_X[15] = 1.0f;

	//Y RotationMatrix
	RotationMatrix_Y[0] = cosf(angle);
	RotationMatrix_Y[1] = 0.0f;
	RotationMatrix_Y[2] = -sinf(angle);
	RotationMatrix_Y[3] = 0.0f;
	RotationMatrix_Y[4] = 0.0f;
	RotationMatrix_Y[5] = 1.0f;
	RotationMatrix_Y[6] = 0.0f;
	RotationMatrix_Y[7] = 0.0f;
	RotationMatrix_Y[8] = sinf(angle);
	RotationMatrix_Y[9] = 0.0f;
	RotationMatrix_Y[10] = cosf(angle);
	RotationMatrix_Y[11] = 0.0f;
	RotationMatrix_Y[12] = 0.0f;
	RotationMatrix_Y[13] = 0.0f;
	RotationMatrix_Y[14] = 0.0f;
	RotationMatrix_Y[15] = 1.0f;

	//Z RotationMatrix
	RotationMatrix_Z[0] = cosf(angle);
	RotationMatrix_Z[1] = sinf(angle);
	RotationMatrix_Z[2] = 0.0f;
	RotationMatrix_Z[3] = 0.0f;
	RotationMatrix_Z[4] = -sinf(angle);
	RotationMatrix_Z[5] = cosf(angle);
	RotationMatrix_Z[6] = 0.0f;
	RotationMatrix_Z[7] = 0.0f;
	RotationMatrix_Z[8] = 0.0f;
	RotationMatrix_Z[9] = 0.0f;
	RotationMatrix_Z[10] = 1.0f;
	RotationMatrix_Z[11] = 0.0f;
	RotationMatrix_Z[12] = 0.0f;
	RotationMatrix_Z[13] = 0.0f;
	RotationMatrix_Z[14] = 0.0f;
	RotationMatrix_Z[15] = 1.0f;

	glMultMatrixf(RotationMatrix_X);
	glMultMatrixf(RotationMatrix_Y);
	glMultMatrixf(RotationMatrix_Z);

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);

	//Front
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	//Right
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);

	//Back
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	//Left
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);

	//Top
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f);

	//Bottom
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);

	glEnd();

	SwapBuffers(ghdc);
}

void update(void)
{
	//code
	angleCube += 0.3f;
	if (angleCube >= 360.0f)
		angleCube -= 360.0f;
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

	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	if (gpFile)
	{
		fprintf(gpFile, "Log File closed successfully!\n");
		fclose(gpFile);
		gpFile = NULL;
	}
}
