#include "Helper.h"
#include <Windows.h>

// These handles are global to the entire process and all threads.
// Do not call CloseHandle for these handles as they will be perminantly closed for the entire process.
const HANDLE InputHandle = GetStdHandle(STD_INPUT_HANDLE);
const HANDLE OutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
const HANDLE ErrorHandle = GetStdHandle(STD_ERROR_HANDLE);

Error::Error(LPCWSTR message) {
	_wideMessage = TRUE;
	_constMessage = TRUE;
	_disposal = ErrorDisposal::Delete;
	_message = const_cast<void*>(reinterpret_cast<const void*>(message));
}
Error::Error(LPCSTR message) {
	_wideMessage = FALSE;
	_constMessage = TRUE;
	_disposal = ErrorDisposal::Delete;
	_message = const_cast<void*>(reinterpret_cast<const void*>(message));
}
Error::Error(LPWSTR message, ErrorDisposal disposal) {
	_wideMessage = TRUE;
	_constMessage = FALSE;
	_disposal = disposal;
	_message = const_cast<void*>(reinterpret_cast<const void*>(message));
}
Error::Error(LPSTR message, ErrorDisposal disposal) {
	_wideMessage = FALSE;
	_constMessage = FALSE;
	_disposal = disposal;
	_message = const_cast<void*>(reinterpret_cast<const void*>(message));
}
void Error::Print() {
	if (_wideMessage) {
		LPCWSTR wideMessage = reinterpret_cast<LPCWSTR>(_message);
		PrintError(wideMessage);
	}
	else {
		LPCSTR narrowMessage = reinterpret_cast<LPCSTR>(_message);
		PrintError(narrowMessage);
	}
}
Error::~Error() {
	if (!_constMessage) {
		switch (_disposal) {
		case ErrorDisposal::Free:
			free(const_cast<void*>(_message));
			break;
		case ErrorDisposal::Delete:
			delete[] const_cast<void*>(_message);
			break;
		case ErrorDisposal::LocalFree:
			::LocalFree(const_cast<void*>(_message));
			break;
		default: break;
		}
	}
	_wideMessage = FALSE;
	_constMessage = FALSE;
	_disposal = ErrorDisposal::Delete;
	_message = nullptr;
}

void PrintError(LPCWSTR errorMessage) {
	// Get initial console attributes.
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(ErrorHandle, &consoleInfo);
	WORD savedAttributes = consoleInfo.wAttributes;

	// Set console attributes to red text.
	SetConsoleTextAttribute(ErrorHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);

	// Print error header.
	DWORD written;
	LPCWSTR errorHeader = L"ERROR: ";
	WriteConsole(ErrorHandle, errorHeader, lstrlen(errorHeader), &written, NULL);

	// Print error message.
	WriteConsole(ErrorHandle, errorMessage, lstrlen(errorMessage), &written, NULL);

	// Restore initial console attributes.
	SetConsoleTextAttribute(ErrorHandle, savedAttributes);
}
void PrintError(LPCSTR errorMessage) {
	// Get initial console attributes.
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(ErrorHandle, &consoleInfo);
	WORD savedAttributes = consoleInfo.wAttributes;

	// Set console attributes to red text.
	SetConsoleTextAttribute(ErrorHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);

	// Print error header.
	DWORD written;
	LPCWSTR errorHeader = L"ERROR: ";
	WriteConsole(ErrorHandle, errorHeader, lstrlen(errorHeader), &written, NULL);

	// Print error message.
	WriteConsole(ErrorHandle, errorMessage, strlen(errorMessage), &written, NULL);

	// Restore initial console attributes.
	SetConsoleTextAttribute(ErrorHandle, savedAttributes);
}
void PrintWarning(LPCWSTR warningMessage) {
	// Get initial console attributes.
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(ErrorHandle, &consoleInfo);
	WORD savedAttributes = consoleInfo.wAttributes;

	// Set console attributes to yellow text.
	SetConsoleTextAttribute(ErrorHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	// Print warning header.
	DWORD written;
	LPCWSTR errorHeader = L"Warning: ";
	WriteConsole(ErrorHandle, errorHeader, lstrlen(errorHeader), &written, NULL);

	// Print warning message.
	WriteConsole(ErrorHandle, warningMessage, lstrlen(warningMessage), &written, NULL);

	// Restore initial console attributes.
	SetConsoleTextAttribute(ErrorHandle, savedAttributes);
}
void PrintWarning(LPCSTR warningMessage) {
	// Get initial console attributes.
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(ErrorHandle, &consoleInfo);
	WORD savedAttributes = consoleInfo.wAttributes;

	// Set console attributes to yellow text.
	SetConsoleTextAttribute(ErrorHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	// Print warning header.
	DWORD written;
	LPCWSTR errorHeader = L"Warning: ";
	WriteConsole(ErrorHandle, errorHeader, lstrlen(errorHeader), &written, NULL);

	// Print warning message.
	WriteConsole(ErrorHandle, warningMessage, strlen(warningMessage), &written, NULL);

	// Restore initial console attributes.
	SetConsoleTextAttribute(ErrorHandle, savedAttributes);
}
void Print(LPCWSTR message) {
	// Print message.
	DWORD written;
	WriteConsole(ErrorHandle, message, lstrlen(message), &written, NULL);
}
void Print(LPCSTR message) {
	// Print message.
	DWORD written;
	WriteConsole(ErrorHandle, message, strlen(message), &written, NULL);
}
void PrintLine(LPCWSTR message) {
	Print(message);
	PrintLine();
}
void PrintLine(LPCSTR message) {
	Print(message);
	PrintLine();
}
void PrintLine() {
	// Print new line.
	DWORD written;
	WriteConsole(ErrorHandle, L"\r\n", 2, &written, NULL);
}

void PressAnyKey() {
	DWORD originalConsoleMode;
	GetConsoleMode(InputHandle, &originalConsoleMode);

	SetConsoleMode(InputHandle, originalConsoleMode & ~ENABLE_ECHO_INPUT);

	INPUT_RECORD irInBuf[1];
	DWORD cNumRead;
	char ch = '\0';
	while (true) {
		if (ReadConsoleInput(InputHandle, irInBuf, 1, &cNumRead)) {
			if (irInBuf[0].EventType == KEY_EVENT) {
				KEY_EVENT_RECORD ker = irInBuf[0].Event.KeyEvent;
				if (ker.bKeyDown && !(ker.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED | SHIFT_PRESSED | NUMLOCK_ON | SCROLLLOCK_ON | CAPSLOCK_ON))) {
					ch = ker.uChar.AsciiChar;
					break;
				}
			}
		}
	}

	SetConsoleMode(InputHandle, originalConsoleMode);
}
void ThrowSysError() {
	DWORD errorCode = GetLastError();

	if (errorCode == 0) {
		PrintWarning(L"PrintLastError called even though nothing went wrong.");
		return;
	}

	WCHAR* errorMessage = nullptr;
	DWORD size = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorMessage, 0, NULL);

	throw new Error(errorMessage, Error::ErrorDisposal::LocalFree);
}