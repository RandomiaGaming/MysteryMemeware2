// Approved 10/26/2024

#pragma once
#include <Windows.h>
#include <iostream>

void EzPrintHexA(void* value, UINT32 length, std::ostream& outputStream);
void EzPrintHexW(void* value, UINT32 length, std::wostream& outputStream);
#ifdef UNICODE
#define EzPrintHex EzPrintHexW
#else
#define EzPrintHex EzPrintHexA
#endif // UNICODE

void EzPrintBinaryA(void* value, UINT32 length, std::ostream& outputStream);
void EzPrintBinaryW(void* value, UINT32 length, std::wostream& outputStream);
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

LPSTR EzGetCurrentExePathA();
LPWSTR EzGetCurrentExePathW();
#ifdef UNICODE
#define EzGetCurrentExePath EzGetCurrentExePathW
#else
#define EzGetCurrentExePath EzGetCurrentExePathA
#endif // UNICODE

void EzCloseHandleSafely(HANDLE handle);
void EzCloseProcessInfoSafely(PROCESS_INFORMATION processInfo);

FARPROC EzGetFunctionAddressA(LPCSTR functionName, LPCSTR libraryName);
FARPROC EzGetFunctionAddressW(LPCWSTR functionName, LPCWSTR libraryName);
#ifdef UNICODE
#define EzGetFunctionAddress EzGetFunctionAddressW
#else
#define EzGetFunctionAddress EzGetFunctionAddressA
#endif // UNICODE

void EzSetProcessCritical(BOOL isCritical);

HWINSTA EzGetCurrentStation();
HWINSTA EzGetPrimaryStation();
void EzSetProcessStation(HWINSTA station);

HDESK EzGetCurrentDesktop();
HDESK EzGetPrimaryDesktop();
HDESK EzGetSecureDesktop();
void EzSetThreadDesktop(HDESK desktop);
void EzSwitchToDesktop(HDESK desktop);