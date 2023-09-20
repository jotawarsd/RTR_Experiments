//Header Files
#include <windows.h>

//Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");

	//code
	//Initialization of WNDCLASSEX structure
	wndclass.cbSize = sizeof(WNDCLASSEX);							//not included in WNDCLASS (only WNDCLASSEX)
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
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);				//not included in WNDCLASS

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
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//variable declarations
	HDC hdc;								//Handle to device context
	PAINTSTRUCT ps;
	RECT rc;
	TCHAR str[] = TEXT("Hello Neo");
	static int iColorFlag = 0;

	//code
	switch (iMsg)
	{
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

	case WM_CHAR:
		switch (wParam)
		{
		case 'R':
		case 'r':
			iColorFlag = 1;
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		case 'G':
		case 'g':
			iColorFlag = 2;
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		case 'B':
		case 'b':
			iColorFlag = 3;
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		default:
			iColorFlag = 0;
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}
		break;

	case WM_PAINT:
		GetClientRect(hwnd, &rc);			//Gets client rectangle

		hdc = BeginPaint(hwnd, &ps);		//Begin paint gets the handle to device context and returns it to the variable hdc, stores additional info in ps
		SetBkColor(hdc, RGB(0, 0, 0));		//sets the background color

		if (iColorFlag == 1)
			SetTextColor(hdc, RGB(255, 0, 0));
		else if (iColorFlag == 2)
			SetTextColor(hdc, RGB(0, 255, 0));
		else if (iColorFlag == 3)
			SetTextColor(hdc, RGB(0, 0, 255));
		else
			SetTextColor(hdc, RGB(255, 255, 255));	//sets the color of text

		DrawText(hdc, str, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);	//draws the text
		EndPaint(hwnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
