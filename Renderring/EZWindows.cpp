#include "EZWindows.h"
#include "Helper.h"
#include <exception>

void EZ::RegisterClass(EZ::ClassSettings settings) {
	WNDCLASS wc = { };
	if (settings.Name == NULL) {
		wc.lpszClassName = EZ::DefaultClassName;
	}
	else {
		wc.lpszClassName = settings.Name;
	}
	if (settings.WndProc == NULL) {
		wc.lpfnWndProc = DefWindowProc;
	}
	else {
		wc.lpfnWndProc = settings.WndProc;
	}
	// NULL tells windows to use the default icon so no need to explicitly set it.
	wc.hIcon = settings.Icon;
	if (settings.Cursor == NULL) {
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	}
	else {
		wc.hCursor = settings.Cursor;
	}
	if (!settings.CustomBackPaint) {
		wc.hbrBackground = CreateSolidBrush(RGB(settings.BackColorR, settings.BackColorG, settings.BackColorB));
		if (wc.hbrBackground == NULL) {
			PrintLastError(); return;
		}
	}
	else {
		wc.hbrBackground = NULL;
	}
	wc.style = settings.Styles;
	if (!settings.DontRedrawOnSize) {
		wc.style |= CS_HREDRAW | CS_VREDRAW;
	}
	if (settings.UniversalDropShadow) {
		wc.style |= CS_DROPSHADOW;
	}
	if (!settings.IgnoreDoubleClicks) {
		wc.style |= CS_DBLCLKS;
	}
	if (settings.NoCloseOption) {
		wc.style |= CS_NOCLOSE;
	}
	if (settings.SaveClippedGraphics) {
		wc.style |= CS_SAVEBITS;
	}

	wc.cbClsExtra = 0; // Allocate 0 extra bytes after the class declaration.
	wc.cbWndExtra = 0; // Allocate 0 extra bytes after windows of this class.
	wc.hInstance = GetModuleHandle(NULL); // The use the current hInstance.
	wc.lpszMenuName = NULL; // Windows of this class have no default menu.

	if (::RegisterClass(&wc) == 0) {
		PrintLastError(); return;
	}
}



EZ::Window::Window(EZ::WindowSettings settings) {
	if (settings.Title == NULL) {
		settings.Title = DefaultWindowTitle;
	}
	if (settings.ClassName == NULL) {
		settings.ClassName = DefaultClassName;
	}
	if (settings.InitialX == CW_USEDEFAULT) {
		settings.InitialX = GetSystemMetrics(SM_CXSCREEN) / 4;
	}
	if (settings.InitialY == CW_USEDEFAULT) {
		settings.InitialY = GetSystemMetrics(SM_CYSCREEN) / 4;
	}
	if (settings.InitialWidth == CW_USEDEFAULT) {
		settings.InitialWidth = GetSystemMetrics(SM_CXSCREEN) / 2;
	}
	if (settings.InitialHeight == CW_USEDEFAULT) {
		settings.InitialHeight = GetSystemMetrics(SM_CYSCREEN) / 2;
	}
	switch (settings.StylePreset) {
	case Normal:
		settings.Styles |= WS_OVERLAPPEDWINDOW;
		break;
	case Popup:
		settings.Styles |= WS_POPUPWINDOW;
		break;
	case Boarderless:
		settings.Styles |= WS_POPUP;
		break;
	case DontTouchMyStyles:
	default:
		break;
	}
	if (!settings.LaunchHidden) {
		settings.Styles |= WS_VISIBLE;
	}
	if (settings.DragNDropFiles) {
		settings.ExtendedStyles |= WS_EX_ACCEPTFILES;
	}
	if (settings.IgnoreFocusSwitch) {
		settings.ExtendedStyles |= WS_EX_NOACTIVATE;
	}
	if (settings.TopMost) {
		settings.ExtendedStyles |= WS_EX_TOPMOST;
	}
	if (settings.HideInTaskbar) {
		settings.ExtendedStyles |= WS_EX_TOOLWINDOW;
	}

	_settings = settings;
	_handle = CreateWindowEx(
		settings.ExtendedStyles,
		settings.ClassName,
		settings.Title,
		settings.Styles,
		settings.InitialX,
		settings.InitialY,
		settings.InitialWidth,
		settings.InitialHeight,
		NULL, // No parent window.
		NULL, // No target menu.
		GetModuleHandle(NULL), // Current process instance.
		NULL // No additional data.
	);
	if (_handle == 0) {
		PrintLastError(); return;
	}

	SetWindowLongPtr(_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}
void EZ::Window::Show(int showCommand) {
	if (IsDestroyed() || IsShowing()) {
		throw std::exception("Window must not be destroyed or already showing to show window.");
	}
	if (showCommand < 0) {
		showCommand = SW_SHOWDEFAULT;
	}
	ShowWindow(_handle, showCommand);
}
BOOL EZ::Window::ProcessOneMessage(BOOL wait) {
	if (!IsShowing()) {
		throw std::exception("Window must be showing to processing messages.");
	}

	MSG msg = { };
	if (wait)
	{
		if (GetMessage(&msg, _handle, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			return TRUE;
		}
	}
	else
	{
		if (PeekMessage(&msg, _handle, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			return TRUE;
		}
	}
	return FALSE;
}
BOOL EZ::Window::ProcessUntilClear() {
	if (!IsShowing()) {
		throw std::exception("Window must be showing to processing messages.");
	}

	BOOL output = FALSE;
	MSG msg = { };
	while (!IsDestroyed() && PeekMessage(&msg, _handle, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		output = TRUE;
	}

	return output;
}
BOOL EZ::Window::RunMessagePump() {
	if (!IsShowing()) {
		throw std::exception("Window must be showing to processing messages.");
	}

	BOOL output = FALSE;
	MSG msg = { };
	while (!IsDestroyed() && GetMessage(&msg, _handle, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		output = TRUE;
	}

	return output;
}
EZ::Window::~Window() {
	if (!IsDestroyed()) {
		DestroyWindow(_handle);
	}
}
HWND EZ::Window::GetHandle() const {
	return _handle;
}
RECT EZ::Window::GetBounds() const {
	RECT output;
	GetWindowRect(_handle, &output);
	return output;
}
EZ::WindowSettings EZ::Window::GetSettings() const {
	return _settings;
}
BOOL EZ::Window::IsShowing() const {
	return IsWindow(_handle) && IsWindowVisible(_handle);
}
BOOL EZ::Window::IsDestroyed() const {
	return !IsWindow(_handle);
}
BOOL EZ::ProcessOneMessage(BOOL wait) {
	MSG msg = { };
	if (wait)
	{
		if (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			return TRUE;
		}
	}
	else
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			return TRUE;
		}
	}
	return FALSE;
}



BOOL EZ::ProcessUntilClear() {
	BOOL output = FALSE;

	MSG msg = { };
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		output = TRUE;
	}

	return output;
}
BOOL EZ::RunMessagePump() {
	BOOL output = FALSE;

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		output = TRUE;
	}

	return output;
}






/*
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
void InitWndClass() {
	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW; // Redraw windows with this class on horizontal or vertical changes.
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0; // Allocate 0 extra bytes after the class declaration.
	wc.cbWndExtra = 0; // Allocate 0 extra bytes after windows of this class.
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hIcon = nullptr; // Default icon
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // Default cursor from system cursors.
	wc.hbrBackground = CreateSolidBrush(RGB(0, 0, 0)); // User implamented painting.
	wc.lpszMenuName = nullptr; // No default menu
	wc.lpszClassName = L"MysteryWindowClass";
	::RegisterClass(&wc);
}



struct MonitorList {
	DWORD length;
	HMONITOR* list;
};
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	vector<HMONITOR>* monitors = reinterpret_cast<vector<HMONITOR>*>(dwData);

	monitors->push_back(hMonitor);

	return TRUE;
}
MonitorList GetMonitors() {
	vector<HMONITOR>* monitors = new vector<HMONITOR>();
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(monitors));

	DWORD length = monitors->size();
	HMONITOR* monitorsArray = new HMONITOR[length];
	for (DWORD i = 0; i < length; ++i) {
		monitorsArray[i] = (*monitors)[i];
	}

	delete monitors;

	return { length, monitorsArray };
}



HWND CreateWnd(HMONITOR monitor) {
	MONITORINFO monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &monitorInfo);

	int x = monitorInfo.rcMonitor.left;
	int y = monitorInfo.rcMonitor.top;
	int width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
	int height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

	HWND handle = CreateWindowEx(
		WS_EX_TOPMOST, // Optional window styles
		L"MysteryWindowClass", // Window class name
		L"Mystery Window", // Window title
		WS_VISIBLE | WS_POPUP, // Window styles
		x, // X position
		y, // Y position
		width, // Width
		height, // Height
		nullptr, // Parent window
		nullptr, // Target menu
		GetModuleHandle(nullptr), // Instance handle
		nullptr // Additional data
	);

	return handle;
}



void PumpMessages() {
	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}



void RunWindows() {
	InitWndClass();

	MonitorList monitors = GetMonitors();
	HWND* windows = new HWND[monitors.length];

	for (DWORD i = 0; i < monitors.length; i++)
	{
		windows[i] = CreateWnd(monitors.list[i]);
	}

	PumpMessages();

	delete[] monitors.list;
}
*/