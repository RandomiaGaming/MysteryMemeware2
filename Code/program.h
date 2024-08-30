#pragma once
#include <Windows.h>

extern BOOL QuitRequested;
int main(int argc, char** argv);
void IntentBSOD();
void IntentInstall();
void IntentUninstall();
void IntentRun();