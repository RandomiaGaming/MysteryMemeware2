#include "Helper.h"
#include <Windows.h>
#include <iostream>
#include <conio.h>

void PressAnyKey() {
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

	DWORD originalConsoleMode;
	GetConsoleMode(hStdin, &originalConsoleMode);

	SetConsoleMode(hStdin, originalConsoleMode & ~ENABLE_ECHO_INPUT);

	(void)_getch();

	SetConsoleMode(hStdin, originalConsoleMode);
}
void PrintLastError() {
	DWORD errorCode = GetLastError();

	WCHAR* errorMessage = nullptr;
	DWORD size = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorMessage, 0, NULL);

	std::wcerr << L"Error: " << errorMessage << std::endl;

	LocalFree(errorMessage);
}