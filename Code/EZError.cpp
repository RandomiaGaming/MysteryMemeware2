#include "EZError.h"
#include <Windows.h>
#include <comdef.h>
#include <sstream>

EZ::Error::Error(DWORD errorCode, LPCSTR file, int line) {
	try {
		std::wostringstream errorMessageStream;
		if (file == NULL || line < 0) {
			errorMessageStream << "ERROR: ";
		}
		else {
			errorMessageStream << "ERROR (" << file << ":" << line << "): ";
		}

		DWORD errorCode = GetLastError();

		LPWSTR systemErrorMessage = NULL;
		DWORD systemErrorLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&systemErrorMessage), 0, NULL);

		errorMessageStream << systemErrorMessage;
		LPCWSTR lastTwoChars = systemErrorMessage + (systemErrorLength - 2);
		if (lastTwoChars[0] != L'\r' || lastTwoChars[1] != L'\n') {
			errorMessageStream << L"\r\n";
		}
		LocalFree(systemErrorMessage);

		std::wstring errorMessageString = errorMessageStream.str();
		_message = new WCHAR[errorMessageString.size() + 1];
		lstrcpyW(_message, errorMessageString.c_str());
	}
	catch (...) {}
}
EZ::Error::Error(HRESULT hr, LPCSTR file, int line) {
	try {
		std::wostringstream errorMessageStream;
		if (file == NULL || line < 0) {
			errorMessageStream << "ERROR: ";
		}
		else {
			errorMessageStream << "ERROR (" << file << ":" << line << "): ";
		}

		DWORD errorCode = GetLastError();

		LPWSTR systemErrorMessage = NULL;
		DWORD systemErrorLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&systemErrorMessage), 0, NULL);

		if (systemErrorLength > 0) {
			errorMessageStream << systemErrorMessage;
			LPCWSTR lastTwoChars = systemErrorMessage + (systemErrorLength - 2);
			if (lastTwoChars[0] != L'\r' || lastTwoChars[1] != L'\n') {
				errorMessageStream << L"\r\n";
			}
			LocalFree(systemErrorMessage);
		}
		else {
			_com_error comError(hr);
			LPCWSTR comErrorMessage = comError.ErrorMessage();

			errorMessageStream << comErrorMessage;
			LPCWSTR lastTwoChars = comErrorMessage + (lstrlenW(comErrorMessage) - 2);
			if (lastTwoChars[0] != L'\r' || lastTwoChars[1] != L'\n') {
				errorMessageStream << L"\r\n";
			}
		}

		std::wstring errorMessageString = errorMessageStream.str();
		_message = new WCHAR[errorMessageString.size() + 1];
		lstrcpyW(_message, errorMessageString.c_str());
	}
	catch (...) {}
}
EZ::Error::Error(LPCWSTR errorMessage, LPCSTR file, int line) {
	try {
		std::wostringstream errorMessageStream;
		if (file == NULL || line < 0) {
			errorMessageStream << "ERROR: ";
		}
		else {
			errorMessageStream << "ERROR (" << file << ":" << line << "): ";
		}

		errorMessageStream << errorMessage;
		LPCWSTR lastTwoChars = errorMessage + (lstrlenW(errorMessage) - 2);
		if (lastTwoChars[0] != L'\r' || lastTwoChars[1] != L'\n') {
			errorMessageStream << L"\r\n";
		}

		std::wstring errorMessageString = errorMessageStream.str();
		_message = new WCHAR[errorMessageString.size() + 1];
		lstrcpyW(_message, errorMessageString.c_str());
	}
	catch (...) {}
}
EZ::Error::Error(LPCSTR errorMessage, LPCSTR file, int line) {
	try {
		std::wostringstream errorMessageStream;
		if (file == NULL || line < 0) {
			errorMessageStream << "ERROR: ";
		}
		else {
			errorMessageStream << "ERROR (" << file << ":" << line << "): ";
		}

		errorMessageStream << errorMessage;
		LPCSTR lastTwoChars = errorMessage + (lstrlenA(errorMessage) - 2);
		if (lastTwoChars[0] != '\r' || lastTwoChars[1] != '\n') {
			errorMessageStream << L"\r\n";
		}

		std::wstring errorMessageString = errorMessageStream.str();
		_message = new WCHAR[errorMessageString.size() + 1];
		lstrcpyW(_message, errorMessageString.c_str());
	}
	catch (...) {}
}

EZ::Error::Error(const EZ::Error& other) {
	if (other._message != NULL) {
		size_t messageLength = lstrlenW(other._message) + 1;
		_message = new WCHAR[messageLength];
		lstrcpyW(_message, other._message);
	}
	else {
		_message = NULL;
	}
}
EZ::Error& EZ::Error::operator=(const EZ::Error& other) {
	if (this != &other) {
		this->~Error();
		if (other._message != NULL) {
			size_t messageLength = lstrlenW(other._message) + 1;
			_message = new WCHAR[messageLength];
			lstrcpyW(_message, other._message);
		}
		else {
			_message = NULL;
		}
	}
	return *this;
}

void EZ::Error::Print() {
	try {
		HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		// Get initial console attributes.
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		GetConsoleScreenBufferInfo(stdoutHandle, &consoleInfo);
		WORD savedAttributes = consoleInfo.wAttributes;

		// Set console attributes to red text.
		SetConsoleTextAttribute(stdoutHandle, static_cast<WORD>(EZ::ConsoleColor::Red));

		// Print error message.
		DWORD charsWritten = 0;
		WriteConsole(stdoutHandle, _message, lstrlenW(_message), &charsWritten, NULL);

		// Restore initial console attributes.
		SetConsoleTextAttribute(stdoutHandle, savedAttributes);
	}
	catch (...) {}

	try {
		// Open log file handle
		HANDLE logFile = CreateFile(L"C:\\ProgramData\\MysteryLog.txt", GENERIC_READ | GENERIC_WRITE, 0 /* FILE_SHARE_NONE */, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		// Get the file size
		DWORD logFileSize = GetFileSize(logFile, NULL);

		BYTE* fileContents = NULL;
		if (logFileSize > 0) {
			// Read the existing contents of the file
			fileContents = new BYTE[logFileSize];
			DWORD bytesRead;
			ReadFile(logFile, fileContents, logFileSize, &bytesRead, NULL);
		}

		// Move the file pointer to the beginning
		SetFilePointer(logFile, 0, NULL, FILE_BEGIN);

		// Write the new line to the file
		DWORD bytesWritten;
		WriteFile(logFile, _message, lstrlenW(_message) * sizeof(WCHAR), &bytesWritten, NULL);

		if (logFileSize > 0) {
			// Write the old contents back to the file
			WriteFile(logFile, fileContents, logFileSize, &bytesWritten, NULL);

			// Cleanup
			delete[] fileContents;
		}

		// Cleanup and return.
		CloseHandle(logFile);
	}
	catch (...) {}
}
EZ::Error::~Error() {
	try {
		if (_message != NULL) {
			delete[] _message;
		}
	}
	catch (...) {}
}

void EZ::Error::ThrowFromCode(DWORD errorCode, LPCSTR file, int line) {
	if (errorCode != 0) {
		throw EZ::Error(errorCode, file, line);
	}
}
void EZ::Error::ThrowFromHR(HRESULT hr, LPCSTR file, int line) {
	if (!SUCCEEDED(hr)) {
		throw EZ::Error(hr, file, line);
	}
}