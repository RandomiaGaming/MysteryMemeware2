#include "MysteryControl.h"
#include "EzError.h"
#include "EzTokens.h"
#include "EzWindow.h"
#include <tlhelp32.h>
#include <sddl.h>

struct WinLogonContext {
	DWORD winLogonPID;
	HANDLE winLogonHandle;
};
WinLogonContext winLogonContext = { };
void DebugBreakWinLogon() {
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
void DebugContinueWinLogon() {
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

HWINSTA currentStation = NULL;
HWINSTA SetStationInteractive() {
	/* NOTE: According to Microsoft documentation
	The interactive window station is the only window station that can display a user interface or receive user input.
	It is assigned to the logon session of the interactive user, and contains the keyboard, mouse, and display device.
	It is always named "WinSta0".
	*/
	HWINSTA winSta0 = OpenWindowStation(L"WinSta0", FALSE, GENERIC_ALL);
	if (winSta0 == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	if (!SetProcessWindowStation(winSta0)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return winSta0;
}
HDESK currentDesktop = NULL;
HDESK SetDesktopInteractive() {
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
HDESK SetSecureDesktop() {
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
void FreeInteractivity() {
	if (!CloseWindowStation(currentStation)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	if (!CloseDesktop(currentDesktop)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

HHOOK keyboardHook = NULL;
HHOOK mouseHook = NULL;
LRESULT CALLBACK BlockingHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return 1; // Returning 1 indicates the event has been handled and should not be sent to any other handlers.
}
void SetLLInputHooks()
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
void UnsetLLInputHooks() {
	if (!UnhookWindowsHookEx(keyboardHook)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	keyboardHook = NULL;

	if (!UnhookWindowsHookEx(mouseHook)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	mouseHook = NULL;
}

void InitMysteryControl() {
	HANDLE currentToken = EzOpenCurrentToken();
	EzEnableAllPrivileges(currentToken);
	BOOL isGodToken = EzIsGodToken(currentToken);
	EzCloseToken(currentToken);
	if (!isGodToken) {
		HANDLE godToken = EzCreateGodToken();
		EzRestartWithToken(godToken);
	}

	currentStation = SetStationInteractive();
	currentDesktop = SetDesktopInteractive();

	DebugBreakWinLogon();
	SetLLInputHooks();
}
void UpdateMysteryControl() {
	EzMessagePumpAll();
}
void FreeMysteryControl() {
	UnsetLLInputHooks();

	FreeInteractivity();

	DebugContinueWinLogon();
}