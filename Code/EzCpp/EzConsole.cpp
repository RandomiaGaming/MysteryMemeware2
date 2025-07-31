// Approved 11/14/2024

#include "EzConsole.h"
#include "EzError.h"

EzConsole::Color EzConsole::GetColor() {
	HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (stdoutHandle == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo = { };
	if (!GetConsoleScreenBufferInfo(stdoutHandle, &consoleScreenBufferInfo)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return static_cast<EzConsole::Color>(consoleScreenBufferInfo.wAttributes &= 0x000F);
}
void EzConsole::SetColor(EzConsole::Color color) {
	HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (stdoutHandle == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo = { };
	if (!GetConsoleScreenBufferInfo(stdoutHandle, &consoleScreenBufferInfo)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	consoleScreenBufferInfo.wAttributes &= 0xFFF0;
	consoleScreenBufferInfo.wAttributes |= static_cast<WORD>(color);

	if (!SetConsoleTextAttribute(stdoutHandle, consoleScreenBufferInfo.wAttributes)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
EzConsole::Color EzConsole::GetBackgroundColor() {
	HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (stdoutHandle == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo = { };
	if (!GetConsoleScreenBufferInfo(stdoutHandle, &consoleScreenBufferInfo)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	consoleScreenBufferInfo.wAttributes &= 0x00F0;
	return static_cast<EzConsole::Color>(consoleScreenBufferInfo.wAttributes >> 4);
}
void EzConsole::SetBackgroundColor(EzConsole::Color color) {
	HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (stdoutHandle == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo = { };
	if (!GetConsoleScreenBufferInfo(stdoutHandle, &consoleScreenBufferInfo)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	consoleScreenBufferInfo.wAttributes &= 0xFF0F;
	consoleScreenBufferInfo.wAttributes |= static_cast<WORD>(color) << 4;

	if (!SetConsoleTextAttribute(stdoutHandle, consoleScreenBufferInfo.wAttributes)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

void EzConsole::WriteA(LPCSTR message) {
	HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (stdoutHandle == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	int messageLength = lstrlenA(message);
	DWORD charsWritten = 0;
	if (!WriteConsoleA(stdoutHandle, message, messageLength, &charsWritten, NULL) || charsWritten != messageLength) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
void EzConsole::WriteW(LPCWSTR message) {
	HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (stdoutHandle == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	int messageLength = lstrlenW(message);
	DWORD charsWritten = 0;
	if (!WriteConsoleW(stdoutHandle, message, messageLength, &charsWritten, NULL) || charsWritten != messageLength) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
void EzConsole::WriteLineA(LPCSTR message) {
	EzConsole::WriteA(message);
	EzConsole::WriteA("\r\n");
}
void EzConsole::WriteLineW(LPCWSTR message) {
	EzConsole::WriteW(message);
	EzConsole::WriteW(L"\r\n");
}

CHAR EzConsole::ReadCharA() {
	HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
	if (stdinHandle == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	DWORD originalMode = 0;
	if (!GetConsoleMode(stdinHandle, &originalMode)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	DWORD mode = originalMode;
	mode &= ~ENABLE_LINE_INPUT;
	mode &= ~ENABLE_ECHO_INPUT;
	if (!SetConsoleMode(stdinHandle, mode)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	while (true) {
		INPUT_RECORD inputRecord = { };
		DWORD numberOfEvents = 0;
		if (!ReadConsoleInputA(stdinHandle, &inputRecord, 1, &numberOfEvents)) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}

		if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
			if (!SetConsoleMode(stdinHandle, originalMode)) {
				EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
			}

			return inputRecord.Event.KeyEvent.uChar.AsciiChar;
		}
	}
}
WCHAR EzConsole::ReadCharW() {
	HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
	if (stdinHandle == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	DWORD originalMode = 0;
	if (!GetConsoleMode(stdinHandle, &originalMode)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	DWORD mode = originalMode;
	mode &= ~ENABLE_LINE_INPUT;
	mode &= ~ENABLE_ECHO_INPUT;
	if (!SetConsoleMode(stdinHandle, mode)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	while (true) {
		INPUT_RECORD inputRecord = { };
		DWORD numberOfEvents = 0;
		if (!ReadConsoleInputW(stdinHandle, &inputRecord, 1, &numberOfEvents)) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}

		if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
			if (!SetConsoleMode(stdinHandle, originalMode)) {
				EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
			}

			return inputRecord.Event.KeyEvent.uChar.UnicodeChar;
		}
	}
}
LPSTR EzConsole::ReadLineA() {
	HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
	if (stdinHandle == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	UINT32 bufferCapacity = 1024;
	UINT32 bufferLength = 0;
	LPSTR buffer = new CHAR[bufferCapacity];

	while (true) {
		if (bufferLength == bufferCapacity) {
			bufferCapacity *= 2;
			LPSTR expandedBuffer = new CHAR[bufferCapacity];
			memcpy(expandedBuffer, buffer, bufferLength * sizeof(CHAR));
			delete[] buffer;
			buffer = expandedBuffer;
		}

		DWORD charsRead = 0;
		if (!ReadConsoleA(stdinHandle, buffer + bufferLength, 1, &charsRead, NULL) || charsRead == 0) {
			continue;
		}

		if (buffer[bufferLength] == '\n') {
			buffer[bufferLength] = '\0';
			bufferLength++;

			LPSTR perfectSizeBuffer = new CHAR[bufferLength];
			memcpy(perfectSizeBuffer, buffer, bufferLength * sizeof(CHAR));
			delete[] buffer;
			return perfectSizeBuffer;
		}
		else if (buffer[bufferLength] != '\r') {
			bufferLength++;
		}
	}
}
LPWSTR EzConsole::ReadLineW() {
	HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
	if (stdinHandle == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	UINT32 bufferCapacity = 1024;
	UINT32 bufferLength = 0;
	LPWSTR buffer = new WCHAR[bufferCapacity];

	while (true) {
		if (bufferLength == bufferCapacity) {
			bufferCapacity *= 2;
			LPWSTR expandedBuffer = new WCHAR[bufferCapacity];
			memcpy(expandedBuffer, buffer, bufferLength * sizeof(WCHAR));
			delete[] buffer;
			buffer = expandedBuffer;
		}

		DWORD charsRead = 0;
		if (!ReadConsoleW(stdinHandle, buffer + bufferLength, 1, &charsRead, NULL) || charsRead == 0) {
			continue;
		}

		if (buffer[bufferLength] == L'\n') {
			buffer[bufferLength] = L'\0';
			bufferLength++;

			LPWSTR perfectSizeBuffer = new WCHAR[bufferLength];
			memcpy(perfectSizeBuffer, buffer, bufferLength * sizeof(WCHAR));
			delete[] buffer;
			return perfectSizeBuffer;
		}
		else if (buffer[bufferLength] != L'\r') {
			bufferLength++;
		}
	}
}