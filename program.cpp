#include "Renderring/EZWindow.h"

int main() {
	EZ::ClassSettings classSettings = { };
	EZ::RegisterClass(classSettings);
	EZ::WindowSettings windowSettings = { };
	EZ::Window* window = new EZ::Window(windowSettings);
	EZ::RunMessagePump();
	delete window;
	return 0;
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