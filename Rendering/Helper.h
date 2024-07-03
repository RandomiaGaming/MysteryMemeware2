#pragma once
#include <Windows.h>

class Error {
public:
	enum ErrorDisposal : BYTE {
		Delete = 0, // Error message buffer is freed with C++ style delete[].
		Free = 1, // Error message buffer is freed with C-Style free(void* _Block) function.
		LocalFree = 3, // Message buffer is freed with Win32 API LocalFree(HLOCAL hMem) from WinBase.h.
	};

	Error(LPCWSTR message);
	Error(LPCSTR message);
	Error(LPWSTR message, ErrorDisposal disposal = ErrorDisposal::Delete);
	Error(LPSTR message, ErrorDisposal disposal = ErrorDisposal::Delete);
	void Print();
	~Error();

private:
	BOOL _wideMessage = TRUE;
	BOOL _constMessage = TRUE;
	ErrorDisposal _disposal = ErrorDisposal::Delete;
	void* _message = NULL;
};

void PrintError(LPCWSTR errorMessage);
void PrintError(LPCSTR errorMessage);
void PrintWarning(LPCWSTR warningMessage);
void PrintWarning(LPCSTR warningMessage);

void PressAnyKey();
void ThrowSysError();