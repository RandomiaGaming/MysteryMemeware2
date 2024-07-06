#include <windows.h>
#include <tlhelp32.h>

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
	DWORD tokenInformation = 0;
	DWORD returnLength = 0;
	GetTokenInformation(token, TokenUIAccess, &tokenInformation, sizeof(DWORD), &returnLength);

	// Close the handle to the current process token we opened earlier.
	CloseHandle(token);

	// tokenInformation is set by GetTokenInformation to 1 if UIAccess is granted otherwise it is 0.
	return tokenInformation != 0;
}

BOOL IsAdmin()
{
	// Try to get the admin group SID. If that fails return false.
	PSID adminGroup = NULL;
	SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
	if (!AllocateAndInitializeSid(&ntAuthority, 2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&adminGroup))
	{
		return FALSE;
	}

	// Try to figure out if we are in the admins groups. If that fails return false.
	BOOL output = FALSE;
	if (!CheckTokenMembership(NULL, adminGroup, &output))
	{
		FreeSid(adminGroup);
		return FALSE;
	}

	// Return the result of the check above.
	return output;
}

void RelaunchAsAdmin()
{
	// Get the path to the current exe
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath));

	// Create a shell execute info to launch the current exe with a UAC prompt.
	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.lpVerb = L"runas";
	sei.lpFile = szPath;
	sei.nShow = SW_NORMAL;

	// Launch the shell execute info with the shell.
	ShellExecuteEx(&sei);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Because this is just a testing window we use the default WndProc.
	// You could change this if you wanted a more intresting window.
	// The background is drawn for us by the desktop window manager because we specified a background brush.
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int main() {
	if (!IsAdmin()) {
		RelaunchAsAdmin();
		return 0;
	}

	// If the current process does not have UIAccess the restart it with UIAccess.
	if (!HasUIAccess()) {
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
		return 0;
	}

	// Register a testing window class for a basic blank window.
	WNDCLASS windowClass;
	windowClass.style = 0;
	windowClass.lpfnWndProc = WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(nullptr);
	windowClass.hIcon = nullptr;
	windowClass.hCursor = nullptr;
	windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BACKGROUND);
	windowClass.lpszMenuName = nullptr;
	windowClass.lpszClassName = L"DefaultWindowClass";
	RegisterClass(&windowClass);

	// Create a testing window.
	HWND windowHandle = CreateWindow(
		L"DefaultWindowClass",
		L"Testing Window",
		WS_OVERLAPPEDWINDOW,
		480, 270, 960, 540,
		nullptr, nullptr,
		GetModuleHandle(nullptr), nullptr
	);

	// Make the window we just created topmost.
	// Without UIAccess a topmost window is on top of other windows.
	// With UIAccess a topmost window is on top of everything even system GUIs.
	// The only thing above a topmost window with UIAccess is the secure desktop and the mouse cursor.
	SetWindowPos(windowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// Show the test window and run its message pump.
	ShowWindow(windowHandle, SW_NORMAL);
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Return and exit.
	return 0;
}