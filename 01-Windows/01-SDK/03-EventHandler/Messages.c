//Header Files
#include <windows.h>

//Global Function Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int iCmdShow)
{
	//Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyWindow");

	//code
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
	while (GetMessage(&msg, NULL, 0, 0))	//GetMessage() is a blocking API
	{
		TranslateMessage(&msg);
		//WndProc(msg.hwnd, msg.message, msg.wParam, msg,lParam);		DispatchMessage() calls wndproc 
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
	case WM_CREATE:
		MessageBox(hwnd, TEXT("This is WM_CREATE Message!"), TEXT("Hello World :)"), MB_YESNO);
		break;
	case WM_KEYDOWN:
		MessageBox(hwnd, TEXT("This is WM_KEYDOWN Message!"), TEXT("Message"), MB_OK);
		break;
	case WM_LBUTTONDOWN:
		MessageBox(hwnd, TEXT("This is WM_LBUTTONDOWN Message!"), TEXT("Message"), MB_OK);
		break;
	case WM_RBUTTONDOWN:
		MessageBox(hwnd, TEXT("This is WM_RBUTTONDOWN Message!"), TEXT("Message"), MB_OK);
		break;
	case WM_DESTROY:
		MessageBox(hwnd, TEXT("This is WM_DESTROY Message!"), TEXT("Goodbye :("), MB_OK);
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
