// Every Ez header must start with pragma once
#pragma once
// Every Ez header must include Windows.h and use it
#include <Windows.h>

// Every Ez function should be in the global namespace unless its in a specific class
// Every Ez function should be static if possible but this one cannot because it's publically accessible
// Every Ez function should be noexcept if possible but this one cannot because ReadFileA sometimes fails
// All output by refrence parameters must start with lowercase p like pFileContents
// All integers should have a specified size like UINT32
// Here we made filePath const because it doesn't have to be mutable
// Since EzReadFile uses strings we created an A and W version as well as a UNICODE macro
UINT32 EzReadFileA(LPCSTR filePath, BYTE** pFileContents);
UINT32 EzReadFileW(LPCWSTR filePath, BYTE** pFileContents);
// All UNICODE macros must have the little // UNICODE tag at the end
#ifdef UNICODE
#define EzReadFile EzReadFileW
#else
#define EzReadFile EzReadFileA
#endif // UNICODE