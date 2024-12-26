// Approved 11/14/2024

#include "EzError.h"
#include "EzConsole.h"
#include <Windows.h>
#include <comdef.h>
#include <sstream>
#include <iomanip>

static constexpr LPCSTR ErrorLogFilePath = "C:\\ProgramData\\EzLog.txt";

static enum class ErrorSource : BYTE {
	CustomString = 0,
	DosErrorCode = 1,
	HResult = 2,
	NTStatus = 3,
};
static LPSTR ConstructMessage(LPCSTR errorMessage, ErrorSource sourceType, void* source, LPCSTR file, UINT32 line) noexcept {
	try {
		std::ostringstream messageStream = { };

		// Append file name
		if (file == NULL) {
			messageStream << "ERROR in UnknownFile";
		}
		else {
			LPCSTR fileNameOnly = file + lstrlenA(file);
			while (fileNameOnly >= file && *fileNameOnly != '\\' && *fileNameOnly != '/') {
				fileNameOnly--;
			}
			messageStream << "ERROR in " << (fileNameOnly + 1);
		}

		// Append line number
		if (line == 0xFFFFFFFF) {
			messageStream << " at UnknownLine";
		}
		else {
			messageStream << " at line " << line;
		}

		// Append current time
		SYSTEMTIME timeNow = { };
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

		// Append error source
		if (sourceType == ErrorSource::DosErrorCode) {
			messageStream << " from DOS error code 0x" << std::hex << std::setw(sizeof(DWORD) * 2) << std::setfill('0')
				<< *reinterpret_cast<DWORD*>(source)
				<< std::setfill(' ') << std::setw(0) << std::dec;
		}
		else if (sourceType == ErrorSource::HResult) {
			messageStream << " from HResult 0x" << std::hex << std::setw(sizeof(HRESULT) * 2) << std::setfill('0')
				<< *reinterpret_cast<HRESULT*>(source)
				<< std::setfill(' ') << std::setw(0) << std::dec;
		}
		else if (sourceType == ErrorSource::NTStatus) {
			messageStream << " from NTStatus 0x" << std::hex << std::setw(sizeof(NTSTATUS) * 2) << std::setfill('0')
				<< *reinterpret_cast<NTSTATUS*>(source)
				<< std::setfill(' ') << std::setw(0) << std::dec;
		}

		// Append error message
		if (errorMessage == NULL) {
			messageStream << ": UnknownMessage\r\n";
		}
		else
		{
			messageStream << ": " << errorMessage;
			UINT32 errorMessageLength = lstrlenA(errorMessage);
			if (errorMessageLength >= 2) {
				LPCSTR lastTwoChars = errorMessage + (errorMessageLength - 2);
				if (lastTwoChars[0] != '\r' || lastTwoChars[1] != '\n') {
					messageStream << "\r\n";
				}
			}
		}

		// Copy string and return
		std::string messageString = messageStream.str();
		LPSTR message = new CHAR[messageString.size() + 1];
		lstrcpyA(message, messageString.c_str());
		return message;
	}
	catch (...) {
		return NULL;
	}
}
static LPSTR NarrowString(LPCWSTR wideStr) noexcept {
	DWORD bufferSize = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);

	LPSTR narrowStr = new CHAR[bufferSize];

	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, narrowStr, bufferSize, NULL, NULL);

	return narrowStr;
}

EzError::EzError(std::exception ex, LPCSTR file, UINT32 line) noexcept {
	try {
		_errorCode = 0;
		_hr = 0;
		_nt = 0;

		_message = ConstructMessage(ex.what(), ErrorSource::CustomString, NULL, file, line);
	}
	catch (...) {}
}
EzError::EzError(DWORD errorCode, LPCSTR file, UINT32 line) noexcept {
	try {
		_errorCode = errorCode;
		_hr = 0;
		_nt = 0;

		LPSTR systemMessage = NULL;
		DWORD systemMessageLength = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, _errorCode, 0, reinterpret_cast<LPSTR>(&systemMessage), 0, NULL);

		_message = ConstructMessage(systemMessage, ErrorSource::DosErrorCode, &_errorCode, file, line);

		LocalFree(systemMessage);
	}
	catch (...) {}
}
EzError::EzError(HRESULT hr, LPCSTR file, UINT32 line) noexcept {
	try {
		_errorCode = 0;
		_hr = hr;
		_nt = 0;

		LPSTR systemMessage = NULL;
		DWORD systemMessageLength = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, _hr, 0, reinterpret_cast<LPSTR>(&systemMessage), 0, NULL);

		if (systemMessageLength > 0) {
			_message = ConstructMessage(systemMessage, ErrorSource::HResult, &_hr, file, line);

			LocalFree(systemMessage);
		}
		else {
			_com_error comError(_hr);
			LPSTR comErrorMessage = NarrowString(comError.ErrorMessage());

			_message = ConstructMessage(comErrorMessage, ErrorSource::HResult, &_hr, file, line);

			delete[] comErrorMessage;
		}
	}
	catch (...) {}
}
EzError::EzError(NTSTATUS* pNt, LPCSTR file, UINT32 line) noexcept {
	try {
		_errorCode = 0;
		_nt = *pNt;
		_hr = HRESULT_FROM_NT(_nt);

		LPSTR systemMessage = NULL;
		DWORD systemMessageLength = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, _hr, 0, reinterpret_cast<LPSTR>(&systemMessage), 0, NULL);

		if (systemMessageLength > 0) {
			_message = ConstructMessage(systemMessage, ErrorSource::NTStatus, &_nt, file, line);

			LocalFree(systemMessage);
		}
		else {
			_com_error comError(_hr);
			LPSTR comErrorMessage = NarrowString(comError.ErrorMessage());

			_message = ConstructMessage(comErrorMessage, ErrorSource::NTStatus, &_nt, file, line);

			delete[] comErrorMessage;
		}
	}
	catch (...) {}
}
EzError::EzError(LPCWSTR message, LPCSTR file, UINT32 line) noexcept {
	try {
		_errorCode = 0;
		_hr = 0;
		_nt = 0;

		LPSTR narrowMessage = NarrowString(message);
		_message = ConstructMessage(narrowMessage, ErrorSource::CustomString, NULL, file, line);
		delete[] narrowMessage;
	}
	catch (...) {}
}
EzError::EzError(LPCSTR message, LPCSTR file, UINT32 line) noexcept {
	try {
		_errorCode = 0;
		_hr = 0;
		_nt = 0;

		_message = ConstructMessage(message, ErrorSource::CustomString, NULL, file, line);
	}
	catch (...) {}
}

EzError::~EzError() noexcept {
	try {
		if (_message != NULL) {
			try {
				delete[] _message;
			}
			catch (...) {}
			_message = NULL;
		}

		_errorCode = 0;
		_hr = 0;
		_nt = 0;
	}
	catch (...) {}
}
EzError::EzError(const EzError& other) noexcept {
	_errorCode = other._errorCode;
	_hr = other._hr;
	_nt = other._nt;

	if (other._message != NULL) {
		size_t messageLength = lstrlenA(other._message) + 1;
		_message = new CHAR[messageLength];
		lstrcpyA(_message, other._message);
	}
	else {
		_message = NULL;
	}
}
EzError& EzError::operator=(const EzError& other) noexcept {
	if (this != &other) {
		this->~EzError();

		_errorCode = other._errorCode;
		_hr = other._hr;
		_nt = other._nt;

		if (other._message != NULL) {
			size_t messageLength = lstrlenA(other._message) + 1;
			_message = new CHAR[messageLength];
			lstrcpyA(_message, other._message);
		}
		else {
			_message = NULL;
		}
	}
	return *this;
}

void EzError::Print() const noexcept {
	try {
		HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (stdoutHandle == INVALID_HANDLE_VALUE) {
			// If this fails we need to give up
			goto printFailed;
		}

		CONSOLE_SCREEN_BUFFER_INFO consoleInfo = { };
		WORD savedAttributes = 0;
		if (!GetConsoleScreenBufferInfo(stdoutHandle, &consoleInfo)) {
			// Don't care about original color
		}
		else {
			savedAttributes = consoleInfo.wAttributes;
		}

		if (!SetConsoleTextAttribute(stdoutHandle, static_cast<WORD>(EzConsole::Color::Red))) {
			// Don't care about the color
		}

		int messageLength = lstrlenA(_message);
		if (messageLength == 0) {
			// If this fails assume the string is probably 64 characters long
			messageLength = 64;
		}

		DWORD charsWritten = 0;
		if (!WriteConsoleA(stdoutHandle, _message, messageLength, &charsWritten, NULL) || charsWritten != messageLength) {
			// Don't care if write fails or is a partial write
		}

		if (!SetConsoleTextAttribute(stdoutHandle, savedAttributes)) {
			// Don't care if restoring original color fails
		}
	}
	catch (...) {}

printFailed:
	try {
		// Try to open the log file
		// If we can't then create a new one
		// If that fails then try again without read access
		// If all else fails then just print the error don't write it to the log file.
		HANDLE logFile = CreateFileA(ErrorLogFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (logFile == INVALID_HANDLE_VALUE) {
			logFile = CreateFileA(ErrorLogFilePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (logFile == INVALID_HANDLE_VALUE) {
				goto writeFailed;
			}
		}

		LONGLONG fileSize = 0;
		if (!GetFileSizeEx(logFile, reinterpret_cast<PLARGE_INTEGER>(&fileSize))) {
			// Don't care just assume the file is 10 kilobytes.
			fileSize = 10240;
		}

		BYTE* fileContents = NULL;
		try {
			fileContents = new BYTE[fileSize];
		}
		catch (...) {
			// Don't care just write to the file only
			fileContents = NULL;
			fileSize = 0;
		}

		if (fileContents != NULL) {
			DWORD bytesRead = 0;
			if (!ReadFile(logFile, fileContents, fileSize, &bytesRead, NULL)) {
				// Don't care about failed reads
			}
			if (bytesRead != fileSize) {
				// If we could only read part of the file that's fine just be happy with what we got
				fileSize = bytesRead;
			}
		}

		// Move the file pointer to the beginning
		if (SetFilePointer(logFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
			// If the pointer wont move then just write to where the pointer is now
			try {
				if (fileContents != NULL) {
					delete[] fileContents;
					fileContents = NULL;
				}
			}
			catch (...) {
				// Don't care if this fails just leak the memory
			}
			fileSize = 0;
		}

		int messageLength = lstrlenA(_message);
		if (messageLength == 0) {
			// If this fails assume the string is probably 64 characters long
			messageLength = 64;
		}

		DWORD bytesWrittenMessage = 0;
		if (!WriteFile(logFile, _message, messageLength * sizeof(CHAR), &bytesWrittenMessage, NULL)) {
			// If we couldn't write to the file then give up
			if (!CloseHandle(logFile)) {
				// Don't care if the handle won't close
			}
			try {
				if (fileContents != NULL) {
					delete[] fileContents;
					fileContents = NULL;
				}
			}
			catch (...) {
				// Don't care if this fails just leak the memory
			}
			goto writeFailed;
		}
		if (bytesWrittenMessage != messageLength) {
			// Don't care if we only wrote half the message just be happy we wrote anything at all
		}

		DWORD bytesWrittenFile = 0;
		if (!WriteFile(logFile, fileContents, fileSize, &bytesWrittenFile, NULL) || bytesWrittenFile != fileSize) {
			// Don't care if we couldn't write the file contents
		}

		if (!CloseHandle(logFile)) {
			// Don't care if the handle won't close
		}

		try {
			if (fileContents != NULL) {
				delete[] fileContents;
				fileContents = NULL;
			}
		}
		catch (...) {
			// Don't care if this fails just leak the memory
		}
	}
	catch (...) {}
writeFailed:
	return;
}
LPCSTR EzError::GetMessagePLZ() const noexcept {
	try {
		return _message;
	}
	catch (...) {
		return NULL;
	}
}
DWORD EzError::GetErrorCode() const noexcept {
	try {
		return _errorCode;
	}
	catch (...) {
		return 0;
	}
}
HRESULT EzError::GetHR() const noexcept {
	try {
		return _hr;
	}
	catch (...) {
		return 0;
	}
}
NTSTATUS EzError::GetNT() const noexcept {
	try {
		return _nt;
	}
	catch (...) {
		return 0;
	}
}

void EzError::ThrowFromException(std::exception ex, LPCSTR file, UINT32 line) {
	throw EzError::EzError(ex, file, line);
}
void EzError::ThrowFromCode(DWORD errorCode, LPCSTR file, UINT32 line) {
	throw EzError(errorCode, file, line);
}
void EzError::ThrowFromHR(HRESULT hr, LPCSTR file, UINT32 line) {
	throw EzError(hr, file, line);
}
void EzError::ThrowFromNT(NTSTATUS nt, LPCSTR file, UINT32 line) {
	throw EzError(nt, file, line);
}

static LPCSTR SECodeToMessage(DWORD code) {
	switch (code) {
	case EXCEPTION_ACCESS_VIOLATION: return "Access violation (Segmentation fault)";
	case EXCEPTION_DATATYPE_MISALIGNMENT: return "Data type misalignment";
	case EXCEPTION_BREAKPOINT: return "Breakpoint";
	case EXCEPTION_SINGLE_STEP: return "Single step";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "Array bounds exceeded";
	case EXCEPTION_FLT_DENORMAL_OPERAND: return "Float denormal operand";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO: return "Float divide by zero";
	case EXCEPTION_FLT_INEXACT_RESULT: return "Float inexact result";
	case EXCEPTION_FLT_INVALID_OPERATION: return "Float invalid operation";
	case EXCEPTION_FLT_OVERFLOW: return "Float overflow";
	case EXCEPTION_FLT_STACK_CHECK: return "Float stack check";
	case EXCEPTION_FLT_UNDERFLOW: return "Float underflow";
	case EXCEPTION_INT_DIVIDE_BY_ZERO: return "Integer divide by zero";
	case EXCEPTION_INT_OVERFLOW: return "Integer overflow";
	case EXCEPTION_PRIV_INSTRUCTION: return "Priv instruction";
	case EXCEPTION_IN_PAGE_ERROR: return "In page error";
	case EXCEPTION_ILLEGAL_INSTRUCTION: return "Illegal instruction";
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Non-continuable exception";
	case EXCEPTION_STACK_OVERFLOW: return "Stack overflow";
	case EXCEPTION_INVALID_DISPOSITION: return "Invalid disposition";
	case EXCEPTION_GUARD_PAGE: return "Guard page";
	case EXCEPTION_INVALID_HANDLE: return "Invalid handle";
	case CONTROL_C_EXIT: return "DLL Initialization Failed";
	default: return "Unknown SEH Exception";
	}
}
static void SE_Translator(unsigned int code, EXCEPTION_POINTERS* pExp) {
	DWORD exceptionCode = pExp->ExceptionRecord->ExceptionCode;
	LPCSTR message = SECodeToMessage(exceptionCode);
	throw EzError(message, __FILE__, __LINE__);
}
void EzError::SetSEHandler() noexcept {
	_set_se_translator(SE_Translator);
}