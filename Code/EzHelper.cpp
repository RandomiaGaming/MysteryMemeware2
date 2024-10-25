#include "EzError.h"
#include <iomanip>

void EzPrintHexA(void* value, DWORD length, std::ostream& outputStream) {
	BYTE* valueBytes = reinterpret_cast<BYTE*>(value);
	outputStream << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase;
	for (DWORD i = length - 1; i != 0xFFFFFFFF; i--)
	{
		outputStream << valueBytes[i];
	}
	outputStream << std::nouppercase << std::setfill(' ') << std::setw(0) << std::dec;
}
void EzPrintHexW(void* value, DWORD length, std::wostream& outputStream) {
	BYTE* valueBytes = reinterpret_cast<BYTE*>(value);
	outputStream << L"0x" << std::hex << std::setw(2) << std::setfill(L'0') << std::uppercase;
	for (DWORD i = length - 1; i != 0xFFFFFFFF; i--)
	{
		outputStream << valueBytes[i];
	}
	outputStream << std::nouppercase << std::setfill(L' ') << std::setw(0) << std::dec;
}

void EzPrintBinaryA(void* value, DWORD length, std::ostream& outputStream) {
	BYTE* valueBytes = reinterpret_cast<BYTE*>(value);
	for (DWORD i = 0; i < length; i++)
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
void EzPrintBinaryW(void* value, DWORD length, std::wostream& outputStream) {
	BYTE* valueBytes = reinterpret_cast<BYTE*>(value);
	for (DWORD i = 0; i < length; i++)
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
	DWORD maxPathLength = MAX_PATH;
	DWORD pathLength = 0;
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
	DWORD maxPathLength = MAX_PATH;
	DWORD pathLength = 0;
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
FARPROC EzGetFunctionAddressW(LPCSTR functionName, LPCWSTR libraryName) {
	HMODULE library = GetModuleHandleW(libraryName);
	if (library == NULL) {
		library = LoadLibraryW(libraryName);
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

void EzSetProcessCritical(BOOL isCritical) {
	// NOTE RtlSetProcessIsCritical requires the SE_DEBUG_NAME privilege to be enabled for the caller process token.
	typedef NTSTATUS(WINAPI* PRtlSetProcessIsCritical) (
		IN  BOOLEAN  bNew,      // new setting for process
		OUT BOOLEAN* pbOld,     // pointer which receives old setting (can be null)
		IN  BOOLEAN  bNeedScb); // need system critical breaks

	NTSTATUS nt = 0;

	PRtlSetProcessIsCritical RtlSetCriticalProcess = reinterpret_cast<PRtlSetProcessIsCritical>(EzGetFunctionAddress("RtlSetProcessIsCritical", L"ntdll.dll"));
	nt = RtlSetCriticalProcess(static_cast<BOOLEAN>(isCritical), NULL, FALSE);
	if (FAILED(nt)) {
		EzError::ThrowFromNT(nt, __FILE__, __LINE__);
	}
}

HWINSTA EzGetActiveStation() {
	/* NOTE: According to Microsoft documentation
	The interactive window station is the only window station that can display a user interface or receive user input.
	It is assigned to the logon session of the interactive user, and contains the keyboard, mouse, and display device.
	It is always named "WinSta0".
	*/
	HWINSTA station = OpenWindowStation(L"WinSta0", FALSE, GENERIC_ALL);
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
HDESK EzGetActiveDesktop() {
	/* NOTE
	OpenInputDesktop normally returns an HDESK to the interactive desktop
	unless the interactive desktop is the secure desktop in which case
	ERROR_INVALID_FUNCTION is returned instead.
	*/
	HDESK desktop = OpenInputDesktop(0, FALSE, GENERIC_ALL);
	if (desktop == NULL) {
		DWORD lastError = GetLastError();
		if (lastError == ERROR_INVALID_FUNCTION) {
			desktop = OpenDesktop(L"Winlogon", 0, FALSE, GENERIC_ALL);
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
	HDESK desktop = OpenDesktop(L"Winlogon", 0, FALSE, GENERIC_ALL);
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