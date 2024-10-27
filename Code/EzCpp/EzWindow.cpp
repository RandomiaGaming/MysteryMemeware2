// Approved 10/26/2024

#include "EzWindow.h"
#include "EzError.h"

ATOM EzRegisterClass(const EzClassSettings* settings) {
	WNDCLASS wc = { };
	if (settings->Name == NULL) {
		wc.lpszClassName = EzDefaultClassName;
	}
	else {
		wc.lpszClassName = settings->Name;
	}
	if (settings->WndProc == NULL) {
		wc.lpfnWndProc = DefWindowProc;
	}
	else {
		wc.lpfnWndProc = settings->WndProc;
	}
	// NULL tells windows to use the default icon so no need to explicitly set it.
	wc.hIcon = settings->Icon;
	if (settings->Cursor == NULL) {
		// No need to free cursors as they are reused across the application instead of being copied for each call to LoadCursor.
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		if (wc.hCursor == NULL) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
	}
	else {
		wc.hCursor = settings->Cursor;
	}
	if (!settings->CustomBackPaint) {
		// No need to free this brush in normal circumstances because UnregisterClass will free it.
		wc.hbrBackground = CreateSolidBrush(RGB(settings->BackColorR, settings->BackColorG, settings->BackColorB));
		if (wc.hbrBackground == NULL) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
	}
	else {
		wc.hbrBackground = NULL;
	}
	wc.style = settings->Styles;
	if (!settings->DontRedrawOnSize) {
		wc.style |= CS_HREDRAW | CS_VREDRAW;
	}
	if (settings->UniversalDropShadow) {
		wc.style |= CS_DROPSHADOW;
	}
	if (!settings->IgnoreDoubleClicks) {
		wc.style |= CS_DBLCLKS;
	}
	if (settings->NoCloseOption) {
		wc.style |= CS_NOCLOSE;
	}
	if (settings->SaveClippedGraphics) {
		wc.style |= CS_SAVEBITS;
	}
	if (!settings->ThisThreadOnly) {
		wc.style |= CS_GLOBALCLASS;
	}

	wc.cbClsExtra = 0; // Allocate 0 extra bytes after the class declaration.
	wc.cbWndExtra = 0; // Allocate 0 extra bytes after windows of this class.
	wc.hInstance = GetModuleHandle(NULL); // WndProc is in the current hInstance.
	if (wc.hInstance == NULL) {
		DeleteObject(wc.hbrBackground);
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	wc.lpszMenuName = NULL; // Windows of this class have no default menu.

	ATOM output = RegisterClass(&wc);
	if (output == 0) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return output;
}

HWND EzCreateWindow(const EzWindowSettings* settings) {
	LPCWSTR title;
	if (settings->Title == NULL) {
		title = EzDefaultWindowTitle;
	}
	else {
		title = settings->Title;
	}

	LPCWSTR className;
	if (settings->ClassName == NULL) {
		className = EzDefaultClassName;
	}
	else {
		className = settings->ClassName;
	}

	int initialX = settings->InitialX;
	if (settings->InitialX == 0x80000000) {
		initialX = GetSystemMetrics(SM_CXSCREEN) / 4;
	}
	int initialY = settings->InitialY;
	if (settings->InitialY == 0x80000000) {
		initialY = GetSystemMetrics(SM_CYSCREEN) / 4;
	}
	int initialWidth = settings->InitialWidth;
	if (settings->InitialWidth == 0xFFFFFFFF) {
		initialWidth = GetSystemMetrics(SM_CXSCREEN) / 2;
	}
	int initialHeight = settings->InitialHeight;
	if (settings->InitialHeight == 0xFFFFFFFF) {
		initialHeight = GetSystemMetrics(SM_CYSCREEN) / 2;
	}

	DWORD styles = settings->Styles;
	switch (settings->StylePreset) {
	case EzWindowStylePreset::Normal:
		styles |= WS_OVERLAPPEDWINDOW;
		break;
	case EzWindowStylePreset::Popup:
		styles |= WS_POPUPWINDOW;
		break;
	case EzWindowStylePreset::Boarderless:
		styles |= WS_POPUP;
		break;
	case EzWindowStylePreset::DontTouchMyStyles:
	default:
		break;
	}
	if (!settings->LaunchHidden) {
		styles |= WS_VISIBLE;
	}

	DWORD extendedStyles = settings->ExtendedStyles;
	if (settings->DragNDropFiles) {
		extendedStyles |= WS_EX_ACCEPTFILES;
	}
	if (settings->IgnoreFocusSwitch) {
		extendedStyles |= WS_EX_NOACTIVATE;
	}
	if (settings->TopMost) {
		extendedStyles |= WS_EX_TOPMOST;
	}
	if (settings->HideInTaskbar) {
		extendedStyles |= WS_EX_TOOLWINDOW;
	}

	if (settings->ClassAtom != 0) {
		className = MAKEINTATOM(settings->ClassAtom);
	}
	else {
		className = settings->ClassName;
	}

	HWND output = CreateWindowEx(
		extendedStyles,
		className,
		title,
		styles,
		initialX,
		initialY,
		initialWidth,
		initialHeight,
		NULL, // No parent window.
		NULL, // No target menu.
		GetModuleHandle(NULL), // Current process instance.
		NULL // No additional data.
	);
	if (output == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	return output;
}
void EzShowWindow(HWND window, int showCommand) {
	SetLastError(0);
	ShowWindow(window, showCommand);
	DWORD lastError = GetLastError();
	if (lastError != 0) {
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}
}

BOOL EzMessagePumpOne(HWND window, BOOL wait) {
	if (EzWindowIsDestroyed(window)) {
		throw EzError(L"window has been destroyed");
	}
	if (wait)
	{
		MSG message = { };
		if (GetMessage(&message, window, 0, 0)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		return TRUE;
	}
	else
	{
		MSG message = { };
		if (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
}
BOOL EzMessagePumpAll(HWND window) {
	if (EzWindowIsDestroyed(window)) {
		throw EzError(L"window has been destroyed");
	}
	BOOL output = FALSE;
	MSG message = { };
	while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
		output = TRUE;
	}
	return output;
}
BOOL EzMessagePumpRun(HWND window) {
	if (EzWindowIsDestroyed(window)) {
		throw EzError(L"window has been destroyed");
	}
	BOOL output = FALSE;
	MSG message = { };
	while (GetMessage(&message, window, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
		output = TRUE;
	}
	return output;
}
BOOL EzMessagePumpOne(BOOL wait) {
	if (wait)
	{
		MSG message = { };
		if (GetMessage(&message, NULL, 0, 0)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		return TRUE;
	}
	else
	{
		MSG message = { };
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
}
BOOL EzMessagePumpAll() {
	BOOL output = FALSE;
	MSG message = { };
	while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
		output = TRUE;
	}
	return output;
}
void EzMessagePumpRun() {
	MSG message = { };
	while (GetMessage(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

BOOL EzWindowIsShowing(HWND window) {
	return !EzWindowIsDestroyed(window) && IsWindowVisible(window);
}
BOOL EzWindowIsDestroyed(HWND window) {
	return !IsWindow(window);
}

void EzSetWindowData(HWND window, void* data) {
	DWORD lastError = 0;

	SetLastError(0);
	if (SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data)) == 0) {
		lastError = GetLastError();
		if (lastError != 0) {
			EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
		}
	}
}
void* EzGetWindowData(HWND window) {
	DWORD lastError = 0;

	SetLastError(0);
	LONG_PTR userData = GetWindowLongPtr(window, GWLP_USERDATA);
	if (userData == 0) {
		lastError = GetLastError();
		if (lastError != 0) {
			EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
		}
	}

	return reinterpret_cast<void*>(userData);
}