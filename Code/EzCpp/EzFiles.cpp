#include "EzFiles.h"
#include "EzError.h"

BYTE* EzReadFileA(LPCSTR filePath, UINT32* pFileContentsLength) {
	DWORD lastError = 0;

	if (filePath == NULL) {
		throw EzError("filePath cannot be null.", __FILE__, __LINE__);
	}

	HANDLE fileHandle = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		lastError = GetLastError();
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	UINT32 fileSize = 0;
	if (!GetFileSizeEx(fileHandle, reinterpret_cast<LARGE_INTEGER*>(&fileSize))) {
		lastError = GetLastError();
		CloseHandle(fileHandle);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	BYTE* fileContents = new BYTE[fileSize];
	if (fileContents == NULL) {
		throw EzError("failed to allocate memory for fileContents", __FILE__, __LINE__);
	}


	if (!ReadFile(fileHandle, fileContents, fileSize, NULL, NULL)) {
		lastError = GetLastError();
		delete[] fileContents;
		CloseHandle(fileHandle);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	if (!CloseHandle(fileHandle)) {
		lastError = GetLastError();
		delete[] fileContents;
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	if (pFileContentsLength != NULL) {
		*pFileContentsLength = fileSize;
	}
	return fileContents;
}
BYTE* EzReadFileW(LPCWSTR filePath, UINT32* pFileContentsLength) {
	DWORD lastError = 0;

	if (filePath == NULL) {
		throw EzError("filePath cannot be null.", __FILE__, __LINE__);
	}

	HANDLE fileHandle = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		lastError = GetLastError();
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	UINT32 fileSize = 0;
	if (!GetFileSizeEx(fileHandle, reinterpret_cast<LARGE_INTEGER*>(&fileSize))) {
		lastError = GetLastError();
		CloseHandle(fileHandle);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	BYTE* fileContents = new BYTE[fileSize];
	if (fileContents == NULL) {
		throw EzError("failed to allocate memory for fileContents", __FILE__, __LINE__);
	}


	if (!ReadFile(fileHandle, fileContents, fileSize, NULL, NULL)) {
		lastError = GetLastError();
		delete[] fileContents;
		CloseHandle(fileHandle);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	if (!CloseHandle(fileHandle)) {
		lastError = GetLastError();
		delete[] fileContents;
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	if (pFileContentsLength != NULL) {
		*pFileContentsLength = fileSize;
	}
	return fileContents;
}

LPSTR EzReadFileTextA(LPCSTR filePath) {
	DWORD lastError = 0;

	if (filePath == NULL) {
		throw EzError("filePath cannot be null.", __FILE__, __LINE__);
	}

	HANDLE fileHandle = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		lastError = GetLastError();
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	UINT32 fileSize = 0;
	if (!GetFileSizeEx(fileHandle, reinterpret_cast<LARGE_INTEGER*>(&fileSize))) {
		lastError = GetLastError();
		CloseHandle(fileHandle);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	UINT32 fileContentsLength = fileSize / sizeof(CHAR);
	LPSTR fileContents = new CHAR[fileContentsLength + 1];
	if (fileContents == NULL) {
		throw EzError("failed to allocate memory for fileContents", __FILE__, __LINE__);
	}
	fileContents[fileContentsLength] = '\0';

	if (!ReadFile(fileHandle, fileContents, fileSize, NULL, NULL)) {
		lastError = GetLastError();
		delete[] fileContents;
		CloseHandle(fileHandle);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	if (!CloseHandle(fileHandle)) {
		lastError = GetLastError();
		delete[] fileContents;
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	return fileContents;
}
LPWSTR EzReadFileTextW(LPCWSTR filePath) {
	DWORD lastError = 0;

	if (filePath == NULL) {
		throw EzError("filePath cannot be null.", __FILE__, __LINE__);
	}

	HANDLE fileHandle = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		lastError = GetLastError();
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	UINT32 fileSize = 0;
	if (!GetFileSizeEx(fileHandle, reinterpret_cast<LARGE_INTEGER*>(&fileSize))) {
		lastError = GetLastError();
		CloseHandle(fileHandle);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	UINT32 fileContentsLength = fileSize / sizeof(WCHAR);
	LPWSTR fileContents = new WCHAR[fileContentsLength + 1];
	if (fileContents == NULL) {
		throw EzError("failed to allocate memory for fileContents", __FILE__, __LINE__);
	}
	fileContents[fileContentsLength] = '\0';

	if (!ReadFile(fileHandle, fileContents, fileSize, NULL, NULL)) {
		lastError = GetLastError();
		delete[] fileContents;
		CloseHandle(fileHandle);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	if (!CloseHandle(fileHandle)) {
		lastError = GetLastError();
		delete[] fileContents;
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	return fileContents;
}

void EzWriteFileA(LPCSTR filePath, const BYTE* fileContents, UINT32 fileContentsLength) {
	DWORD lastError = 0;

	if (filePath == NULL) {
		throw EzError("filePath cannot be null.", __FILE__, __LINE__);
	}

	HANDLE fileHandle = CreateFileA(filePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		lastError = GetLastError();
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	DWORD bytesWritten = 0;
	if (!WriteFile(fileHandle, fileContents, fileContentsLength, &bytesWritten, NULL) || bytesWritten != fileContentsLength) {
		lastError = GetLastError();
		CloseHandle(fileHandle);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	if (!CloseHandle(fileHandle)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
void EzWriteFileW(LPCWSTR filePath, const BYTE* fileContents, UINT32 fileContentsLength) {
	DWORD lastError = 0;

	if (filePath == NULL) {
		throw EzError("filePath cannot be null.", __FILE__, __LINE__);
	}

	HANDLE fileHandle = CreateFileW(filePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		lastError = GetLastError();
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	DWORD bytesWritten = 0;
	if (!WriteFile(fileHandle, fileContents, fileContentsLength, &bytesWritten, NULL) || bytesWritten != fileContentsLength) {
		lastError = GetLastError();
		CloseHandle(fileHandle);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	if (!CloseHandle(fileHandle)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

void EzWriteFileTextA(LPCSTR filePath, LPCSTR fileContents) {
	EzWriteFileA(filePath, reinterpret_cast<const BYTE*>(fileContents), lstrlenA(fileContents) * sizeof(CHAR));
}
void EzWriteFileTextW(LPCWSTR filePath, LPCWSTR fileContents) {
	EzWriteFileW(filePath, reinterpret_cast<const BYTE*>(fileContents), lstrlenW(fileContents) * sizeof(WCHAR));
}