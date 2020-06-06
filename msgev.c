#include <windows.h>
#include <stdio.h>

HWND label;

void PopulateWindow(HWND win) {
	label = CreateWindow(L"static",
		L"Press one of the key below",
		WS_VISIBLE | WS_CHILD,
		10,
		10,
		200,
		100,
		win,
		(HMENU)1,
		NULL,
		NULL);

	HWND btn = CreateWindow(L"button",
		L"Exit",
		WS_VISIBLE | WS_CHILD,
		100,
		100,
		50,
		40,
		win,
		(HMENU)1,
		NULL,
		NULL);

	HWND okbtn = CreateWindow(L"button",
		L"OK",
		WS_VISIBLE | WS_CHILD,
		200,
		100,
		50,
		40,
		win,
		(HMENU)2,
		NULL,
		NULL);

	SendMessage(label, WM_SETFONT, GetStockObject(DEFAULT_GUI_FONT), TRUE);
	SendMessage(btn, WM_SETFONT, GetStockObject(DEFAULT_GUI_FONT), TRUE);
	SendMessage(okbtn, WM_SETFONT, GetStockObject(DEFAULT_GUI_FONT), TRUE);
}

LRESULT CALLBACK WndProc(HWND win, UINT msgt, WPARAM w, LPARAM l) {
	switch (msgt) {
	case WM_CREATE:
		PopulateWindow(win);
		break;
	case WM_COMMAND: {
		if (LOWORD(w) == 1) {
			DestroyWindow(win);
		}
		else if (LOWORD(w) == 2) {
			static int i;
			if (i == 0) {
				SetWindowTextA(label, "Ok pressed");
				i = 1;
			}
			else if (i) {
				SetWindowTextA(label, "OK pressed again");
				i = 0;
			}
		}
		break;
	}
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

	if (!RegisterClass(&cls)) {
		CloseHandle(instance);
		puts("unable to register class");
		return 1;
	}

	HWND window = CreateWindow(
		cls.lpszClassName,
		L"Default window",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		300,
		200,
		NULL,
		NULL,
		instance,
		NULL);

	if (window == INVALID_HANDLE_VALUE) {
		CloseHandle(instance);
		puts("unable to create window (CreateWindow)");
		return 1;
	}

	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
