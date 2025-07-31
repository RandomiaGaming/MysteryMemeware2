// Approved 11/14/2024

#include "EzHelper.h"
#include "EzTokens.h"
#include "EzError.h"
#include "EzLL.h"
#include <iomanip>

void EzPrintHexA(void* value, UINT32 length, std::ostream& outputStream) {
	BYTE* valueBytes = reinterpret_cast<BYTE*>(value);
	outputStream << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase;
	for (UINT32 i = length - 1; i != 0xFFFFFFFF; i--)
	{
		outputStream << valueBytes[i];
	}
	outputStream << std::nouppercase << std::setfill(' ') << std::setw(0) << std::dec;
}
void EzPrintHexW(void* value, UINT32 length, std::wostream& outputStream) {
	BYTE* valueBytes = reinterpret_cast<BYTE*>(value);
	outputStream << L"0x" << std::hex << std::setw(2) << std::setfill(L'0') << std::uppercase;
	for (UINT32 i = length - 1; i != 0xFFFFFFFF; i--)
	{
		outputStream << valueBytes[i];
	}
	outputStream << std::nouppercase << std::setfill(L' ') << std::setw(0) << std::dec;
}

void EzPrintBinaryA(void* value, UINT32 length, std::ostream& outputStream) {
	BYTE* valueBytes = reinterpret_cast<BYTE*>(value);
	for (UINT32 i = 0; i < length; i++)
	{
		if (i != 0) { outputStream << ' '; }
		if ((valueBytes[i] & 128) == 0) { outputStream << '0'; }
		else { outputStream << '1'; }
		if ((valueBytes[i] & 64) == 0) { outputStream << '0'; }
		else { outputStream << '1'; }
		if ((valueBytes[i] & 32) == 0) { outputStream << '0'; }
		else { outputStream << '1'; }
		if ((valueBytes[i] & 16) == 0) { outputStream << '0'; }
		else { outputStream << '1'; }
		if ((valueBytes[i] & 8) == 0) { outputStream << '0'; }
		else { outputStream << '1'; }
		if ((valueBytes[i] & 4) == 0) { outputStream << '0'; }
		else { outputStream << '1'; }
		if ((valueBytes[i] & 2) == 0) { outputStream << '0'; }
		else { outputStream << '1'; }
		if ((valueBytes[i] & 1) == 0) { outputStream << '0'; }
		else { outputStream << '1'; }
	}
}
void EzPrintBinaryW(void* value, UINT32 length, std::wostream& outputStream) {
	BYTE* valueBytes = reinterpret_cast<BYTE*>(value);
	for (UINT32 i = 0; i < length; i++)
	{
		if (i != 0) { outputStream << L' '; }
		if ((valueBytes[i] & 128) == 0) { outputStream << L'0'; }
		else { outputStream << L'1'; }
		if ((valueBytes[i] & 64) == 0) { outputStream << L'0'; }
		else { outputStream << L'1'; }
		if ((valueBytes[i] & 32) == 0) { outputStream << L'0'; }
		else { outputStream << L'1'; }
		if ((valueBytes[i] & 16) == 0) { outputStream << L'0'; }
		else { outputStream << L'1'; }
		if ((valueBytes[i] & 8) == 0) { outputStream << L'0'; }
		else { outputStream << L'1'; }
		if ((valueBytes[i] & 4) == 0) { outputStream << L'0'; }
		else { outputStream << L'1'; }
		if ((valueBytes[i] & 2) == 0) { outputStream << L'0'; }
		else { outputStream << L'1'; }
		if ((valueBytes[i] & 1) == 0) { outputStream << L'0'; }
		else { outputStream << L'1'; }
	}
}

void EzPrintBoolA(BOOL value, std::ostream& outputStream) {
	if (value) {
		outputStream << "True";
	}
	else {
		outputStream << "False";
	}
}
void EzPrintBoolW(BOOL value, std::wostream& outputStream) {
	if (value) {
		outputStream << L"True";
	}
	else {
		outputStream << L"False";
	}
}

void EzPrintSidA(PSID value, std::ostream& outputStream) {
	CHAR name[256];
	CHAR domain[256];
	DWORD nameLen = sizeof(name) / sizeof(WCHAR);
	DWORD domainLen = sizeof(domain) / sizeof(WCHAR);
	SID_NAME_USE sidType;
	BOOL valid = LookupAccountSidA(NULL, value, name, &nameLen, domain, &domainLen, &sidType);

	if (valid) {
		if (domainLen != 0)
		{
			outputStream << domain << "/";
		}
		outputStream << name;
	}
	else {
		sidType = SidTypeInvalid;
	}

	switch (sidType) {
	case SidTypeUser:
		outputStream << " (User)";
		break;
	case SidTypeGroup:
		outputStream << " (Group)";
		break;
	case SidTypeDomain:
		outputStream << " (Domain)";
		break;
	case SidTypeAlias:
		outputStream << " (Alias)";
		break;
	case SidTypeWellKnownGroup:
		outputStream << " (Well Known Group)";
		break;
	case SidTypeDeletedAccount:
		outputStream << " (Deleted Account)";
		break;
	case SidTypeComputer:
		outputStream << " (Computer)";
		break;
	case SidTypeLabel:
		outputStream << " (Label)";
		break;
	case SidTypeLogonSession:
		outputStream << " (Logon Session)";
		break;
	case SidTypeInvalid:
	case SidTypeUnknown:
	default:
		if (value == 0) { outputStream << "Null SID"; }
		else { outputStream << "Invalid/Unknown SID"; }
		break;
	}
}
void EzPrintSidW(PSID value, std::wostream& outputStream) {
	WCHAR name[256];
	WCHAR domain[256];
	DWORD nameLen = sizeof(name) / sizeof(WCHAR);
	DWORD domainLen = sizeof(domain) / sizeof(WCHAR);
	SID_NAME_USE sidType;
	BOOL valid = LookupAccountSidW(NULL, value, name, &nameLen, domain, &domainLen, &sidType);

	if (valid) {
		if (domainLen != 0)
		{
			outputStream << domain << L"/";
		}
		outputStream << name;
	}
	else {
		sidType = SidTypeInvalid;
	}

	switch (sidType) {
	case SidTypeUser:
		outputStream << L" (User)";
		break;
	case SidTypeGroup:
		outputStream << L" (Group)";
		break;
	case SidTypeDomain:
		outputStream << L" (Domain)";
		break;
	case SidTypeAlias:
		outputStream << L" (Alias)";
		break;
	case SidTypeWellKnownGroup:
		outputStream << L" (Well Known Group)";
		break;
	case SidTypeDeletedAccount:
		outputStream << L" (Deleted Account)";
		break;
	case SidTypeComputer:
		outputStream << L" (Computer)";
		break;
	case SidTypeLabel:
		outputStream << L" (Label)";
		break;
	case SidTypeLogonSession:
		outputStream << L" (Logon Session)";
		break;
	case SidTypeInvalid:
	case SidTypeUnknown:
	default:
		if (value == 0) { outputStream << L"Null SID"; }
		else { outputStream << L"Invalid/Unknown SID"; }
		break;
	}
}

void EzPrintLuidA(LUID value, std::ostream& outputStream) {
	CHAR privilegeName[256];
	DWORD nameLen = sizeof(privilegeName) / sizeof(privilegeName[0]);

	if (LookupPrivilegeNameA(NULL, &value, privilegeName, &nameLen)) {
		outputStream << privilegeName;
	}
	else {
		EzPrintHexA(&value, sizeof(LUID), outputStream);
	}
}
void EzPrintLuidW(LUID value, std::wostream& outputStream) {
	WCHAR privilegeName[256];
	DWORD nameLen = sizeof(privilegeName) / sizeof(privilegeName[0]);

	if (LookupPrivilegeNameW(NULL, &value, privilegeName, &nameLen)) {
		outputStream << privilegeName;
	}
	else {
		EzPrintHexW(&value, sizeof(LUID), outputStream);
	}
}

LPSTR EzGetCurrentExePathA() {
	UINT32 maxPathLength = MAX_PATH;
	UINT32 pathLength = 0;
	LPSTR path = new CHAR[maxPathLength];
	while (true) {
		pathLength = GetModuleFileNameA(NULL, path, maxPathLength);
		if (pathLength == 0) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
		else if (pathLength == maxPathLength) {
			delete[] path;
			maxPathLength += MAX_PATH;
			path = new CHAR[maxPathLength];
		}
		else {
			LPSTR pathTrimmed = new CHAR[pathLength + 1];
			lstrcpyA(pathTrimmed, path);
			delete[] path;
			return pathTrimmed;
		}
	}
}
LPWSTR EzGetCurrentExePathW() {
	UINT32 maxPathLength = MAX_PATH;
	UINT32 pathLength = 0;
	LPWSTR path = new WCHAR[maxPathLength];
	while (true) {
		pathLength = GetModuleFileNameW(NULL, path, maxPathLength);
		if (pathLength == 0) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
		else if (pathLength == maxPathLength) {
			delete[] path;
			maxPathLength += MAX_PATH;
			path = new WCHAR[maxPathLength];
		}
		else {
			LPWSTR pathTrimmed = new WCHAR[pathLength + 1];
			lstrcpyW(pathTrimmed, path);
			delete[] path;
			return pathTrimmed;
		}
	}
}

BOOL EzMatchesCaselessA(LPCSTR strA, LPCSTR strB) {
	UINT32 strALen = lstrlenA(strA);
	UINT32 strBLen = lstrlenA(strB);
	if (strALen != strBLen) {
		return FALSE;
	}
	for (UINT32 i = 0; i < strALen; i++) {
		if (tolower(strA[i]) != tolower(strB[i])) {
			return FALSE;
		}
	}
	return TRUE;
}
BOOL EzMatchesCaselessW(LPCWSTR strA, LPCWSTR strB) {
	UINT32 strALen = lstrlenW(strA);
	UINT32 strBLen = lstrlenW(strB);
	if (strALen != strBLen) {
		return FALSE;
	}
	for (UINT32 i = 0; i < strALen; i++) {
		if (tolower(strA[i]) != tolower(strB[i])) {
			return FALSE;
		}
	}
	return TRUE;
}

void EzCloseHandleSafely(HANDLE handle) {
	if (!CloseHandle(handle)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
void EzCloseProcessInfoSafely(PROCESS_INFORMATION processInfo) {
	EzCloseHandleSafely(processInfo.hThread);
	EzCloseHandleSafely(processInfo.hProcess);
}

FARPROC EzGetFunctionAddressA(LPCSTR functionName, LPCSTR libraryName) {
	HMODULE library = GetModuleHandleA(libraryName);
	if (library == NULL) {
		library = LoadLibraryA(libraryName);
		if (library == NULL) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
	}

	FARPROC functionAddress = GetProcAddress(library, functionName);
	if (functionAddress == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return functionAddress;
}
FARPROC EzGetFunctionAddressW(LPCWSTR functionName, LPCWSTR libraryName) {
	HMODULE library = GetModuleHandleW(libraryName);
	if (library == NULL) {
		library = LoadLibraryW(libraryName);
		if (library == NULL) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
	}

	CHAR defaultChar = '?';
	UINT32 narrowFunctionNameLength = WideCharToMultiByte(CP_UTF8, 0, functionName, -1, NULL, 0, &defaultChar, NULL);
	LPSTR narrowFunctionName = new CHAR[narrowFunctionNameLength];
	WideCharToMultiByte(CP_UTF8, 0, functionName, -1, narrowFunctionName, narrowFunctionNameLength, &defaultChar, NULL);
	narrowFunctionName[narrowFunctionNameLength - 1] = L'\0';

	FARPROC functionAddress = GetProcAddress(library, narrowFunctionName);
	DWORD lastError = GetLastError();

	delete[] narrowFunctionName;

	if (functionAddress == NULL) {
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	return functionAddress;
}

void EzSetProcessCritical(BOOL isCritical) {
	// NOTE RtlSetProcessIsCritical requires the SE_DEBUG_NAME privilege to be enabled for the caller process token.
	typedef NTSTATUS(WINAPI* PRtlSetProcessIsCritical) (
		IN  BOOLEAN  bNew,      // new setting for process
		OUT BOOLEAN* pbOld,     // pointer which receives old setting (can be null)
		IN  BOOLEAN  bNeedScb); // need system critical breaks

	NTSTATUS nt = 0;

	PRtlSetProcessIsCritical RtlSetCriticalProcess = reinterpret_cast<PRtlSetProcessIsCritical>(EzGetFunctionAddressW(L"RtlSetProcessIsCritical", L"ntdll.dll"));
	nt = RtlSetCriticalProcess(static_cast<BOOLEAN>(isCritical), NULL, FALSE);
	if (FAILED(nt)) {
		EzError::ThrowFromNT(nt, __FILE__, __LINE__);
	}
}

HWINSTA EzGetCurrentStation() {
	HWINSTA station = GetProcessWindowStation();
	if (station == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return station;
}
HWINSTA EzGetPrimaryStation() {
	/* NOTE: According to Microsoft documentation
	The interactive window station is the only window station that can display a user interface or receive user input.
	It is assigned to the logon session of the interactive user, and contains the keyboard, mouse, and display device.
	It is always named "WinSta0".
	*/
	HWINSTA station = OpenWindowStationW(L"WinSta0", FALSE, GENERIC_ALL);
	if (station == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return station;
}
void EzSetProcessStation(HWINSTA station) {
	if (!SetProcessWindowStation(station)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

HDESK EzGetCurrentDesktop() {
	HDESK desktop = GetThreadDesktop(GetCurrentThreadId());
	if (desktop == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	return desktop;
}
HDESK EzGetPrimaryDesktop() {
	/* NOTE
	OpenInputDesktop normally returns an HDESK to the interactive desktop
	unless the interactive desktop is the secure desktop in which case
	ERROR_INVALID_FUNCTION is returned instead.
	*/
	HDESK desktop = OpenInputDesktop(0, FALSE, GENERIC_ALL);
	if (desktop == NULL) {
		DWORD lastError = GetLastError();
		if (lastError == ERROR_INVALID_FUNCTION) {
			desktop = OpenDesktopW(L"Winlogon", 0, FALSE, GENERIC_ALL);
			if (desktop == NULL) {
				EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
			}
		}
		else {
			EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
		}
	}

	return desktop;
}
HDESK EzGetSecureDesktop() {
	HDESK desktop = OpenDesktopW(L"Winlogon", 0, FALSE, GENERIC_ALL);
	if (desktop == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	return desktop;
}
void EzSetThreadDesktop(HDESK desktop) {
	if (!SetThreadDesktop(desktop)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
void EzSwitchToDesktop(HDESK desktop) {
	if (!SwitchDesktop(desktop)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

HCURSOR EzGetCurrentCursor() {
	// This function will never fail
	// A return of NULL simply means the mouse is invisible right now
	return GetCursor();
}
HCURSOR EzGetPrimaryCursor() {
	HCURSOR cursor = LoadCursorW(NULL, IDC_ARROW);
	if (cursor == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	return cursor;
}
void EzSetCursor(HCURSOR cursor) {
	// This function will never fail
	// The return value simply represents the previous cursor value
	SetCursor(cursor);
}
void EzHideCursor() {
	int cursorCount = 1;
	while (cursorCount > 0) {
		cursorCount = ShowCursor(FALSE);
	}
}
void EzShowCursor() {
	int cursorCount = 0;
	while (cursorCount <= 0) {
		cursorCount = ShowCursor(TRUE);
	}
}

void EzBSOD(NTSTATUS errorCode) {
	typedef struct _UNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
		PWSTR Buffer;
	} UNICODE_STRING, * PUNICODE_STRING;
	typedef enum _HARDERROR_RESPONSE_OPTION {
		OptionAbortRetryIgnore = 0,
		OptionOk = 1,
		OptionOkCancel = 2,
		OptionRetryCancel = 3,
		OptionYesNo = 4,
		OptionYesNoCancel = 5,
		OptionShutdownSystem = 6
	} HARDERROR_RESPONSE_OPTION, * PHARDERROR_RESPONSE_OPTION;
	typedef enum _HARDERROR_RESPONSE {
		ResponseReturnToCaller = 0,
		ResponseNotHandled = 1,
		ResponseAbort = 2,
		ResponseCancel = 3,
		ResponseIgnore = 4,
		ResponseNo = 5,
		ResponseOk = 6,
		ResponseRetry = 7,
		ResponseYes = 8
	} HARDERROR_RESPONSE, * PHARDERROR_RESPONSE;
	typedef NTSTATUS(*PNtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, PUNICODE_STRING UnicodeStringParameterMask, PVOID* Parameters, HARDERROR_RESPONSE_OPTION ResponseOption, PHARDERROR_RESPONSE Response);

	NTSTATUS nt = 0;

	HANDLE currentToken = EzOpenCurrentToken();
	EzEnableAllPrivileges(currentToken);
	EzCloseHandleSafely(currentToken);

	PNtRaiseHardError NtRaiseHardError = reinterpret_cast<PNtRaiseHardError>(EzGetFunctionAddressW(L"NtRaiseHardError", L"ntdll.dll"));
	HARDERROR_RESPONSE response;
	nt = NtRaiseHardError(errorCode, 0, NULL, NULL, HARDERROR_RESPONSE_OPTION::OptionShutdownSystem, &response);
	if (FAILED(nt)) {
		EzError::ThrowFromNT(nt, __FILE__, __LINE__);
	}
}
void EzBSODACPD() {
	HANDLE currentToken = EzOpenCurrentToken();
	EzEnableAllPrivileges(currentToken);
	EzCloseHandleSafely(currentToken);

	EzSetProcessCritical(TRUE);
	ExitProcess(0);
}

PROCESS_INFORMATION EzLaunchProcess(LPCWSTR exePath) {
	STARTUPINFO si = {};
	si.cb = sizeof(STARTUPINFO);

	PROCESS_INFORMATION output = {};

	if (!CreateProcessW(exePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &output)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return output;
}
void EzShellExecuteProcess(LPCWSTR exePath, LPCWSTR arguments, BOOL hide) {
	INT nShowCmd = SW_NORMAL;
	if (hide) {
		nShowCmd = SW_HIDE;
	}
	if (ShellExecuteW(NULL, L"run", exePath, arguments, NULL, nShowCmd) == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	EzLL<HMONITOR>* monitors = reinterpret_cast<EzLL<HMONITOR>*>(dwData);
	monitors->InsertHead(hMonitor);
	return TRUE;
}
UINT32 GetMonitors(HMONITOR** output) {
	EzLL<HMONITOR> monitorsLL = { };
	if (!EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&monitorsLL))) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	UINT32 monitorCount = monitorsLL.Count();
	HMONITOR* monitors = monitorsLL.ToArray();

	if (output == NULL) {
		delete[] monitors;
	}
	else {
		*output = monitors;
	}
	return monitorCount;
}