//Header Files
#include <windows.h>
#include "OGL.h"
#include <stdio.h>
#include <stdlib.h>

//OpenGL Header files
#include <GL/gl.h>
#include <GL/glu.h>

//math header
#define _USE_MATH_DEFINES 1
#include <math.h>

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

//FPS variables
float DesiredFPS = 165.0f;
float deltaTime = 0.0f;
float disp = 0.0f;
float v;

//camera
struct camera
{
	float x = 0.0f, y = 0.0f, z = -10.0f;
	float camAngle = M_PI;
	float fx = 90.0f * sin(camAngle); 
	float fy = 0.0f;
	float fz = -90.0f * cos(camAngle);
	float height = 0.4f;
	float up = 0.0f;
}cam;

float treeAngle = 0.0f;

GLUquadric *quadric;

//Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Function Declarations
	int initialize(void);
	void display(void);
	void update(void);
	void resize(int, int);
	void uninitialize(void);
	void ToggleFullscreen(void);

	//Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");
	BOOL bDone = FALSE;
	int iRetVal = 0;

	LARGE_INTEGER startingTime, endingTime;
	LARGE_INTEGER frequency;

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
		szAppName,
		TEXT("Shaunak Jotawar Perspective"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		X / 2 - (WIN_WIDTH / 2),
		Y / 2 - (WIN_HEIGHT / 2),
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

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

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startingTime);
	float timePerFrame = 1.0 / DesiredFPS;
	float inverseFrequency = 1.0 / (float)frequency.QuadPart;

	ToggleFullscreen();

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
				//updating the scene
				update();

				//Rendering the scene
				display();

				//fps locking
				QueryPerformanceCounter(&endingTime);
				deltaTime = ((float)(endingTime.QuadPart - startingTime.QuadPart) * inverseFrequency);
				while(deltaTime < timePerFrame)
				{
					QueryPerformanceCounter(&endingTime);
					deltaTime = ((float)(endingTime.QuadPart - startingTime.QuadPart) * inverseFrequency);
				}
				startingTime = endingTime;
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
	void resize(int, int);

	//code
	switch (iMsg)
	{
	case WM_CREATE:
		cam.camAngle += 0.01f;
		break;
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
		case 'w':
		case 'W':
			cam.x += (cam.fx + 5.0f) * 0.001f;
			cam.z += (cam.fz + 5.0f) * 0.001f;
			break;
		case 's':
		case 'S':
			cam.x -= (cam.fx + 5.0f) * 0.001f;
			cam.z -= (cam.fz + 5.0f) * 0.001f;
			break;
		case 'a':
		case 'A':
			cam.camAngle -= 0.01f;
			cam.fx = 90.0f * sin(cam.camAngle);
			cam.fz = -90.0f * cos(cam.camAngle);
			break;
		case 'd':
		case 'D':
			cam.camAngle += 0.01f;
			cam.fx = 90.0f * sin(cam.camAngle);
			cam.fz = -90.0f * cos(cam.camAngle);
			break;
		case 't':
		case 'T':
			cam.height += 0.5f;
			break;
		case 'g':
		case 'G':
			cam.height -= 0.5f;
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
			cam.up += 30.0f;
			break;
		case VK_DOWN:
			cam.up -= 30.0f;
			break;
		case VK_RIGHT:
			treeAngle += 0.5f;
			break;
		case VK_LEFT:
			treeAngle -= 0.5f;
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

			//ShowCursor(FALSE);
			gbFullscreen = TRUE;
		}
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wp);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOSIZE | SWP_FRAMECHANGED);
		//ShowCursor(TRUE);
		gbFullscreen = FALSE;
	}
}

int initialize(void)
{
	//Function Prototypes
	void resize(int, int);
	//float *getBuildingCoords(int, int, int, int *);
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

	//Depth Related Functions for 3Dimensions
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//enable lighting and textures
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_POINT_SIZE);
	glEnable(GL_POINT_SMOOTH);
	//glEnable(GL_LIGHTING);

	quadric = gluNewQuadric();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	resize(WIN_WIDTH, WIN_HEIGHT);
	return 0;
}

void resize(int width, int height)
{
	//code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, ((GLfloat)width / (GLfloat)height), 0.1f, 1000.0f);
}

void display(void)
{
	//function Prototypes
	void sceneOneDraw(void);
	void tree(float);

	//variable declarations

	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_LIGHT0);

	gluLookAt(
		cam.x, cam.height, cam.z,
		(cam.x + cam.fx) * 5.0f, cam.height + cam.up, (cam.z + cam.fz) * 5.0f,
		0.0f, 1.0f, 0.0f
	);

	tree(1.0f);
	
	SwapBuffers(ghdc);
}

void update(void)
{
	//code
	disp += 0.00002f;
}

void tree(float length)
{
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	gluCylinder(quadric, length / 7.0f, length / 10.0f, length, 10, 10);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glPointSize(20.0);
	glBegin(GL_POINTS);
	glVertex3f(0.0f, length, 0.0f);
	glEnd();
	glColor3f(0.5f, 0.25f, 0.0f);

	glTranslatef(0.0f, length, 0.0f);
	if (length >= 0.05)
	{
		glPushMatrix();
		glRotatef(treeAngle, 0.0f, 0.0f, 1.0f);
		tree(0.67 * length);
		glPopMatrix();
		glPushMatrix();
		glRotatef(-treeAngle, 0.0f, 0.0f, 1.0f);
		tree(0.67 * length);
		glPopMatrix();
	}
}

void terrainGrid(float breadth, float length, float mul, GLuint texture)
{
	//variable declarations
	int x, y;

	//code
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texture);
	//glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	for (y = 0; y < breadth; y++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (x = 0; x < length + 1; x++)
		{
			glTexCoord2f(x / (length / mul), (y + 1) / (breadth / (mul)));
			glVertex3f(x - length / 2.0f, y + 1 - breadth / 2.0f, 0.0f);
			glTexCoord2f(x / (length / mul), (y) / (breadth / (mul)));
			glVertex3f(x - length / 2.0f, y - breadth / 2.0f, 0.0f);
		}
		glEnd();
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
	glEnable(GL_LIGHTING);
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
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, Texture);
		glBindTexture(GL_TEXTURE_2D, *Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		glBindTexture(GL_TEXTURE, 0);
		DeleteObject(hBitmap);
	}

	return bResult;
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
