//Header files
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "Win1.h"

//OpenGL Header files and libraries
#include <GL/gl.h>

//Macro declarations
#define X GetSystemMetrics(SM_CXSCREEN);
#define Y GetSystemMetrics(SM_CYSCREEN);
#define WIN_WIDTH 640
#define WIN_HEIGHT 360

//Global Function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL;
BOOL gbFullscreen = FALSE;
BOOL glActiveWindow = FALSE;
FILE gpFile = NULL;

//Entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Function prototypes
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);

	//variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName = TEXT("MyWindow");
	BOOL bDone = FALSE;
	int iRetVal = 0;

	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File creation failed. Exiting now!"), TEXT("File I/O error!"), MB_OK | MB_ICONEXCLAMATION);
		exit(0);
	}
	else
		fprintf(gpFile, "Log File Created successfully\n");

	//Initialization of wndclass
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(MYCURSOR));
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;

	//register class
	RegisterClassEx(&wndclass);

	//create window
	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		szAppName,
		TEXT("Shaunak Jotawar OpenGL"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
		X / 2 - WIN_WIDTH / 2,
		Y / 2 - WIN_HEIGHT / 2,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL, NULL,
		hInstance, NULL
	);

	ghwnd = hwnd;

	//initialize
	iRetVal = initialize();
	if (iRetVal == -1)
	{
		fprintf("Choose Pixel Format failed!!\n");
		uninitialize();
	}
	else if (iRetVal == -2)
	{
		fprintf("Set Pixel Format failed!!\n");
		uninitialize();
	}
	else if (iRetVal == -3)
	{
		fprintf("Create OpenGl context failed!!\n");
		uninitialize();
	}
	else if (iRetVal == -4)
	{
		fprintf("Set OpenGL as current context failed!!\n");
		uninitialize();
	}
	else
		fprintf("Initialization successful!!\n");

	//show window
	ShowWindow(hwnd, iCmdShow);

	//Foregrounding and focusing the window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//Game Loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message = WM_QUIT)
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
				display();
				update();
			}
		}
	}

	return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Function Prototypes
	void ToggleFullscreen(void);
	void resize(void);
	void uninitialize(void);

	//code
	switch (iMsg)
	{
		case WM_SETFOCUS
	}
}
