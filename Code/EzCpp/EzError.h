// Approved 10/26/2024

#pragma once
#include <Windows.h>

// Note On SEH Exceptions:
// To ensure SEH exceptions are thrown like normal C++ exceptions go to
// Solution > Properties > Configuration Properties > C/C++ > Code Generation > Enable C++ Exceptions
// and set it to "Yes with SEH Exceptions (/EHa)".

class EzError final {
public:
	EzError(DWORD errorCode, LPCSTR file = NULL, UINT32 line = 0xFFFFFFFF);
	EzError(HRESULT hr, LPCSTR file = NULL, UINT32 line = 0xFFFFFFFF);
	EzError(LONGLONG ntLonger, LPCSTR file = NULL, UINT32 line = 0xFFFFFFFF);
	EzError(LPCSTR message, LPCSTR file = NULL, UINT32 line = 0xFFFFFFFF);
	EzError(LPCWSTR message, LPCSTR file = NULL, UINT32 line = 0xFFFFFFFF);
	void Print() const;
	LPCWSTR GetMessage() const;
	DWORD GetErrorCode() const;
	HRESULT GetHR() const;
	NTSTATUS GetNT() const;
	~EzError();

	EzError(const EzError& other);
	EzError& operator=(const EzError& other);

	static void ThrowFromCode(DWORD errorCode, LPCSTR file = NULL, UINT32 line = 0xFFFFFFFF);
	static void ThrowFromHR(HRESULT hr, LPCSTR file = NULL, UINT32 line = 0xFFFFFFFF);
	static void ThrowFromNT(NTSTATUS nt, LPCSTR file = NULL, UINT32 line = 0xFFFFFFFF);

private:
	LPWSTR _message = NULL;
	DWORD _errorCode = 0;
	HRESULT _hr = 0;
	NTSTATUS _nt = 0;
};

// This is unused but saved for future refrence cause it's useful.
enum class EzConsoleColor : WORD {
	Black = 0,
	DarkRed = FOREGROUND_RED,
	DarkGreen = FOREGROUND_GREEN,
	DarkBlue = FOREGROUND_BLUE,
	DarkYellow = FOREGROUND_RED | FOREGROUND_GREEN,
	DarkCyan = FOREGROUND_GREEN | FOREGROUND_BLUE,
	DarkMagenta = FOREGROUND_RED | FOREGROUND_BLUE,
	DarkGrey = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	Grey = FOREGROUND_INTENSITY,
	Red = FOREGROUND_RED | FOREGROUND_INTENSITY,
	Green = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	Blue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	Yellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	Cyan = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	Magenta = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
};