#include <windows.h>
#include <stdio.h>

LRESULT CALLBACK WndProc(HWND win, UINT msgt, WPARAM w, LPARAM l) {

	switch(msgt) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}

	return DefWindowProc(win, msgt, w, l);
}

int main() {
	HINSTANCE instance = GetModuleHandle(NULL);

	WNDCLASS cls;
	cls.lpszClassName = "dflclass";
	cls.hInstance = instance;
	cls.lpszMenuName = NULL;
	cls.lpfnWndProc = WndProc;
	cls.cbClsExtra = 0;
	cls.cbWndExtra = 0;
	cls.style = CS_HREDRAW | CS_VREDRAW;
	cls.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	cls.hCursor = LoadCursor(NULL, IDC_ARROW);
	cls.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClass(&cls)) {
		CloseHandle(instance);
		puts("unable to register class");
		return 1;
	}

	HWND window = CreateWindow(
			cls.lpszClassName, 
			"Default window", 
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			800,
			600,
			NULL,
			NULL,
			instance,
			NULL);

	if(window == INVALID_HANDLE_VALUE) {
		CloseHandle(instance);
		puts("unable to create window (CreateWindow)");
		return 1;
	}

	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CloseHandle(instance);
	CloseHandle(window);

	return (int) msg.wParam;
}
