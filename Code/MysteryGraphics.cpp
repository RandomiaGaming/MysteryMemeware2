// Approved 10/29/2024

#include "MysteryGraphics.h"
#include "MysteryImage.h"
#include "EzCpp/EzError.h"
#include "EzCpp/EzLL.h"
#include "EzCpp/EzWindow.h"
#include "EzCpp/EzRenderer.h"
#include "EzCpp//EzHelper.h"
#include <iostream>

namespace MysteryGraphics {
	static struct Context {
		HMONITOR monitor;
		HWND window;
		ID2D1Factory* factory;
		ID2D1HwndRenderTarget* windowRenderTarget;
		ID2D1Bitmap* mysteryImage;
	};
	static EzLL<Context*> contexts = { };

	static LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		Context* context = EzGetWindowDataAs<Context>(hwnd);
		if (msg == WM_SIZE && context != NULL) {
			UINT32 width = LOWORD(lParam);
			UINT32 height = HIWORD(lParam);
			context->windowRenderTarget->Resize(D2D1::SizeU(width, height));

			std::wcout << L"Somehow a window was resized." << std::endl;
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	static Context* GetContextForMonitor(HMONITOR hMonitor) {
		// Note returns a disabled context if there is no context associated with the hMonitor and there are disabled contexts availible.
		Context* disabledContext = NULL;
		UINT32 contextCount = contexts.Count();
		contexts.EnumHead();
		for (UINT32 i = 0; i < contextCount; i++)
		{
			Context* context = contexts.EnumGet();
			contexts.EnumNext();

			if (context->monitor == NULL) {
				disabledContext = context;
			}

			if (context->monitor == hMonitor) {
				return context;
			}
		}
		return disabledContext;
	}
	static void AddContext(HMONITOR monitor) {
		Context* context = new Context();

		context->monitor = monitor;
		MONITORINFOEX monitorInfo = { };
		monitorInfo.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(context->monitor, &monitorInfo);

		EzWindowSettings windowSettings = { };
		windowSettings.HideInTaskbar = TRUE;
		windowSettings.ClassName = L"MysteryWindowClass";
		windowSettings.Title = L"Mystery Experience Host Window";
		windowSettings.TopMost = TRUE;
		windowSettings.StylePreset = EzWindowStylePreset::Boarderless;
		windowSettings.InitialX = monitorInfo.rcMonitor.left;
		windowSettings.InitialY = monitorInfo.rcMonitor.top;
		windowSettings.InitialWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		windowSettings.InitialHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
		context->window = EzCreateWindow(&windowSettings);

		EzHwndRendererSettings rendererSettings = { };
		EzCreateHwndRenderer(context->window, &rendererSettings, &context->factory, &context->windowRenderTarget);

		context->mysteryImage = EzLoadBitmap(context->windowRenderTarget, &MysteryImage::Asset);

		EzSetWindowData(context->window, context);

		contexts.InsertHead(context);

		EzSetCursor(NULL);

		std::wcout << L"Adding window for monitor ("
			<< windowSettings.InitialX << ", "
			<< windowSettings.InitialY << ", "
			<< windowSettings.InitialWidth << ", "
			<< windowSettings.InitialHeight << ")." << std::endl;
	}
	static void DisableContext(Context* context) {
		ShowWindow(context->window, SW_HIDE);

		context->monitor = NULL;

		RECT rect = { };
		if (!GetWindowRect(context->window, &rect)) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
		std::wcout << L"Disabling window for monitor ("
			<< rect.left << ", "
			<< rect.top << ", "
			<< (rect.right - rect.left) << ", "
			<< (rect.bottom - rect.top) << ")." << std::endl;
	}
	static void ReinitContext(Context* context, HMONITOR monitor) {
		context->monitor = monitor;
		MONITORINFOEX monitorInfo = { };
		monitorInfo.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(context->monitor, &monitorInfo);

		int x = monitorInfo.rcMonitor.left;
		int y = monitorInfo.rcMonitor.top;
		int width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		int height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

		if (!SetWindowPos(context->window, HWND_TOPMOST, x, y, width, height, 0)) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}

		ShowWindow(context->window, SW_SHOWNORMAL);

		EzSetCursor(NULL);

		std::wcout << L"Reusing window for monitor ("
			<< x << ", "
			<< y << ", "
			<< width << ", "
			<< height << ")." << std::endl;
	}
	static void RemoveContext(Context* context) {
		context->mysteryImage->Release();
		context->mysteryImage = NULL;
		context->windowRenderTarget->Release();
		context->windowRenderTarget = NULL;
		context->factory->Release();
		context->factory = NULL;
		if (!DestroyWindow(context->window)) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
		context->window = NULL;
		context->monitor = NULL;
		contexts.Remove(context);
	}

	void Init() {
		EzClassSettings classSettings = { };
		classSettings.WndProc = WindowProcedure;
		classSettings.Name = L"MysteryWindowClass";
		EzRegisterClass(&classSettings);
	}
	void Update() {
		HMONITOR* monitors = NULL;
		UINT32 monitorCount = GetMonitors(&monitors);
		for (UINT32 i = 0; i < monitorCount; i++)
		{
			Context* context = GetContextForMonitor(monitors[i]);
			if (context == NULL) {
				AddContext(monitors[i]);
			}
			else if (context->monitor == NULL) {
				ReinitContext(context, monitors[i]);
			}
		}

		UINT32 contextCount = contexts.Count();
		contexts.EnumHead();
		for (UINT32 i = 0; i < contextCount; i++)
		{
			Context* context = contexts.EnumGet();
			contexts.EnumNext();

			if (context->monitor == NULL) {
				continue;
			}

			BOOL contextValid = FALSE;
			for (UINT32 i = 0; i < monitorCount; i++)
			{
				if (context->monitor == monitors[i]) {
					contextValid = TRUE;
					break;
				}
			}

			if (!contextValid) {
				DisableContext(context);
			}
		}
		delete[] monitors;

		EzMessagePumpAll();

		contextCount = contexts.Count();
		contexts.EnumHead();
		for (UINT32 i = 0; i < contextCount; i++)
		{
			Context* context = contexts.EnumGet();
			contexts.EnumNext();

			context->windowRenderTarget->BeginDraw();
			context->windowRenderTarget->Clear();
			D2D1_SIZE_U size = context->windowRenderTarget->GetPixelSize();
			EzDrawBitmap(context->windowRenderTarget, context->mysteryImage, EzRectL(0, 0, size.width, size.height));
			context->windowRenderTarget->EndDraw();
		}
	}
	void Free() {
		while (!contexts.IsEmpty())
		{
			Context* context = contexts.GetHead();
			RemoveContext(context);
		}
		EzMessagePumpAll();
		if (!UnregisterClassW(L"MysteryWindowClass", NULL)) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
	}
}