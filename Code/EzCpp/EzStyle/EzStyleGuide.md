Approved 11/14/2024

# What to name stuff
All Ez things in the global namespace must begin with the letters "Ez"
EzGetTokenType not GetTokenType

All Ez things in Ez classes do not require the letters "Ez"
EzError::ThrowFromCode not EzError::EzThrowFromCode

Do not place anything in the Ez namespace
EzError not Ez::Error

Don't repeat yourself
EzError::ThrowFromCode not EzError::ThrowErrorFromCode

Don't place things into namespaces and don't use container classes
EzReadFileTextA not EzFile::ReadTextA

All Ez classes should be final unless they explicitly support inheritance

All Ez class constructors should be explicit

All Ez classes must have a destructor and that destructor should be noexcept

All Ez functions should be static if possible and noexcept if possible

All Ez class functions should be const if possible

Make sure to implament or explicitly delete all of the following in every Ez class:
Default constructor: MyClass();
Destructor: ~MyClass();
Copy constructor: MyClass(const MyClass&)
Copy assignment operator: MyClass& operator=(const MyClass&);
Move constructor: MyClass(MyClass&&);
Move assignment operator: MyClass& operator=(MyClass&&);

# Ez file structure
All header files in Ez must refrence Windows.h and use its functions where possible
lstrlen not strlen

All cpp files in Ez must refrence the associated header and each .h file must come paired with exactly one .cpp file with the same name
The only exception to this is dangling .h files where the header contains all the code and there is no need for a .cpp file
EzError.cpp includes EzError.h

Any functions which use strings must have an A and W version as well as a UNICODE macro in the header file to make their usage easier

Include statements which can be moved from the .h header to the .cpp source file should be

Always include #pragma once in each header file

If a #include statement can be removed from any file then it should be

Always assume the user already called CoInitializeEx and never call CoUninitialize

Always assume the user already wrapped all code in a try/catch for std::exceptions and EzErrors

Always assume the user already set a SE_Translator for the current scope

Always allocate memory with new and new[] and free with delete or delete[]

# Types and Macros
LPSTR not CHAR* or char*
LPWSTR not WCHAR* or wchar_t*
LPCSTR not const CHAR* or const char*
LPCWSTR not const WCHAR* or const wchar_t*
CHAR not char
WCHAR not wchar_t
void not VOID
BOOL not BOOLEAN or bool or int
FALSE not false
TRUE not true
BYTE not char or unsigned char
INT32 not INT or int
UINT32 not UINT or unsigned int or DWORD
INT64 not LONGLONG or long long
UINT64 not ULONGLONG or unsigned long long
BYTE* not LPBYTE
void* not LPVOID or VOID*
NULL not nullptr or 0

# Other Rules
Always hardcode A or W never use UNICODE Macros
ReadFileW not ReadFile

Function pointers must match exactly this includes passed structures. Even if this violates the rules above
typedef NTSTATUS(WINAPI* PRtlSetProcessIsCritical) (IN BOOLEAN bNew, OUT BOOLEAN* pbOld, IN BOOLEAN bNeedScb); not typedef NTSTATUS(WINAPI* PRtlSetProcessIsCritical) (BOOL bNew, BOOL* pbOld, BOOL bNeedScb);

Never use IN or OUT

For functions which output by refrence always start variable names with p
BYTE* Read(UINT32* pBufferLength) not BYTE* Read(UINT32* bufferLength)

const must be used if possible. If you can make a copy of a const input and modify that then do it

Throw an error if memory allocation fails. All calls to new or new[] must have error checking

Functions that return arrays should return the array pointer not the length

Functions that output by refrence should allow output ptrs to be NULL

# Errors and failing safely
Dont leak memory even when a function call fails. Always cleanup memory before throwing an exception

Always validate input at the top of any function

If a output by refrence variable is NULL then discard that output. If it was heap allocated don't forget to free it

Always call GetLastError before running cleanup code if there is an Error

Declare storage variables for error codes at the top of a function not at the site of the potential error

If cleanup code fails do not throw another error. Swallow any exceptions thrown by cleanup code if there is already an error.