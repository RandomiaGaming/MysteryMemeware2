#include "EzError.h"
#include <Windows.h>
#include <comdef.h>
#include <sstream>
#include <iomanip>

enum class ErrorSource : BYTE {
	CustomString = 0,
	DosErrorCode = 1,
	HResult = 2,
	NTStatus = 3,
};
static LPWSTR ConstructMessage(LPCWSTR errorMessage, ErrorSource sourceType = ErrorSource::CustomString, void* source = NULL, LPCSTR file = NULL, int line = -1) {
	try {
		std::wostringstream messageStream;

		if (file == NULL) { messageStream << L"ERROR in UnknownFile"; }
		else {
			LPCSTR fileNameOnly = file + lstrlenA(file);
			while (fileNameOnly >= file && *fileNameOnly != '\\') { fileNameOnly--; }
			messageStream << L"ERROR in " << (fileNameOnly + 1);
		}

		if (line < 0) { messageStream << L" at UnknownLine"; }
		else { messageStream << L" at line " << line; }

		SYSTEMTIME timeNow;
		GetLocalTime(&timeNow);
		if (timeNow.wHour == 0) {
			messageStream << L" at 12:" << timeNow.wMinute << L":" << timeNow.wSecond << L"am";
		}
		else if (timeNow.wHour < 12) {
			messageStream << L" at " << (timeNow.wHour % 12) << L":" << timeNow.wMinute << L":" << timeNow.wSecond << L"am";
		}
		else {
			messageStream << L" at " << (timeNow.wHour % 12) << L":" << timeNow.wMinute << L":" << timeNow.wSecond << L"pm";
		}
		messageStream << L" on " << timeNow.wMonth << L"/" << timeNow.wDay << L"/" << timeNow.wYear;

		if (sourceType == ErrorSource::DosErrorCode) {
			messageStream << L" from DOS error code 0x" << std::hex << std::setw(sizeof(DWORD) * 2) << std::setfill(L'0')
				<< *reinterpret_cast<DWORD*>(source)
				<< std::setfill(L' ') << std::setw(0) << std::dec;
		}
		else if (sourceType == ErrorSource::HResult) {
			messageStream << L" from HResult 0x" << std::hex << std::setw(sizeof(HRESULT) * 2) << std::setfill(L'0')
				<< *reinterpret_cast<HRESULT*>(source)
				<< std::setfill(L' ') << std::setw(0) << std::dec;
		}
		else if (sourceType == ErrorSource::NTStatus) {
			messageStream << L" from NTStatus 0x" << std::hex << std::setw(sizeof(NTSTATUS) * 2) << std::setfill(L'0')
				<< *reinterpret_cast<NTSTATUS*>(source)
				<< std::setfill(L' ') << std::setw(0) << std::dec;
		}

		messageStream << L": " << errorMessage;

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

EzError::EzError(DWORD errorCode, LPCSTR file, int line) {
	try {
		_errorCode = errorCode;
		_hr = 0;
		_nt = 0;

		LPWSTR errorMessage = NULL;
		DWORD systemErrorLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, _errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&errorMessage), 0, NULL);

		_message = ConstructMessage(errorMessage, ErrorSource::DosErrorCode, &_errorCode, file, line);

		LocalFree(errorMessage);
	}
	catch (...) {}
}
EzError::EzError(HRESULT hr, LPCSTR file, int line) {
	try {
		_errorCode = 0;
		_hr = hr;
		_nt = 0;

		LPWSTR errorMessage = NULL;
		DWORD errorMessageLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, _hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&errorMessage), 0, NULL);

		if (errorMessageLength > 0) {
			_message = ConstructMessage(errorMessage, ErrorSource::HResult, &_hr, file, line);

			LocalFree(errorMessage);
		}
		else {
			_com_error comError(_hr);
			LPCWSTR comErrorMessage = comError.ErrorMessage();

			_message = ConstructMessage(comErrorMessage, ErrorSource::HResult, &_hr, file, line);
		}
	}
	catch (...) {}
}
EzError::EzError(LONGLONG ntLonger, LPCSTR file, int line) {
	try {
		_errorCode = 0;
		_nt = static_cast<NTSTATUS>(ntLonger);
		_hr = HRESULT_FROM_NT(_nt);

		LPWSTR errorMessage = NULL;
		DWORD errorMessageLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, _hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&errorMessage), 0, NULL);

		if (errorMessageLength > 0) {
			_message = ConstructMessage(errorMessage, ErrorSource::NTStatus, &_nt, file, line);

			LocalFree(errorMessage);
		}
		else {
			_com_error comError(_hr);
			LPCWSTR comErrorMessage = comError.ErrorMessage();

			_message = ConstructMessage(comErrorMessage, ErrorSource::NTStatus, &_nt, file, line);
		}
	}
	catch (...) {}
}
EzError::EzError(LPCWSTR errorMessage, LPCSTR file, int line) {
	try {
		_errorCode = 0;
		_hr = 0;
		_nt = 0;

		_message = ConstructMessage(errorMessage, ErrorSource::CustomString, NULL, file, line);
	}
	catch (...) {}
}

EzError::EzError(const EzError& other) {
	_errorCode = other._errorCode;
	_hr = other._hr;
	_nt = other._nt;

	if (other._message != NULL) {
		size_t messageLength = lstrlenW(other._message) + 1;
		_message = new WCHAR[messageLength];
		lstrcpyW(_message, other._message);
	}
	else {
		_message = NULL;
	}
}
EzError& EzError::operator=(const EzError& other) {
	if (this != &other) {
		_errorCode = other._errorCode;
		_hr = other._hr;
		_nt = other._nt;

		this->~EzError();
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

void EzError::Print() const {
	try {
		HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		// Get initial console attributes.
		CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
		GetConsoleScreenBufferInfo(stdoutHandle, &consoleInfo);
		WORD savedAttributes = consoleInfo.wAttributes;

		// Set console attributes to red text.
		SetConsoleTextAttribute(stdoutHandle, static_cast<WORD>(EzConsoleColor::Red));

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
			(void)ReadFile(logFile, fileContents, logFileSize, &bytesRead, NULL);
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
EzError::~EzError() {
	try {
		if (_message != NULL) {
			delete[] _message;
		}
	}
	catch (...) {}
}

LPCWSTR EzError::GetMessage() const {
	return _message;
}
DWORD EzError::GetErrorCode() const {
	return _errorCode;
}
HRESULT EzError::GetHR() const {
	return _hr;
}
NTSTATUS EzError::GetNT() const {
	return _nt;
}

void EzError::ThrowFromCode(DWORD errorCode, LPCSTR file, int line) {
	throw EzError(errorCode, file, line);
}
void EzError::ThrowFromHR(HRESULT hr, LPCSTR file, int line) {
	throw EzError(hr, file, line);
}
void EzError::ThrowFromNT(NTSTATUS nt, LPCSTR file, int line) {
	throw EzError(static_cast<LONGLONG>(nt), file, line);
}