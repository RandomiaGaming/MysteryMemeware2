// All Ez cpp files must have an associated header and refrence that header
#include "EzExample.h"
// All errors must be thrown as EzErrors if possible
// We moved this include statement to the cpp file since it's not needed in the header
#include "../EzError.h"

UINT32 EzReadFileA(LPCSTR filePath, BYTE** pFileContents) {
	// Error code variable declared at the top of the function
	DWORD lastError = 0;

	// Start with input validation first
	if (filePath == NULL) {
		// All errors should be thrown as EzErrors
		// __FILE__ and __LINE__ are required
		throw EzError("filePath cannot be null.", __FILE__, __LINE__);
	}

	// Notice we hard coded CreateFileW instead of using the UNICODE macro
	HANDLE fileHandle = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		// As soon as something goes wrong we call GetLastError immediatly
		lastError = GetLastError();
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	LARGE_INTEGER fileSize = { 0 };
	if (GetFileSizeEx(fileHandle, &fileSize)) {
		// Notice we ignore errors in CloseHandle since cleanup code may not throw errors
		// We also call GetLastError before doing any cleanup to ensure the last error code is fresh
		lastError = GetLastError();
		CloseHandle(fileHandle);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	// Notice how if new[] fails we throw an error
	BYTE* fileContents = new BYTE[fileSize.QuadPart];
	if (fileContents == NULL) {
		throw EzError("failed to allocate memory for fileContents", __FILE__, __LINE__);
	}


	if (!ReadFile(fileHandle, fileContents, fileSize.QuadPart, NULL, NULL)) {
		// Don't forget to free any allocated memory when something fails
		lastError = GetLastError();
		delete[] fileContents;
		CloseHandle(fileHandle);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	// Notice two things about this:
	// 1. We don't change pFileContents until the very end so if there's an error pFileContents will be left unchanged
	// 2. If pFileContents is NULL not only do we not set it's value but we cleanup the allocated memory as well
	if (pFileContents != NULL) {
		*pFileContents = fileContents;
	}
	else {
		delete[] fileContents;
	}
	return fileSize.QuadPart;
}
UINT32 EzReadFileW(LPCWSTR filePath, BYTE** pFileContents) {
	// Left blank for demonstration purposes.
	return 0;
}