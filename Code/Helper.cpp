#include "Helper.h"
#include <Windows.h>
#include <comdef.h>

void EZ::Log::ErrorFromHR(HRESULT hr) {
	if (SUCCEEDED(hr)) {
		return;
	}

	WCHAR* errorMessage = nullptr;
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorMessage, 0, NULL) > 0) {
		EZ::Log::Error(errorMessage);
		LocalFree(errorMessage);
	}
	else {
		_com_error comError(hr);
		LPCWSTR comErrorMessage = comError.ErrorMessage();

		constexpr const WCHAR* unknownError = L"Unknown error";
		for (int i = 0; unknownError[i] != '\0'; i++)
		{
			if (comErrorMessage[i] != unknownError[i]) {
				goto comErrorKnown;
			}
		}
		{
			WORD code = HRESULT_CODE(hr);
			AppendLog(L"Unknown com error with code: " + code);
			throw Error(L"Unknown com error with code: " + code);
		}

	comErrorKnown:
		errorMessage = new WCHAR[lstrlen(comErrorMessage)];
		lstrcpy(errorMessage, comErrorMessage);
		AppendLog(errorMessage);
		throw Error(errorMessage, Error::ErrorDisposal::Delete);
	}
}
void EZ::Log::ErrorFromCode(DWORD errorCode) {
	if (errorCode == 0) {
		return;
	}

	WCHAR* errorMessage = nullptr;
	DWORD size = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errorMessage, 0, NULL);

	EZ::Log::Error(errorMessage);

	LocalFree(errorMessage);
}
void EZ::Log::Error(LPCWSTR errorMessage) {
	// Get std::wcout handle.
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	// Get initial console color.
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(consoleHandle, &consoleInfo);
	WORD originalColor = consoleInfo.wAttributes;

	// Set console color.
	SetConsoleTextAttribute(consoleHandle, static_cast<WORD>(EZ::ConsoleColor::Red));

	// Print error message.
	DWORD charsWritten;
	LPCWSTR errorHeader = L"ERROR: ";
	WriteConsole(consoleHandle, errorHeader, lstrlen(errorHeader), &charsWritten, NULL);
	WriteConsole(consoleHandle, errorMessage, lstrlen(errorMessage), &charsWritten, NULL);

	// Restore initial console attributes.
	SetConsoleTextAttribute(consoleHandle, originalColor);

	// Open log file handle
	HANDLE logFile = CreateFile(L"%ProgramData%\\MysteryLog.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	// Get the file size
	LONGLONG logFileSize;
	GetFileSizeEx(logFile, reinterpret_cast<LARGE_INTEGER*>(&logFileSize));

	// Read the existing contents of the file
	BYTE* fileContents = new BYTE[logFileSize];
	DWORD bytesRead;
	ReadFile(logFile, fileContents, static_cast<DWORD>(logFileSize), &bytesRead, NULL);

	// Move the file pointer to the beginning
	SetFilePointer(logFile, 0, NULL, FILE_BEGIN);

	// Write the new line to the file
	DWORD bytesWritten;
	WriteFile(logFile, errorHeader, static_cast<DWORD>(lstrlen(errorHeader) * sizeof(WCHAR)), &bytesWritten, NULL);
	WriteFile(logFile, errorMessage, static_cast<DWORD>(lstrlen(errorMessage) * sizeof(WCHAR)), &bytesWritten, NULL);

	// Write the old contents back to the file
	WriteFile(logFile, fileContents, static_cast<DWORD>(logFileSize), &bytesWritten, NULL);

	// Cleanup and return.
	delete[] fileContents;
	CloseHandle(logFile);
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