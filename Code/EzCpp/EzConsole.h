// Approved 11/14/2024

#pragma once
#include <Windows.h>

namespace EzConsole {
	// Note the following is the meaning of each bit in the wAttributes field of the CONSOLE_SCREEN_BUFFER_INFO structure
	// Bit 1 = Foreground Blue
	// Bit 2 = Foreground Green
	// Bit 3 = Foreground Red
	// Bit 4 = Foreground Intensity
	// Bit 5 = Background Blue
	// Bit 6 = Background Green
	// Bit 7 = Background Red
	// Bit 8 = Background Intensity
	// Bit 9 = Leading Byte Of DBCS I think
	// Bit 10 = Trailing Byte Of DBCS I think
	// Bit 11 = Line On Top
	// Bit 12 = Line On Left
	// Bit 13 = Line On Right
	// Bit 14 = SBCS Or DBCS Flag I think
	// Bit 15 = Reverse Foreground And Background I think
	// Bit 16 = Line On Bottom

	// Stores the 16 possible colors supported by the windows console
	enum class Color : WORD {
		Black = 0,
		DarkBlue = FOREGROUND_BLUE,
		DarkGreen = FOREGROUND_GREEN,
		DarkCyan = FOREGROUND_GREEN | FOREGROUND_BLUE,
		DarkRed = FOREGROUND_RED,
		DarkMagenta = FOREGROUND_RED | FOREGROUND_BLUE,
		DarkYellow = FOREGROUND_RED | FOREGROUND_GREEN,
		DarkGrey = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		Grey = FOREGROUND_INTENSITY,
		Blue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		Green = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		Cyan = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		Red = FOREGROUND_RED | FOREGROUND_INTENSITY,
		Magenta = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		Yellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
	};

	// Gets the current console text color 
	EzConsole::Color GetColor();
	// Sets the current console text color
	void SetColor(EzConsole::Color color);
	// Gets the current console background color
	EzConsole::Color GetBackgroundColor();
	// Sets the current console background color
	void SetBackgroundColor(EzConsole::Color color);

	// Writes a message to the console using only APIs in Windows.h
	void WriteA(LPCSTR message);
	void WriteW(LPCWSTR message);
#ifdef UNICODE
#define Write WriteW
#else
#define Write WriteA
#endif // UNICODE

	// Writes a message plus a new line to the console using only APIs in Windows.h
	void WriteLineA(LPCSTR message);
	void WriteLineW(LPCWSTR message);
#ifdef UNICODE
#define WriteLine WriteLineW
#else
#define WriteLine WriteLineA
#endif // UNICODE

	// Reads a single character from the console using only APIs in Windows.h
	// Unlike with ReadLine ReadChar intercepts the character as soon as it is pressed
	// Because of this the character will not be echoed to the screen
	CHAR ReadCharA();
	WCHAR ReadCharW();
#ifdef UNICODE
#define ReadChar ReadCharW
#else
#define ReadChar ReadCharA
#endif // UNICODE

	// Reads a line from the console using only APIs in Windows.h
	// The outputted line is allocated with new[] and must be manually deleted
	LPSTR ReadLineA();
	LPWSTR ReadLineW();
#ifdef UNICODE
#define ReadLine ReadLineW
#else
#define ReadLine ReadLineA
#endif // UNICODE
};