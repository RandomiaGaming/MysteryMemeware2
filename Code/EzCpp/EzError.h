// Approved 11/14/2024

// KNOWN ISSUE
// You could go to VcxProj > Properties > Configuration Properties > C/C++ > Code Generation > Enable C++ Exceptions
// and set it to "Yes with SEH Exceptions (/EHa)" however this will cause SEH exceptions to propigate up the call stack
// as typeless exceptions which can only be caught by a (...) which is basically useless
// Instead use SetSEHandler()

#pragma once
#include <Windows.h>
#include <exception>

class EzError final {
public:
	explicit EzError(std::exception ex, LPCSTR file, UINT32 line) noexcept;
	explicit EzError(DWORD errorCode, LPCSTR file, UINT32 line) noexcept;
	explicit EzError(HRESULT hr, LPCSTR file, UINT32 line) noexcept;
	explicit EzError(NTSTATUS* pNt, LPCSTR file, UINT32 line) noexcept;
	explicit EzError(LPCWSTR message, LPCSTR file, UINT32 line) noexcept;
	explicit EzError(LPCSTR message, LPCSTR file, UINT32 line) noexcept;
	
	~EzError() noexcept;
	EzError(const EzError& other) noexcept;
	EzError& operator=(const EzError& other) noexcept;

	void Print() const noexcept;
	LPCSTR GetMessagePLZ() const noexcept;
	DWORD GetErrorCode() const noexcept;
	HRESULT GetHR() const noexcept;
	NTSTATUS GetNT() const noexcept;

	static void ThrowFromException(std::exception ex, LPCSTR file, UINT32 line);
	static void ThrowFromCode(DWORD errorCode, LPCSTR file, UINT32 line);
	static void ThrowFromHR(HRESULT hr, LPCSTR file, UINT32 line);
	static void ThrowFromNT(NTSTATUS nt, LPCSTR file, UINT32 line);

	static void SetSEHandler() noexcept;

private:
	LPSTR _message = NULL;
	DWORD _errorCode = 0;
	HRESULT _hr = 0;
	NTSTATUS _nt = 0;
};