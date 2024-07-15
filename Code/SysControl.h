#pragma once
#include <Windows.h>

DWORD GetWinlogonPID();
HANDLE CreateUIAccessToken();
BOOL HasUIAccess();
BOOL IsAdmin();
void RelaunchAsAdmin();
void RelaunchWithUIAccess();
void TakeSEDebugPrivilege();
void BreakWinlogon();
void BlockInput();
void MakeSystemCritical();