#include "EZError.h"
#include <Windows.h>
#include <comdef.h>
#include <sstream>
#include <winternl.h>

#pragma comment(lib, "ntdll.lib")

// Formats multiple pieces of data into a full error message and allocates that string on the heap.
LPWSTR ConstructMessage(LPCWSTR errorMessage, LPCSTR file, int line) {
	try {
		std::wostringstream messageStream;

		if (file == NULL) { messageStream << "ERROR in UnknownFile"; }
		else {
			LPCSTR fileNameOnly = file + lstrlenA(file);
			while (fileNameOnly >= file && *fileNameOnly != '\\') { fileNameOnly--; }
			messageStream << "ERROR in " << (fileNameOnly + 1);
		}

		if (line < 0) { messageStream << " at UnknownLine"; }
		else { messageStream << " at line " << line; }

		SYSTEMTIME timeNow;
		GetLocalTime(&timeNow);
		if (timeNow.wHour == 0) {
			messageStream << " at 12:" << timeNow.wMinute << ":" << timeNow.wSecond << "am";
		}
		else if (timeNow.wHour < 12) {
			messageStream << " at " << (timeNow.wHour % 12) << ":" << timeNow.wMinute << ":" << timeNow.wSecond << "am";
		}
		else {
			messageStream << " at " << (timeNow.wHour % 12) << ":" << timeNow.wMinute << ":" << timeNow.wSecond << "pm";
		}
		messageStream << " on " << timeNow.wMonth << "/" << timeNow.wDay << "/" << timeNow.wYear;

		messageStream << ": " << errorMessage;

		DWORD errorMessageLength = lstrlenW(errorMessage);
		if (errorMessageLength >= 2) {
			LPCWSTR lastTwoChars = errorMessage + (errorMessageLength - 2);
			if (lastTwoChars[0] != L'\r' || lastTwoChars[1] != L'\n') {
				messageStream << L"\r\n";
			}
		}

		std::wstring messageString = messageStream.str();
		LPWSTR message = new WCHAR[messageString.size() + 1];
		lstrcpyW(message, messageString.c_str());

		return message;
	}
	catch (...) { return NULL; }
}

EZ::Error::Error(DWORD errorCode, LPCSTR file, int line) {
	try {
		DWORD errorCode = GetLastError();

		LPWSTR errorMessage = NULL;
		DWORD systemErrorLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&errorMessage), 0, NULL);

		_message = ConstructMessage(errorMessage, file, line);

		LocalFree(errorMessage);
	}
	catch (...) {}
}
EZ::Error::Error(HRESULT hr, LPCSTR file, int line) {
	try {
		LPWSTR errorMessage = NULL;
		DWORD errorMessageLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&errorMessage), 0, NULL);

		if (errorMessageLength > 0) {
			_message = ConstructMessage(errorMessage, file, line);

			LocalFree(errorMessage);
		}
		else {
			_com_error comError(hr);
			LPCWSTR comErrorMessage = comError.ErrorMessage();

			_message = ConstructMessage(comErrorMessage, file, line);
		}
	}
	catch (...) {}
}
EZ::Error::Error(LONGLONG ntLonger, LPCSTR file, int line) {
	try {
		NTSTATUS nt = static_cast<NTSTATUS>(ntLonger);

		DWORD errorCode = RtlNtStatusToDosError(nt);
		if (errorCode != ERROR_MR_MID_NOT_FOUND) {
			LPWSTR errorMessage = NULL;
			DWORD systemErrorLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&errorMessage), 0, NULL);

			_message = ConstructMessage(errorMessage, file, line);

			LocalFree(errorMessage);
		}
		else {
			HRESULT hr = HRESULT_FROM_NT(nt);

			LPWSTR errorMessage = NULL;
			DWORD errorMessageLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&errorMessage), 0, NULL);

			if (errorMessageLength > 0) {
				_message = ConstructMessage(errorMessage, file, line);

				LocalFree(errorMessage);
			}
			else {
				_com_error comError(hr);
				LPCWSTR comErrorMessage = comError.ErrorMessage();

				_message = ConstructMessage(comErrorMessage, file, line);
			}
		}
	}
	catch (...) {}
}
EZ::Error::Error(LPCWSTR errorMessage, LPCSTR file, int line) {
	try {
		_message = ConstructMessage(errorMessage, file, line);
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
void EZ::Error::ThrowFromNT(NTSTATUS nt, LPCSTR file, int line) {
	if (!SUCCEEDED(nt)) {
		throw EZ::Error(static_cast<LONGLONG>(nt), file, line);
	}
}