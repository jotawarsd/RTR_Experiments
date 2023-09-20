//Header Files
#include <windows.h>
#include <stdio.h>	//For file I/O functions
#include <stdlib.h>	//For Exit function 

//Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Global variable declarations
FILE *gpFile = NULL;

//Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");

	//code
	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Creation of Log File Failed. Exiting now!"), TEXT("File I/O Error!"), MB_OK | MB_ICONEXCLAMATION);
		exit(0);
	}
	else
		fprintf(gpFile, "Log file created successfully!\n");

	//Initialization of WNDCLASSEX structure
	wndclass.cbSize = sizeof(WNDCLASSEX);    //not included in WNDCLASS (only WNDCLASSEX)
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	//Client area color (Canvas)
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);    //not included in WNDCLASS

	//Registering Above class
	RegisterClassEx(&wndclass);

	//Creating the window
	hwnd = CreateWindow(szAppName,				//Name of Wnd class
		TEXT("Shaunak Deepak Jotawar"),			//Title bar text
		WS_OVERLAPPEDWINDOW,					//style of window
		CW_USEDEFAULT,							//x coordinate of top left corner
		CW_USEDEFAULT,							//y coordinate of top left corner
		CW_USEDEFAULT,							//width
		CW_USEDEFAULT,							//height
		NULL,									//parent window (NULL = Desktop)
		NULL,									//Handle to Menu (NULL = no menu)
		hInstance,								//handle to instance
		NULL);									//Extra info for window

	//Show Window
	ShowWindow(hwnd, iCmdShow);

	//update the window
	UpdateWindow(hwnd);

	//Message loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return ((int)msg.wParam);
}

//Callback Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg,
	WPARAM wParam, LPARAM lParam)
{
	//code
	switch (iMsg)
	{
	//-----<start>
	case WM_DESTROY:
		if (gpFile)
		{
			fprintf(gpFile, "Log file closed successfully!\n");
			fclose(gpFile);
			gpFile = NULL;
		}
		PostQuitMessage(0);
		break;
	//-----<end> : This code block is called "Message Handler"
	default:
		break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
