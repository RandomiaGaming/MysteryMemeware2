#include <Windows.h>
#include <iostream>

void EzPrintHexA(void* value, DWORD length, std::ostream& outputStream);
void EzPrintBinaryA(void* value, DWORD length, std::ostream& outputStream);
void EzPrintBoolA(BOOL value, std::ostream& outputStream);
void EzPrintSidA(PSID value, std::ostream& outputStream);
void EzPrintLuidA(LUID value, std::ostream& outputStream);
void EzPrintHexW(void* value, DWORD length, std::wostream& outputStream);
void EzPrintBinaryW(void* value, DWORD length, std::wostream& outputStream);
void EzPrintBoolW(BOOL value, std::wostream& outputStream);
void EzPrintSidW(PSID value, std::wostream& outputStream);
void EzPrintLuidW(LUID value, std::wostream& outputStream);

#ifdef UNICODE
#define EzPrintHex EzPrintHexW
#else
#define EzPrintHex EzPrintHexA
#endif

#ifdef UNICODE
#define EzPrintBinary EzPrintBinaryW
#else
#define EzPrintBinary EzPrintBinaryA
#endif

#ifdef UNICODE
#define EzPrintBool EzPrintBoolW
#else
#define EzPrintBool EzPrintBoolA
#endif

#ifdef UNICODE
#define EzPrintSid EzPrintSidW
#else
#define EzPrintSid EzPrintSidA
#endif

#ifdef UNICODE
#define EzPrintLuid EzPrintLuidW
#else
#define EzPrintLuid EzPrintLuidA
#endif

void EzCloseHandleSafely(HANDLE handle);
LPWSTR EzGetCurrentExePath();
void EzCloseProcessInfoSafely(PROCESS_INFORMATION processInfo);

FARPROC EzGetFunctionAddress(LPCSTR functionName, LPCWSTR libraryName);
void EzSetProcessCritical(BOOL isCritical);
HWINSTA EzGetActiveStation();
void EzSetProcessStation(HWINSTA station);
HDESK EzGetActiveDesktop();
HDESK EzGetSecureDesktop();
void EzSetThreadDesktop(HDESK desktop);
void EzSwitchToDesktop(HDESK desktop);