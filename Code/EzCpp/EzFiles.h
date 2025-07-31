// Approved 11/14/2024
#pragma once
#include "Windows.h"

BYTE* EzReadFileA(LPCSTR filePath, UINT32* pFileContentsLength);
BYTE* EzReadFileW(LPCWSTR filePath, UINT32* pFileContentsLength);
#ifdef UNICODE
#define EzReadFile EzReadFileW
#else
#define EzReadFile EzReadFileA
#endif // UNICODE

LPSTR EzReadFileTextA(LPCSTR filePath);
LPWSTR EzReadFileTextW(LPCWSTR filePath);
#ifdef UNICODE
#define EzReadFileText EzReadFileTextW
#else
#define EzReadFileText EzReadFileTextA
#endif // UNICODE

void EzWriteFileA(LPCSTR filePath, const BYTE* fileContents, UINT32 fileContentsLength);
void EzWriteFileW(LPCWSTR filePath, const BYTE* fileContents, UINT32 fileContentsLength);
#ifdef UNICODE
#define EzWriteFile EzWriteFileW
#else
#define EzWriteFile EzWriteFileA
#endif // UNICODE

void EzWriteFileTextA(LPCSTR filePath, LPCSTR fileContents);
void EzWriteFileTextW(LPCWSTR filePath, LPCWSTR fileContents);
#ifdef UNICODE
#define EzWriteFileText EzWriteFileTextW
#else
#define EzWriteFileText EzWriteFileTextA
#endif // UNICODE