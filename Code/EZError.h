#pragma once
#include <Windows.h>

class EzError {
public:
	EzError(DWORD errorCode, LPCSTR file = NULL, int line = -1);
	EzError(HRESULT hr, LPCSTR file = NULL, int line = -1);
	EzError(LONGLONG ntLonger, LPCSTR file = NULL, int line = -1);
	EzError(LPCWSTR message, LPCSTR file = NULL, int line = -1);
	void Print() const;
	LPCWSTR GetMessage() const;
	DWORD GetErrorCode() const;
	HRESULT GetHR() const;
	NTSTATUS GetNT() const;
	~EzError();

	EzError(const EzError& other);
	EzError& operator=(const EzError& other);

	static void ThrowFromCode(DWORD errorCode, LPCSTR file = NULL, int line = -1);
	static void ThrowFromHR(HRESULT hr, LPCSTR file = NULL, int line = -1);
	static void ThrowFromNT(NTSTATUS nt, LPCSTR file = NULL, int line = -1);

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