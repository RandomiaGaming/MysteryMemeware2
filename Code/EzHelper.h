#include <Windows.h>
#include <iostream>

void EzPrintHexA(void* value, DWORD length, std::ostream& outputStream);
void EzPrintHexW(void* value, DWORD length, std::wostream& outputStream);
#ifdef UNICODE
#define EzPrintHex EzPrintHexW
#else
#define EzPrintHex EzPrintHexA
#endif // UNICODE

void EzPrintBinaryA(void* value, DWORD length, std::ostream& outputStream);
void EzPrintBinaryW(void* value, DWORD length, std::wostream& outputStream);
#ifdef UNICODE
#define EzPrintBinary EzPrintBinaryW
#else
#define EzPrintBinary EzPrintBinaryA
#endif // UNICODE

void EzPrintBoolA(BOOL value, std::ostream& outputStream);
void EzPrintBoolW(BOOL value, std::wostream& outputStream);
#ifdef UNICODE
#define EzPrintBool EzPrintBoolW
#else
#define EzPrintBool EzPrintBoolA
#endif // UNICODE

void EzPrintSidA(PSID value, std::ostream& outputStream);
void EzPrintSidW(PSID value, std::wostream& outputStream);
#ifdef UNICODE
#define EzPrintSid EzPrintSidW
#else
#define EzPrintSid EzPrintSidA
#endif // UNICODE

void EzPrintLuidA(LUID value, std::ostream& outputStream);
void EzPrintLuidW(LUID value, std::wostream& outputStream);
#ifdef UNICODE
#define EzPrintLuid EzPrintLuidW
#else
#define EzPrintLuid EzPrintLuidA
#endif // UNICODE

LPWSTR EzGetCurrentExePathA();
LPWSTR EzGetCurrentExePathW();
#ifdef UNICODE
#define EzGetCurrentExePath EzGetCurrentExePathW
#else
#define EzGetCurrentExePath EzGetCurrentExePathA
#endif // UNICODE

void EzCloseHandleSafely(HANDLE handle);
void EzCloseProcessInfoSafely(PROCESS_INFORMATION processInfo);

FARPROC EzGetFunctionAddressA(LPCSTR functionName, LPCSTR libraryName);
FARPROC EzGetFunctionAddressW(LPCSTR functionName, LPCWSTR libraryName);
#ifdef UNICODE
#define EzGetFunctionAddress EzGetFunctionAddressW
#else
#define EzGetFunctionAddress EzGetFunctionAddressA
#endif // UNICODE

void EzSetProcessCritical(BOOL isCritical);

HWINSTA EzGetActiveStation();
void EzSetProcessStation(HWINSTA station);
HDESK EzGetActiveDesktop();
HDESK EzGetSecureDesktop();
void EzSetThreadDesktop(HDESK desktop);
void EzSwitchToDesktop(HDESK desktop);