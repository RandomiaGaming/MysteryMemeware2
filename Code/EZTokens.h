#pragma once
#include <Windows.h>
#include <ostream>

namespace EZ {
	// Getting info about tokens
	SID_AND_ATTRIBUTES GetTokenUser(HANDLE token);
	TOKEN_GROUPS* GetTokenGroups(HANDLE token);
	TOKEN_PRIVILEGES* GetTokenPrivileges(HANDLE token);
	PSID GetTokenOwner(HANDLE token);
	PSID GetTokenPrimaryGroup(HANDLE token);
	TOKEN_DEFAULT_DACL* GetTokenDefaultDacl(HANDLE token);
	TOKEN_SOURCE GetTokenSource(HANDLE token);
	TOKEN_TYPE GetTokenType(HANDLE token);
	SECURITY_IMPERSONATION_LEVEL GetTokenImpersonationLevel(HANDLE token);
	TOKEN_STATISTICS GetTokenStatistics(HANDLE token);
	TOKEN_GROUPS* GetTokenRestrictedSids(HANDLE token);
	DWORD GetTokenSessionId(HANDLE token);
	TOKEN_GROUPS_AND_PRIVILEGES GetTokenGroupsAndPrivileges(HANDLE token);
	BOOL GetTokenSandBoxInert(HANDLE token);
	LUID GetTokenOrigin(HANDLE token);
	TOKEN_ELEVATION_TYPE GetTokenElevationType(HANDLE token);
	HANDLE GetTokenLinkedToken(HANDLE token);
	BOOL GetTokenElevation(HANDLE token);
	BOOL GetTokenHasRestrictions(HANDLE token);
	TOKEN_ACCESS_INFORMATION GetTokenAccessInformation(HANDLE token);
	BOOL GetTokenVirtualizationAllowed(HANDLE token);
	BOOL GetTokenVirtualizationEnabled(HANDLE token);
	SID_AND_ATTRIBUTES GetTokenIntegrityLevel(HANDLE token);
	BOOL GetTokenUIAccess(HANDLE token);
	DWORD GetTokenMandatoryPolicy(HANDLE token);
	TOKEN_GROUPS* GetTokenLogonSid(HANDLE token);
	BOOL GetTokenIsAppContainer(HANDLE token);
	TOKEN_GROUPS* GetTokenCapabilities(HANDLE token);
	PSID GetTokenAppContainerSid(HANDLE token);
	DWORD GetTokenAppContainerNumber(HANDLE token);
	CLAIM_SECURITY_ATTRIBUTES_INFORMATION* GetTokenUserClaimAttributes(HANDLE token);
	CLAIM_SECURITY_ATTRIBUTES_INFORMATION* GetTokenDeviceClaimAttributes(HANDLE token);
	TOKEN_GROUPS* GetTokenDeviceGroups(HANDLE token);
	TOKEN_GROUPS* GetTokenRestrictedDeviceGroups(HANDLE token);
	CLAIM_SECURITY_ATTRIBUTES_INFORMATION* GetTokenSecurityAttributes(HANDLE token);
	BOOL GetTokenIsRestricted(HANDLE token);
	PSID GetTokenProcessTrustLevel(HANDLE token);
	BOOL GetTokenPrivateNameSpace(HANDLE token);
	CLAIM_SECURITY_ATTRIBUTES_INFORMATION* GetTokenSingletonAttributes(HANDLE token);
	typedef struct _TOKEN_BNO_ISOLATION_INFORMATION {
		BOOL IsolationEnabled;
		PSID  IsolationPrefixSid;
	} TOKEN_BNO_ISOLATION_INFORMATION, * PTOKEN_BNO_ISOLATION_INFORMATION;
	EZ::TOKEN_BNO_ISOLATION_INFORMATION GetTokenBnoIsolation(HANDLE token);
	BOOL GetTokenIsSandboxed(HANDLE token);

	// Setting info about tokens
	void SetTokenUser(HANDLE token, SID_AND_ATTRIBUTES value);
	void SetTokenGroups(HANDLE token, TOKEN_GROUPS* value);
	void SetTokenPrivileges(HANDLE token, TOKEN_PRIVILEGES* value);
	void SetTokenOwner(HANDLE token, PSID value);
	void SetTokenPrimaryGroup(HANDLE token, PSID value);
	void SetTokenDefaultDacl(HANDLE token, TOKEN_DEFAULT_DACL* value);
	void SetTokenSource(HANDLE token, TOKEN_SOURCE value);
	void SetTokenType(HANDLE token, TOKEN_TYPE value);
	void SetTokenImpersonationLevel(HANDLE token, SECURITY_IMPERSONATION_LEVEL value);
	void SetTokenStatistics(HANDLE token, TOKEN_STATISTICS value);
	void SetTokenRestrictedSids(HANDLE token, TOKEN_GROUPS* value);
	void SetTokenSessionId(HANDLE token, DWORD value);
	void SetTokenGroupsAndPrivileges(HANDLE token, TOKEN_GROUPS_AND_PRIVILEGES value);
	void SetTokenSandBoxInert(HANDLE token, BOOL value);
	void SetTokenOrigin(HANDLE token, LUID value);
	void SetTokenElevationType(HANDLE token, TOKEN_ELEVATION_TYPE value);
	void SetTokenLinkedToken(HANDLE token, HANDLE value);
	void SetTokenElevation(HANDLE token, BOOL value);
	void SetTokenHasRestrictions(HANDLE token, BOOL value);
	void SetTokenAccessInformation(HANDLE token, TOKEN_ACCESS_INFORMATION value);
	void SetTokenVirtualizationAllowed(HANDLE token, BOOL value);
	void SetTokenVirtualizationEnabled(HANDLE token, BOOL value);
	void SetTokenIntegrityLevel(HANDLE token, SID_AND_ATTRIBUTES value);
	void SetTokenUIAccess(HANDLE token, BOOL value);
	void SetTokenMandatoryPolicy(HANDLE token, DWORD value);
	void SetTokenLogonSid(HANDLE token, TOKEN_GROUPS* value);
	void SetTokenIsAppContainer(HANDLE token, BOOL value);
	void SetTokenCapabilities(HANDLE token, TOKEN_GROUPS* value);
	void SetTokenAppContainerSid(HANDLE token, PSID value);
	void SetTokenAppContainerNumber(HANDLE token, DWORD value);
	void SetTokenUserClaimAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value);
	void SetTokenDeviceClaimAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value);
	void SetTokenDeviceGroups(HANDLE token, TOKEN_GROUPS* value);
	void SetTokenRestrictedDeviceGroups(HANDLE token, TOKEN_GROUPS* value);
	void SetTokenSecurityAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value);
	void SetTokenIsRestricted(HANDLE token, BOOL value);
	void SetTokenProcessTrustLevel(HANDLE token, PSID value);
	void SetTokenPrivateNameSpace(HANDLE token, BOOL value);
	void SetTokenSingletonAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value);
	void SetTokenBnoIsolation(HANDLE token, EZ::TOKEN_BNO_ISOLATION_INFORMATION value);
	void SetTokenIsSandboxed(HANDLE token, BOOL value);

	// Logging info about tokens as text to a specified output stream.
	void LogTokenUser(HANDLE token, std::wostream& outputStream);
	void LogTokenGroups(HANDLE token, std::wostream& outputStream);
	void LogTokenPrivileges(HANDLE token, std::wostream& outputStream);
	void LogTokenOwner(HANDLE token, std::wostream& outputStream);
	void LogTokenPrimaryGroup(HANDLE token, std::wostream& outputStream);
	void LogTokenDefaultDacl(HANDLE token, std::wostream& outputStream);
	void LogTokenSource(HANDLE token, std::wostream& outputStream);
	void LogTokenType(HANDLE token, std::wostream& outputStream);
	void LogTokenImpersonationLevel(HANDLE token, std::wostream& outputStream);
	void LogTokenStatistics(HANDLE token, std::wostream& outputStream);
	void LogTokenRestrictedSids(HANDLE token, std::wostream& outputStream);
	void LogTokenSessionId(HANDLE token, std::wostream& outputStream);
	void LogTokenGroupsAndPrivileges(HANDLE token, std::wostream& outputStream);
	void LogTokenSessionReference(HANDLE token, std::wostream& outputStream);
	void LogTokenSandBoxInert(HANDLE token, std::wostream& outputStream);
	void LogTokenAuditPolicy(HANDLE token, std::wostream& outputStream);
	void LogTokenOrigin(HANDLE token, std::wostream& outputStream);
	void LogTokenElevationType(HANDLE token, std::wostream& outputStream);
	void LogTokenLinkedToken(HANDLE token, std::wostream& outputStream);
	void LogTokenElevation(HANDLE token, std::wostream& outputStream);
	void LogTokenHasRestrictions(HANDLE token, std::wostream& outputStream);
	void LogTokenAccessInformation(HANDLE token, std::wostream& outputStream);
	void LogTokenVirtualizationAllowed(HANDLE token, std::wostream& outputStream);
	void LogTokenVirtualizationEnabled(HANDLE token, std::wostream& outputStream);
	void LogTokenIntegrityLevel(HANDLE token, std::wostream& outputStream);
	void LogTokenUIAccess(HANDLE token, std::wostream& outputStream);
	void LogTokenMandatoryPolicy(HANDLE token, std::wostream& outputStream);
	void LogTokenLogonSid(HANDLE token, std::wostream& outputStream);
	void LogTokenIsAppContainer(HANDLE token, std::wostream& outputStream);
	void LogTokenCapabilities(HANDLE token, std::wostream& outputStream);
	void LogTokenAppContainerSid(HANDLE token, std::wostream& outputStream);
	void LogTokenAppContainerNumber(HANDLE token, std::wostream& outputStream);
	void LogTokenUserClaimAttributes(HANDLE token, std::wostream& outputStream);
	void LogTokenDeviceClaimAttributes(HANDLE token, std::wostream& outputStream);
	void LogTokenRestrictedUserClaimAttributes(HANDLE token, std::wostream& outputStream);
	void LogTokenRestrictedDeviceClaimAttributes(HANDLE token, std::wostream& outputStream);
	void LogTokenDeviceGroups(HANDLE token, std::wostream& outputStream);
	void LogTokenRestrictedDeviceGroups(HANDLE token, std::wostream& outputStream);
	void LogTokenSecurityAttributes(HANDLE token, std::wostream& outputStream);
	void LogTokenIsRestricted(HANDLE token, std::wostream& outputStream);
	void LogTokenProcessTrustLevel(HANDLE token, std::wostream& outputStream);
	void LogTokenPrivateNameSpace(HANDLE token, std::wostream& outputStream);
	void LogTokenSingletonAttributes(HANDLE token, std::wostream& outputStream);
	void LogTokenBnoIsolation(HANDLE token, std::wostream& outputStream);
	void LogTokenChildProcessFlags(HANDLE token, std::wostream& outputStream);
	void LogTokenIsLessPrivilegedAppContainer(HANDLE token, std::wostream& outputStream);
	void LogTokenIsSandboxed(HANDLE token, std::wostream& outputStream);
	void LogTokenIsAppSilo(HANDLE token, std::wostream& outputStream);
	void LogTokenLoggingInformation(HANDLE token, std::wostream& outputStream);
	void LogMaxTokenInfoClass(HANDLE token, std::wostream& outputStream);

	void LogTokenInfo(HANDLE token, std::wostream& outputStream);

	// Working with the current token
	HANDLE OpenCurrentToken();
	HANDLE DuplicateCurrentToken();
	void CloseToken(HANDLE token);

	// Impersonating tokens
	void Impersonate(HANDLE token);
	void StopImpersonating();
	void ImpersonateWinLogon();
	void ImpersonateLsass();

	// Enabling/disabling token privileges
	LUID LookupPrivilege(LPCWSTR privilege);
	void EnableAllPrivileges(HANDLE token);
	void DisableAllPrivileges(HANDLE token);
	void EnablePrivilege(HANDLE token, LUID privilege);
	void DisablePrivilege(HANDLE token, LUID privilege);
	BOOL TokenHasPrivilege(HANDLE token, LUID privilege);

	// Starting processes with tokens
	LPWSTR GetCurrentExePath();
	void CloseProcessInformation(PROCESS_INFORMATION processInfo);
	PROCESS_INFORMATION LaunchAsToken(HANDLE token, LPCWSTR exePath);
	PROCESS_INFORMATION LaunchAsToken(HANDLE token);
	PROCESS_INFORMATION LaunchAsUser(HANDLE token, LPCWSTR exePath);
	PROCESS_INFORMATION LaunchAsUser(HANDLE token);
	void RestartWithToken(HANDLE token);

	// Token privilege escalation
	void GrantUIAccessToToken(HANDLE token);
	void MakeTokenInteractive(HANDLE token);
	void GiveTokenSystemIntegrity(HANDLE token);
	void StealCreateTokenPermission(HANDLE token);
	HANDLE CreateGodToken();
}