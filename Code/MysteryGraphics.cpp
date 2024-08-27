#include "MysteryGraphics.h"
#include "EzError.h"
#include "CoverImage.h"
#include "EzWindow.h"
#include "EzRenderer.h"

struct HMONITORNODE {
	HMONITOR value;
	HMONITORNODE* next;
};
struct HMONITORLL {
	UINT32 count;
	HMONITORNODE* head;
};
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	HMONITORLL* monitorsLL = reinterpret_cast<HMONITORLL*>(dwData);
	HMONITORNODE* newNode = new HMONITORNODE();
	newNode->value = hMonitor;
	newNode->next = monitorsLL->head;
	monitorsLL->head = newNode;
	monitorsLL->count++;
	return TRUE;
}
UINT32 GetMonitors(HMONITOR** pMonitors) {
	HMONITORLL monitorsLL = { };
	if (!EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&monitorsLL))) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	HMONITORNODE* currentNode = monitorsLL.head;
	HMONITOR* monitors = new HMONITOR[monitorsLL.count];
	for (UINT32 i = 0; i < monitorsLL.count; ++i) {
		monitors[i] = currentNode->value;
		currentNode = currentNode->next;
	}

	if (pMonitors == NULL) {
		delete[] monitors;
	}
	else {
		*pMonitors = monitors;
	}

	return monitorsLL.count;
}

struct WindowContext {
	HMONITOR monitor;
	EzWindowSettings windowSettings;
	HWND window;
	EzHwndRendererSettings rendererSettings;
	ID2D1Factory* factory;
	ID2D1HwndRenderTarget* windowRenderTarget;
	ID2D1Bitmap* mysteryImage;
};
UINT32 windowContextCount = 0;
WindowContext* windowContexts = NULL;
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	WindowContext* windowContext = reinterpret_cast<WindowContext*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (msg == WM_SIZE && windowContext != NULL) {
		UINT32 width = LOWORD(lParam);
		UINT32 height = HIWORD(lParam);
		windowContext->windowRenderTarget->Resize(D2D1::SizeU(width, height));
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void InitMysteryGraphics() {
	EzClassSettings classSettings = { };
	classSettings.WndProc = WindowProcedure;
	classSettings.Name = L"MysteryWindowClass";
	EzRegisterClass(&classSettings);

	HMONITOR* monitors = NULL;
	windowContextCount = GetMonitors(&monitors);
	windowContexts = new WindowContext[windowContextCount];
	memset(windowContexts, 0, sizeof(WindowContext) * windowContextCount);

	for (UINT32 i = 0; i < windowContextCount; i++)
	{
		windowContexts[i].monitor = monitors[i];
		MONITORINFOEX monitorInfo = { };
		monitorInfo.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(windowContexts[i].monitor, &monitorInfo);

		windowContexts[i].windowSettings = { };
		windowContexts[i].windowSettings.HideInTaskbar = TRUE;
		windowContexts[i].windowSettings.ClassName = L"MysteryWindowClass";
		windowContexts[i].windowSettings.Title = L"Mystery Experience Host Window";
		windowContexts[i].windowSettings.TopMost = TRUE;
		windowContexts[i].windowSettings.StylePreset = EzWindowStylePreset::Boarderless;
		windowContexts[i].windowSettings.InitialX = monitorInfo.rcMonitor.left;
		windowContexts[i].windowSettings.InitialY = monitorInfo.rcMonitor.top;
		windowContexts[i].windowSettings.InitialWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		windowContexts[i].windowSettings.InitialHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
		windowContexts[i].window = EzCreateWindow(&windowContexts[i].windowSettings);

		windowContexts[i].rendererSettings = { };
		EzCreateHwndRenderer(windowContexts[i].window, &windowContexts[i].rendererSettings, &windowContexts[i].factory, &windowContexts[i].windowRenderTarget);

		windowContexts[i].mysteryImage = EzLoadBitmap(windowContexts[i].windowRenderTarget, &CoverImage_Asset);

		SetLastError(0);
		SetWindowLongPtr(windowContexts[i].window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&windowContexts[i]));
		DWORD lastError = GetLastError();
		if (lastError != 0) {
			EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
		}
	}

	delete[] monitors;
}
void UpdateMysteryGraphics() {
	for (UINT32 i = 0; i < windowContextCount; i++)
	{
		windowContexts[i].windowRenderTarget->BeginDraw();
		windowContexts[i].windowRenderTarget->Clear();
		D2D1_SIZE_U size = windowContexts[i].windowRenderTarget->GetPixelSize();
		EzDrawBitmap(windowContexts[i].windowRenderTarget, windowContexts[i].mysteryImage, EzRectL(0, 0, size.width, size.height));
		windowContexts[i].windowRenderTarget->EndDraw();
	}
}
void FreeMysteryGraphics() {
	for (UINT32 i = 0; i < windowContextCount; i++)
	{
		windowContexts[i].mysteryImage->Release();
		windowContexts[i].windowRenderTarget->Release();
		windowContexts[i].factory->Release();
		if (!DestroyWindow(windowContexts[i].window)) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
	}
	windowContextCount = 0;
	delete[] windowContexts;

	if (!UnregisterClass(L"MysteryWindowClass", NULL)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}