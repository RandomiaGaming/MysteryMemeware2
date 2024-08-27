#pragma once
#include <Windows.h>
#include <ostream>

// Getting info about tokens
SID_AND_ATTRIBUTES EzGetTokenUser(HANDLE token);
TOKEN_GROUPS* EzGetTokenGroups(HANDLE token);
TOKEN_PRIVILEGES* EzGetTokenPrivileges(HANDLE token);
PSID EzGetTokenOwner(HANDLE token);
PSID EzGetTokenPrimaryGroup(HANDLE token);
TOKEN_DEFAULT_DACL* EzGetTokenDefaultDacl(HANDLE token);
TOKEN_SOURCE EzGetTokenSource(HANDLE token);
TOKEN_TYPE EzGetTokenType(HANDLE token);
SECURITY_IMPERSONATION_LEVEL EzGetTokenImpersonationLevel(HANDLE token);
TOKEN_STATISTICS EzGetTokenStatistics(HANDLE token);
TOKEN_GROUPS* EzGetTokenRestrictedSids(HANDLE token);
DWORD EzGetTokenSessionId(HANDLE token);
TOKEN_GROUPS_AND_PRIVILEGES EzGetTokenGroupsAndPrivileges(HANDLE token);
BOOL EzGetTokenSandBoxInert(HANDLE token);
LUID EzGetTokenOrigin(HANDLE token);
TOKEN_ELEVATION_TYPE EzGetTokenElevationType(HANDLE token);
HANDLE EzGetTokenLinkedToken(HANDLE token);
BOOL EzGetTokenElevation(HANDLE token);
BOOL EzGetTokenHasRestrictions(HANDLE token);
TOKEN_ACCESS_INFORMATION EzGetTokenAccessInformation(HANDLE token);
BOOL EzGetTokenVirtualizationAllowed(HANDLE token);
BOOL EzGetTokenVirtualizationEnabled(HANDLE token);
SID_AND_ATTRIBUTES EzGetTokenIntegrityLevel(HANDLE token);
BOOL EzGetTokenUIAccess(HANDLE token);
DWORD EzGetTokenMandatoryPolicy(HANDLE token);
TOKEN_GROUPS* EzGetTokenLogonSid(HANDLE token);
BOOL EzGetTokenIsAppContainer(HANDLE token);
TOKEN_GROUPS* EzGetTokenCapabilities(HANDLE token);
PSID EzGetTokenAppContainerSid(HANDLE token);
DWORD EzGetTokenAppContainerNumber(HANDLE token);
CLAIM_SECURITY_ATTRIBUTES_INFORMATION* EzGetTokenUserClaimAttributes(HANDLE token);
CLAIM_SECURITY_ATTRIBUTES_INFORMATION* EzGetTokenDeviceClaimAttributes(HANDLE token);
TOKEN_GROUPS* EzGetTokenDeviceGroups(HANDLE token);
TOKEN_GROUPS* EzGetTokenRestrictedDeviceGroups(HANDLE token);
CLAIM_SECURITY_ATTRIBUTES_INFORMATION* EzGetTokenSecurityAttributes(HANDLE token);
BOOL EzGetTokenIsRestricted(HANDLE token);
PSID EzGetTokenProcessTrustLevel(HANDLE token);
BOOL EzGetTokenPrivateNameSpace(HANDLE token);
CLAIM_SECURITY_ATTRIBUTES_INFORMATION* EzGetTokenSingletonAttributes(HANDLE token);
TOKEN_BNO_ISOLATION_INFORMATION EzGetTokenBnoIsolation(HANDLE token);
BOOL EzGetTokenIsSandboxed(HANDLE token);

// Setting info about tokens
void EzSetTokenUser(HANDLE token, SID_AND_ATTRIBUTES value);
void EzSetTokenGroups(HANDLE token, TOKEN_GROUPS* value);
void EzSetTokenPrivileges(HANDLE token, TOKEN_PRIVILEGES* value);
void EzSetTokenOwner(HANDLE token, PSID value);
void EzSetTokenPrimaryGroup(HANDLE token, PSID value);
void EzSetTokenDefaultDacl(HANDLE token, TOKEN_DEFAULT_DACL* value);
void EzSetTokenSource(HANDLE token, TOKEN_SOURCE value);
void EzSetTokenType(HANDLE token, TOKEN_TYPE value);
void EzSetTokenImpersonationLevel(HANDLE token, SECURITY_IMPERSONATION_LEVEL value);
void EzSetTokenStatistics(HANDLE token, TOKEN_STATISTICS value);
void EzSetTokenRestrictedSids(HANDLE token, TOKEN_GROUPS* value);
void EzSetTokenSessionId(HANDLE token, DWORD value);
void EzSetTokenGroupsAndPrivileges(HANDLE token, TOKEN_GROUPS_AND_PRIVILEGES value);
void EzSetTokenSandBoxInert(HANDLE token, BOOL value);
void EzSetTokenOrigin(HANDLE token, LUID value);
void EzSetTokenElevationType(HANDLE token, TOKEN_ELEVATION_TYPE value);
void EzSetTokenLinkedToken(HANDLE token, HANDLE value);
void EzSetTokenElevation(HANDLE token, BOOL value);
void EzSetTokenHasRestrictions(HANDLE token, BOOL value);
void EzSetTokenAccessInformation(HANDLE token, TOKEN_ACCESS_INFORMATION value);
void EzSetTokenVirtualizationAllowed(HANDLE token, BOOL value);
void EzSetTokenVirtualizationEnabled(HANDLE token, BOOL value);
void EzSetTokenIntegrityLevel(HANDLE token, SID_AND_ATTRIBUTES value);
void EzSetTokenUIAccess(HANDLE token, BOOL value);
void EzSetTokenMandatoryPolicy(HANDLE token, DWORD value);
void EzSetTokenLogonSid(HANDLE token, TOKEN_GROUPS* value);
void EzSetTokenIsAppContainer(HANDLE token, BOOL value);
void EzSetTokenCapabilities(HANDLE token, TOKEN_GROUPS* value);
void EzSetTokenAppContainerSid(HANDLE token, PSID value);
void EzSetTokenAppContainerNumber(HANDLE token, DWORD value);
void EzSetTokenUserClaimAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value);
void EzSetTokenDeviceClaimAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value);
void EzSetTokenDeviceGroups(HANDLE token, TOKEN_GROUPS* value);
void EzSetTokenRestrictedDeviceGroups(HANDLE token, TOKEN_GROUPS* value);
void EzSetTokenSecurityAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value);
void EzSetTokenIsRestricted(HANDLE token, BOOL value);
void EzSetTokenProcessTrustLevel(HANDLE token, PSID value);
void EzSetTokenPrivateNameSpace(HANDLE token, BOOL value);
void EzSetTokenSingletonAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value);
void EzSetTokenBnoIsolation(HANDLE token, TOKEN_BNO_ISOLATION_INFORMATION value);
void EzSetTokenIsSandboxed(HANDLE token, BOOL value);

// Logging info about tokens as text to a specified output stream.
void EzLogTokenUser(HANDLE token, std::wostream& outputStream);
void EzLogTokenGroups(HANDLE token, std::wostream& outputStream);
void EzLogTokenPrivileges(HANDLE token, std::wostream& outputStream);
void EzLogTokenOwner(HANDLE token, std::wostream& outputStream);
void EzLogTokenPrimaryGroup(HANDLE token, std::wostream& outputStream);
void EzLogTokenDefaultDacl(HANDLE token, std::wostream& outputStream);
void EzLogTokenSource(HANDLE token, std::wostream& outputStream);
void EzLogTokenType(HANDLE token, std::wostream& outputStream);
void EzLogTokenImpersonationLevel(HANDLE token, std::wostream& outputStream);
void EzLogTokenStatistics(HANDLE token, std::wostream& outputStream);
void EzLogTokenRestrictedSids(HANDLE token, std::wostream& outputStream);
void EzLogTokenSessionId(HANDLE token, std::wostream& outputStream);
void EzLogTokenGroupsAndPrivileges(HANDLE token, std::wostream& outputStream);
void EzLogTokenSessionReference(HANDLE token, std::wostream& outputStream);
void EzLogTokenSandBoxInert(HANDLE token, std::wostream& outputStream);
void EzLogTokenAuditPolicy(HANDLE token, std::wostream& outputStream);
void EzLogTokenOrigin(HANDLE token, std::wostream& outputStream);
void EzLogTokenElevationType(HANDLE token, std::wostream& outputStream);
void EzLogTokenLinkedToken(HANDLE token, std::wostream& outputStream);
void EzLogTokenElevation(HANDLE token, std::wostream& outputStream);
void EzLogTokenHasRestrictions(HANDLE token, std::wostream& outputStream);
void EzLogTokenAccessInformation(HANDLE token, std::wostream& outputStream);
void EzLogTokenVirtualizationAllowed(HANDLE token, std::wostream& outputStream);
void EzLogTokenVirtualizationEnabled(HANDLE token, std::wostream& outputStream);
void EzLogTokenIntegrityLevel(HANDLE token, std::wostream& outputStream);
void EzLogTokenUIAccess(HANDLE token, std::wostream& outputStream);
void EzLogTokenMandatoryPolicy(HANDLE token, std::wostream& outputStream);
void EzLogTokenLogonSid(HANDLE token, std::wostream& outputStream);
void EzLogTokenIsAppContainer(HANDLE token, std::wostream& outputStream);
void EzLogTokenCapabilities(HANDLE token, std::wostream& outputStream);
void EzLogTokenAppContainerSid(HANDLE token, std::wostream& outputStream);
void EzLogTokenAppContainerNumber(HANDLE token, std::wostream& outputStream);
void EzLogTokenUserClaimAttributes(HANDLE token, std::wostream& outputStream);
void EzLogTokenDeviceClaimAttributes(HANDLE token, std::wostream& outputStream);
void EzLogTokenRestrictedUserClaimAttributes(HANDLE token, std::wostream& outputStream);
void EzLogTokenRestrictedDeviceClaimAttributes(HANDLE token, std::wostream& outputStream);
void EzLogTokenDeviceGroups(HANDLE token, std::wostream& outputStream);
void EzLogTokenRestrictedDeviceGroups(HANDLE token, std::wostream& outputStream);
void EzLogTokenSecurityAttributes(HANDLE token, std::wostream& outputStream);
void EzLogTokenIsRestricted(HANDLE token, std::wostream& outputStream);
void EzLogTokenProcessTrustLevel(HANDLE token, std::wostream& outputStream);
void EzLogTokenPrivateNameSpace(HANDLE token, std::wostream& outputStream);
void EzLogTokenSingletonAttributes(HANDLE token, std::wostream& outputStream);
void EzLogTokenBnoIsolation(HANDLE token, std::wostream& outputStream);
void EzLogTokenChildProcessFlags(HANDLE token, std::wostream& outputStream);
void EzLogTokenIsLessPrivilegedAppContainer(HANDLE token, std::wostream& outputStream);
void EzLogTokenIsSandboxed(HANDLE token, std::wostream& outputStream);
void EzLogTokenIsAppSilo(HANDLE token, std::wostream& outputStream);
void EzLogTokenLoggingInformation(HANDLE token, std::wostream& outputStream);
void EzLogMaxTokenInfoClass(HANDLE token, std::wostream& outputStream);

void EzLogTokenInfo(HANDLE token, std::wostream& outputStream);

// Working with the current token
HANDLE EzOpenCurrentToken();
HANDLE EzDuplicateCurrentToken();
void EzCloseToken(HANDLE token);

// Impersonating tokens
void EzImpersonate(HANDLE token);
void EzStopImpersonating();
void EzImpersonateWinLogon();
void EzImpersonateLsass();

// Enabling/disabling token privileges
LUID EzLookupPrivilege(LPCWSTR privilege);
void EzEnableAllPrivileges(HANDLE token);
void EzDisableAllPrivileges(HANDLE token);
void EzEnablePrivilege(HANDLE token, LUID privilege);
void EzDisablePrivilege(HANDLE token, LUID privilege);
BOOL EzTokenHasPrivilege(HANDLE token, LUID privilege);

// Starting processes with tokens
LPWSTR EzGetCurrentExePath();
void EzCloseProcessInformation(PROCESS_INFORMATION processInfo);
PROCESS_INFORMATION EzLaunchAsToken(HANDLE token, LPCWSTR exePath);
PROCESS_INFORMATION EzLaunchAsToken(HANDLE token);
PROCESS_INFORMATION EzLaunchAsUser(HANDLE token, LPCWSTR exePath);
PROCESS_INFORMATION EzLaunchAsUser(HANDLE token);
void EzRestartWithToken(HANDLE token);

// Token privilege escalation
void EzGrantUIAccessToToken(HANDLE token);
void EzMakeTokenInteractive(HANDLE token);
void EzGiveTokenSystemIntegrity(HANDLE token);
void EzStealCreateTokenPermission(HANDLE token);
HANDLE EzCreateGodToken();
BOOL EzIsGodToken(HANDLE token);