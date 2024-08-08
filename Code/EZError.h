#pragma once
#include <Windows.h>

namespace EZ {
	enum class ConsoleColor : WORD {
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

	class Error {
	public:
		Error(DWORD errorCode, LPCSTR file = NULL, int line = -1);
		Error(HRESULT hr, LPCSTR file = NULL, int line = -1);
		Error(LONGLONG ntLonger, LPCSTR file = NULL, int line = -1);
		Error(LPCWSTR message, LPCSTR file = NULL, int line = -1);
		void Print();
		~Error();

		// Copy constructor and copy assignment operator.
		Error(const EZ::Error& other);
		EZ::Error& operator=(const EZ::Error& other);

		static void ThrowFromCode(DWORD errorCode, LPCSTR file = NULL, int line = -1);
		static void ThrowFromHR(HRESULT hr, LPCSTR file = NULL, int line = -1);
		static void ThrowFromNT(NTSTATUS nt, LPCSTR file = NULL, int line = -1);

	private:
		LPWSTR _message = NULL;
	};
}