// GOOD ENOUGH
#include "EzTokens.h"
#include "EzError.h"
#include "EzHelper.h"
#include <sddl.h>
#include <tlhelp32.h>

#pragma comment(lib, "Advapi32.lib")

// Collapse All: Ctrl+M+O
// Toggle Collapse: Ctrl+M+L

// Missing ANSI version of CreateProcessWithToken fix
_Must_inspect_result_ BOOL WINAPI
CreateProcessWithTokenA(
	_In_        HANDLE hToken,
	_In_        DWORD dwLogonFlags,
	_In_opt_    LPCSTR lpApplicationName,
	_Inout_opt_ LPSTR lpCommandLine,
	_In_        DWORD dwCreationFlags,
	_In_opt_    LPVOID lpEnvironment,
	_In_opt_    LPCSTR lpCurrentDirectory,
	_In_        LPSTARTUPINFOA lpStartupInfo,
	_Out_       LPPROCESS_INFORMATION lpProcessInformation
) {


	return CreateProcessWithTokenW(hToken, dwLogonFlags, lpApplicationName, lpCommandLine, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

// Getting info about tokens
static void* GetTokenInfo(HANDLE token, TOKEN_INFORMATION_CLASS desiredInfo) {
	DWORD length = 0;
	GetTokenInformation(token, desiredInfo, NULL, 0, &length);
	if (length == 0) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	void* output = new BYTE[length];
	if (!GetTokenInformation(token, desiredInfo, output, length, &length)) {
		delete[] output;
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return output;
}

SID_AND_ATTRIBUTES EzGetTokenUser(HANDLE token) {
	TOKEN_USER* outputPtr = reinterpret_cast<TOKEN_USER*>(GetTokenInfo(token, TokenUser));
	SID_AND_ATTRIBUTES output = outputPtr->User;
	delete[] outputPtr;
	return output;
}
TOKEN_GROUPS* EzGetTokenGroups(HANDLE token) {
	return reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenGroups));
}
TOKEN_PRIVILEGES* EzGetTokenPrivileges(HANDLE token) {
	return reinterpret_cast<TOKEN_PRIVILEGES*>(GetTokenInfo(token, TokenPrivileges));
}
PSID EzGetTokenOwner(HANDLE token) {
	TOKEN_OWNER* outputPtr = reinterpret_cast<TOKEN_OWNER*>(GetTokenInfo(token, TokenOwner));
	PSID output = outputPtr->Owner;
	delete[] outputPtr;
	return output;
}
PSID EzGetTokenPrimaryGroup(HANDLE token) {
	TOKEN_PRIMARY_GROUP* outputPtr = reinterpret_cast<TOKEN_PRIMARY_GROUP*>(GetTokenInfo(token, TokenPrimaryGroup));
	PSID output = outputPtr->PrimaryGroup;
	delete[] outputPtr;
	return output;
}
TOKEN_DEFAULT_DACL* EzGetTokenDefaultDacl(HANDLE token) {
	return reinterpret_cast<TOKEN_DEFAULT_DACL*>(GetTokenInfo(token, TokenDefaultDacl));
}
TOKEN_SOURCE EzGetTokenSource(HANDLE token) {
	TOKEN_SOURCE* outputPtr = reinterpret_cast<TOKEN_SOURCE*>(GetTokenInfo(token, TokenSource));
	TOKEN_SOURCE output = *outputPtr;
	delete[] outputPtr;
	return output;
}
TOKEN_TYPE EzGetTokenType(HANDLE token) {
	TOKEN_TYPE* outputPtr = reinterpret_cast<TOKEN_TYPE*>(GetTokenInfo(token, TokenType));
	TOKEN_TYPE output = *outputPtr;
	delete[] outputPtr;
	return output;
}
SECURITY_IMPERSONATION_LEVEL EzGetTokenImpersonationLevel(HANDLE token) {
	SECURITY_IMPERSONATION_LEVEL* outputPtr = reinterpret_cast<SECURITY_IMPERSONATION_LEVEL*>(GetTokenInfo(token, TokenImpersonationLevel));
	SECURITY_IMPERSONATION_LEVEL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
TOKEN_STATISTICS EzGetTokenStatistics(HANDLE token) {
	TOKEN_STATISTICS* outputPtr = reinterpret_cast<TOKEN_STATISTICS*>(GetTokenInfo(token, TokenStatistics));
	TOKEN_STATISTICS output = *outputPtr;
	delete[] outputPtr;
	return output;
}
TOKEN_GROUPS* EzGetTokenRestrictedSids(HANDLE token) {
	return reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenRestrictedSids));
}
DWORD EzGetTokenSessionId(HANDLE token) {
	DWORD* outputPtr = reinterpret_cast<DWORD*>(GetTokenInfo(token, TokenSessionId));
	DWORD output = *outputPtr;
	delete[] outputPtr;
	return output;
}
TOKEN_GROUPS_AND_PRIVILEGES EzGetTokenGroupsAndPrivileges(HANDLE token) {
	TOKEN_GROUPS_AND_PRIVILEGES* outputPtr = reinterpret_cast<TOKEN_GROUPS_AND_PRIVILEGES*>(GetTokenInfo(token, TokenGroupsAndPrivileges));
	TOKEN_GROUPS_AND_PRIVILEGES output = *outputPtr;
	delete[] outputPtr;
	return output;
}
BOOL EzGetTokenSandBoxInert(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenSandBoxInert));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
LUID EzGetTokenOrigin(HANDLE token) {
	TOKEN_ORIGIN* outputPtr = reinterpret_cast<TOKEN_ORIGIN*>(GetTokenInfo(token, TokenOrigin));
	LUID output = outputPtr->OriginatingLogonSession;
	delete[] outputPtr;
	return output;
}
TOKEN_ELEVATION_TYPE EzGetTokenElevationType(HANDLE token) {
	TOKEN_ELEVATION_TYPE* outputPtr = reinterpret_cast<TOKEN_ELEVATION_TYPE*>(GetTokenInfo(token, TokenElevationType));
	TOKEN_ELEVATION_TYPE output = *outputPtr;
	delete[] outputPtr;
	return output;
}
HANDLE EzGetTokenLinkedToken(HANDLE token) {
	TOKEN_LINKED_TOKEN* outputPtr = reinterpret_cast<TOKEN_LINKED_TOKEN*>(GetTokenInfo(token, TokenLinkedToken));
	HANDLE output = outputPtr->LinkedToken;
	delete[] outputPtr;
	return output;
}
BOOL EzGetTokenElevation(HANDLE token) {
	TOKEN_ELEVATION* outputPtr = reinterpret_cast<TOKEN_ELEVATION*>(GetTokenInfo(token, TokenElevation));
	BOOL output = outputPtr->TokenIsElevated;
	delete[] outputPtr;
	return output;
}
BOOL EzGetTokenHasRestrictions(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenHasRestrictions));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
TOKEN_ACCESS_INFORMATION EzGetTokenAccessInformation(HANDLE token) {
	TOKEN_ACCESS_INFORMATION* outputPtr = reinterpret_cast<TOKEN_ACCESS_INFORMATION*>(GetTokenInfo(token, TokenAccessInformation));
	TOKEN_ACCESS_INFORMATION output = *outputPtr;
	delete[] outputPtr;
	return output;
}
BOOL EzGetTokenVirtualizationAllowed(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenVirtualizationAllowed));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
BOOL EzGetTokenVirtualizationEnabled(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenVirtualizationEnabled));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
SID_AND_ATTRIBUTES EzGetTokenIntegrityLevel(HANDLE token) {
	TOKEN_MANDATORY_LABEL* outputPtr = reinterpret_cast<TOKEN_MANDATORY_LABEL*>(GetTokenInfo(token, TokenIntegrityLevel));
	SID_AND_ATTRIBUTES output = outputPtr->Label;
	delete[] outputPtr;
	return output;
}
BOOL EzGetTokenUIAccess(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenUIAccess));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
DWORD EzGetTokenMandatoryPolicy(HANDLE token) {
	TOKEN_MANDATORY_POLICY* outputPtr = reinterpret_cast<TOKEN_MANDATORY_POLICY*>(GetTokenInfo(token, TokenMandatoryPolicy));
	DWORD output = outputPtr->Policy;
	delete[] outputPtr;
	return output;
}
TOKEN_GROUPS* EzGetTokenLogonSid(HANDLE token) {
	return reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenLogonSid));
}
BOOL EzGetTokenIsAppContainer(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenIsAppContainer));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
TOKEN_GROUPS* EzGetTokenCapabilities(HANDLE token) {
	return reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenCapabilities));
}
PSID EzGetTokenAppContainerSid(HANDLE token) {
	TOKEN_APPCONTAINER_INFORMATION* outputPtr = reinterpret_cast<TOKEN_APPCONTAINER_INFORMATION*>(GetTokenInfo(token, TokenAppContainerSid));
	PSID output = outputPtr->TokenAppContainer;
	delete[] outputPtr;
	return output;
}
DWORD EzGetTokenAppContainerNumber(HANDLE token) {
	DWORD* outputPtr = reinterpret_cast<DWORD*>(GetTokenInfo(token, TokenAppContainerNumber));
	DWORD output = *outputPtr;
	delete[] outputPtr;
	return output;
}
CLAIM_SECURITY_ATTRIBUTES_INFORMATION* EzGetTokenUserClaimAttributes(HANDLE token) {
	return reinterpret_cast<CLAIM_SECURITY_ATTRIBUTES_INFORMATION*>(GetTokenInfo(token, TokenUserClaimAttributes));
}
CLAIM_SECURITY_ATTRIBUTES_INFORMATION* EzGetTokenDeviceClaimAttributes(HANDLE token) {
	return reinterpret_cast<CLAIM_SECURITY_ATTRIBUTES_INFORMATION*>(GetTokenInfo(token, TokenDeviceClaimAttributes));
}
TOKEN_GROUPS* EzGetTokenDeviceGroups(HANDLE token) {
	return reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenDeviceGroups));
}
TOKEN_GROUPS* EzGetTokenRestrictedDeviceGroups(HANDLE token) {
	return reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenRestrictedDeviceGroups));
}
CLAIM_SECURITY_ATTRIBUTES_INFORMATION* EzGetTokenSecurityAttributes(HANDLE token) {
	return reinterpret_cast<CLAIM_SECURITY_ATTRIBUTES_INFORMATION*>(GetTokenInfo(token, TokenSecurityAttributes));
}
BOOL EzGetTokenIsRestricted(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenIsRestricted));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
PSID EzGetTokenProcessTrustLevel(HANDLE token) {
	PSID* outputPtr = reinterpret_cast<PSID*>(GetTokenInfo(token, TokenProcessTrustLevel));
	PSID output = *outputPtr;
	delete[] outputPtr;
	return output;
}
BOOL EzGetTokenPrivateNameSpace(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenPrivateNameSpace));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
CLAIM_SECURITY_ATTRIBUTES_INFORMATION* EzGetTokenSingletonAttributes(HANDLE token) {
	return reinterpret_cast<CLAIM_SECURITY_ATTRIBUTES_INFORMATION*>(GetTokenInfo(token, TokenSingletonAttributes));
}
TOKEN_BNO_ISOLATION_INFORMATION EzGetTokenBnoIsolation(HANDLE token) {
	TOKEN_BNO_ISOLATION_INFORMATION* outputPtr = reinterpret_cast<TOKEN_BNO_ISOLATION_INFORMATION*>(GetTokenInfo(token, TokenBnoIsolation));
	TOKEN_BNO_ISOLATION_INFORMATION output = *outputPtr;
	delete[] outputPtr;
	return output;
}
BOOL EzGetTokenIsSandboxed(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenIsSandboxed));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}

// Setting info about tokens
static void SetTokenInfo(HANDLE token, TOKEN_INFORMATION_CLASS targetClass, void* newInfo, DWORD newInfoLength) {
	if (!SetTokenInformation(token, targetClass, newInfo, newInfoLength)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

void EzSetTokenUser(HANDLE token, SID_AND_ATTRIBUTES value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(SID_AND_ATTRIBUTES));
}
void EzSetTokenGroups(HANDLE token, TOKEN_GROUPS* value) {
	SetTokenInfo(token, TokenGroups, value, sizeof(TOKEN_GROUPS) + ((value->GroupCount - 1) * sizeof(SID_AND_ATTRIBUTES)));
}
void EzSetTokenPrivileges(HANDLE token, TOKEN_PRIVILEGES* value) {
	SetTokenInfo(token, TokenPrivileges, value, sizeof(TOKEN_PRIVILEGES) + ((value->PrivilegeCount - 1) * sizeof(LUID_AND_ATTRIBUTES)));
}
void EzSetTokenOwner(HANDLE token, PSID value) {
	SetTokenInfo(token, TokenOwner, &value, sizeof(PSID));
}
void EzSetTokenPrimaryGroup(HANDLE token, PSID value) {
	SetTokenInfo(token, TokenPrimaryGroup, &value, sizeof(PSID));
}
void EzSetTokenDefaultDacl(HANDLE token, TOKEN_DEFAULT_DACL* value) {
	SetTokenInfo(token, TokenDefaultDacl, &value, sizeof(TOKEN_DEFAULT_DACL));
}
void EzSetTokenSource(HANDLE token, TOKEN_SOURCE value) {
	SetTokenInfo(token, TokenSource, &value, sizeof(TOKEN_SOURCE));
}
void EzSetTokenType(HANDLE token, TOKEN_TYPE value) {
	SetTokenInfo(token, TokenType, &value, sizeof(TOKEN_TYPE));
}
void EzSetTokenImpersonationLevel(HANDLE token, SECURITY_IMPERSONATION_LEVEL value) {
	SetTokenInfo(token, TokenImpersonationLevel, &value, sizeof(SECURITY_IMPERSONATION_LEVEL));
}
void EzSetTokenStatistics(HANDLE token, TOKEN_STATISTICS value) {
	SetTokenInfo(token, TokenStatistics, &value, sizeof(TOKEN_STATISTICS));
}
void EzSetTokenRestrictedSids(HANDLE token, TOKEN_GROUPS* value) {
	SetTokenInfo(token, TokenRestrictedSids, &value, sizeof(TOKEN_GROUPS) + ((value->GroupCount - 1) * sizeof(SID_AND_ATTRIBUTES)));
}
void EzSetTokenSessionId(HANDLE token, DWORD value) {
	SetTokenInfo(token, TokenSessionId, &value, sizeof(DWORD));
}
void EzSetTokenGroupsAndPrivileges(HANDLE token, TOKEN_GROUPS_AND_PRIVILEGES value) {
	SetTokenInfo(token, TokenGroupsAndPrivileges, &value, sizeof(TOKEN_GROUPS_AND_PRIVILEGES));
}
void EzSetTokenSandBoxInert(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenSandBoxInert, &value, sizeof(BOOL));
}
void EzSetTokenOrigin(HANDLE token, LUID value) {
	SetTokenInfo(token, TokenOrigin, &value, sizeof(LUID));
}
void EzSetTokenElevationType(HANDLE token, TOKEN_ELEVATION_TYPE value) {
	SetTokenInfo(token, TokenElevationType, &value, sizeof(TOKEN_ELEVATION_TYPE));
}
void EzSetTokenLinkedToken(HANDLE token, HANDLE value) {
	SetTokenInfo(token, TokenLinkedToken, &value, sizeof(HANDLE));
}
void EzSetTokenElevation(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenElevation, &value, sizeof(BOOL));
}
void EzSetTokenHasRestrictions(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenHasRestrictions, &value, sizeof(BOOL));
}
void EzSetTokenAccessInformation(HANDLE token, TOKEN_ACCESS_INFORMATION value) {
	SetTokenInfo(token, TokenAccessInformation, &value, sizeof(TOKEN_ACCESS_INFORMATION));
}
void EzSetTokenVirtualizationAllowed(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenVirtualizationAllowed, &value, sizeof(BOOL));
}
void EzSetTokenVirtualizationEnabled(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenVirtualizationEnabled, &value, sizeof(BOOL));
}
void EzSetTokenIntegrityLevel(HANDLE token, SID_AND_ATTRIBUTES value) {
	SetTokenInfo(token, TokenIntegrityLevel, &value, sizeof(SID_AND_ATTRIBUTES));
}
void EzSetTokenUIAccess(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenUIAccess, &value, sizeof(BOOL));
}
void EzSetTokenMandatoryPolicy(HANDLE token, DWORD value) {
	SetTokenInfo(token, TokenMandatoryPolicy, &value, sizeof(DWORD));
}
void EzSetTokenLogonSid(HANDLE token, TOKEN_GROUPS* value) {
	SetTokenInfo(token, TokenLogonSid, value, sizeof(TOKEN_GROUPS) + ((value->GroupCount - 1) * sizeof(SID_AND_ATTRIBUTES)));
}
void EzSetTokenIsAppContainer(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenIsAppContainer, &value, sizeof(BOOL));
}
void EzSetTokenCapabilities(HANDLE token, TOKEN_GROUPS* value) {
	SetTokenInfo(token, TokenCapabilities, value, sizeof(TOKEN_GROUPS) + ((value->GroupCount - 1) * sizeof(SID_AND_ATTRIBUTES)));
}
void EzSetTokenAppContainerSid(HANDLE token, PSID value) {
	SetTokenInfo(token, TokenAppContainerSid, &value, sizeof(PSID));
}
void EzSetTokenAppContainerNumber(HANDLE token, DWORD value) {
	SetTokenInfo(token, TokenAppContainerNumber, &value, sizeof(DWORD));
}
void EzSetTokenUserClaimAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value) {
	SetTokenInfo(token, TokenUserClaimAttributes, value, sizeof(CLAIM_SECURITY_ATTRIBUTES_INFORMATION));
}
void EzSetTokenDeviceClaimAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value) {
	SetTokenInfo(token, TokenDeviceClaimAttributes, value, sizeof(CLAIM_SECURITY_ATTRIBUTES_INFORMATION));
}
void EzSetTokenDeviceGroups(HANDLE token, TOKEN_GROUPS* value) {
	SetTokenInfo(token, TokenDeviceGroups, value, sizeof(TOKEN_GROUPS) + ((value->GroupCount - 1) * sizeof(SID_AND_ATTRIBUTES)));
}
void EzSetTokenRestrictedDeviceGroups(HANDLE token, TOKEN_GROUPS* value) {
	SetTokenInfo(token, TokenRestrictedDeviceGroups, value, sizeof(TOKEN_GROUPS) + ((value->GroupCount - 1) * sizeof(SID_AND_ATTRIBUTES)));
}
void EzSetTokenSecurityAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value) {
	SetTokenInfo(token, TokenSecurityAttributes, value, sizeof(CLAIM_SECURITY_ATTRIBUTES_INFORMATION));
}
void EzSetTokenIsRestricted(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenIsRestricted, &value, sizeof(BOOL));
}
void EzSetTokenProcessTrustLevel(HANDLE token, PSID value) {
	SetTokenInfo(token, TokenProcessTrustLevel, &value, sizeof(PSID));
}
void EzSetTokenPrivateNameSpace(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenPrivateNameSpace, &value, sizeof(BOOL));
}
void EzSetTokenSingletonAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value) {
	SetTokenInfo(token, TokenSingletonAttributes, value, sizeof(CLAIM_SECURITY_ATTRIBUTES_INFORMATION));
}
void EzSetTokenBnoIsolation(HANDLE token, TOKEN_BNO_ISOLATION_INFORMATION value) {
	SetTokenInfo(token, TokenBnoIsolation, &value, sizeof(TOKEN_BNO_ISOLATION_INFORMATION));
}
void EzSetTokenIsSandboxed(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenIsSandboxed, &value, sizeof(BOOL));
}


// Logging info about tokens as text to a specified output stream
void EzLogTokenUser(HANDLE token, std::wostream& outputStream) {
	try {
		SID_AND_ATTRIBUTES tokenUser = EzGetTokenUser(token);

		outputStream << L"Token User:" << std::endl;
		outputStream << L"    SID: "; EzPrintSid(tokenUser.Sid, outputStream); outputStream << std::endl;
		outputStream << L"    Attributes: "; EzPrintBinary(reinterpret_cast<BYTE*>(&tokenUser.Attributes), sizeof(DWORD), outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenGroups(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_GROUPS* tokenGroups = EzGetTokenGroups(token);

		outputStream << L"Token Groups [" << tokenGroups->GroupCount << L"]:"; if (tokenGroups->GroupCount == 0) { outputStream << L" None"; } outputStream << std::endl;
		for (DWORD i = 0; i < tokenGroups->GroupCount; i++)
		{
			if (i != 0) { outputStream << std::endl; }
			outputStream << L"   SID: "; EzPrintSid(tokenGroups->Groups[i].Sid, outputStream); outputStream << std::endl;
			outputStream << L"   Attributes: "; EzPrintBinary(reinterpret_cast<BYTE*>(&tokenGroups->Groups[i].Attributes), sizeof(DWORD), outputStream); outputStream << std::endl;
		}

		delete[] tokenGroups;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenPrivileges(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_PRIVILEGES* tokenPrivileges = EzGetTokenPrivileges(token);

		outputStream << L"Token Privileges [" << tokenPrivileges->PrivilegeCount << L"]:"; if (tokenPrivileges->PrivilegeCount == 0) { outputStream << L" None"; } outputStream << std::endl;
		for (DWORD i = 0; i < tokenPrivileges->PrivilegeCount; i++)
		{
			if (i != 0) { outputStream << std::endl; }
			outputStream << L"    LUID: "; EzPrintLuid(tokenPrivileges->Privileges[i].Luid, outputStream); outputStream << std::endl;
			outputStream << L"    Attributes: "; EzPrintBinary(reinterpret_cast<BYTE*>(&tokenPrivileges->Privileges[i].Attributes), 4, outputStream); outputStream << std::endl;
		}

		delete[] tokenPrivileges;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenOwner(HANDLE token, std::wostream& outputStream) {
	try {
		PSID tokenOwner = EzGetTokenOwner(token);

		outputStream << L"Token Owner: "; EzPrintSid(tokenOwner, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenPrimaryGroup(HANDLE token, std::wostream& outputStream) {
	try {
		PSID tokenPrimaryGroup = EzGetTokenPrimaryGroup(token);

		outputStream << L"Token Primary Group: "; EzPrintSid(tokenPrimaryGroup, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenDefaultDacl(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_DEFAULT_DACL* tokenDefaultDacl = EzGetTokenDefaultDacl(token);

		if (tokenDefaultDacl->DefaultDacl == NULL) {
			outputStream << L"Token Default DACL: NULL" << std::endl;
		}
		else {
			outputStream << L"Token Default DACL:" << std::endl;
			outputStream << L"    ACL Revision: " << tokenDefaultDacl->DefaultDacl->AclRevision << std::endl;
			outputStream << L"    ACL Size: " << tokenDefaultDacl->DefaultDacl->AclSize << std::endl;
			outputStream << L"    ACE List [" << tokenDefaultDacl->DefaultDacl->AceCount << L"]:"; if (tokenDefaultDacl->DefaultDacl->AceCount == 0) { outputStream << L" None"; } outputStream << std::endl;
			for (DWORD i = 0; i < tokenDefaultDacl->DefaultDacl->AceCount; i++)
			{
				if (i != 0) { outputStream << std::endl; }
				ACE_HEADER* currentACE = NULL;
				GetAce(tokenDefaultDacl->DefaultDacl, i, reinterpret_cast<void**>(&currentACE));
				outputStream << L"    Type: " << currentACE->AceType << std::endl;
				outputStream << L"    Flags: " << currentACE->AceFlags << std::endl;
				outputStream << L"    Size: " << currentACE->AceSize << std::endl;
			}
		}

		delete[] tokenDefaultDacl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenSource(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_SOURCE tokenSource = EzGetTokenSource(token);

		CHAR safeSourceName[9];
		memcpy(safeSourceName, tokenSource.SourceName, 8);
		safeSourceName[8] = '\0';

		outputStream << L"Token Source:" << std::endl;
		outputStream << L"    Name: " << safeSourceName << std::endl;
		outputStream << L"    Identifier: "; EzPrintLuid(tokenSource.SourceIdentifier, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenType(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_TYPE tokenType = EzGetTokenType(token);

		outputStream << L"Token Type: ";
		if (tokenType == TokenPrimary) {
			outputStream << L"Primary";
		}
		else if (tokenType == TokenImpersonation) {
			outputStream << L"Impersonation";
		}
		else {
			outputStream << L"Invalid";
		}
		outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenImpersonationLevel(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_TYPE tokenType = EzGetTokenType(token);
		if (tokenType != TokenImpersonation) {
			outputStream << L"Token Impersonation Level: N/A" << std::endl;
			return;
		}

		SECURITY_IMPERSONATION_LEVEL tokenImpersonationLevel = EzGetTokenImpersonationLevel(token);

		outputStream << L"Token Impersonation Level: ";
		if (tokenImpersonationLevel == SecurityAnonymous) {
			outputStream << L"Anonymous";
		}
		else if (tokenImpersonationLevel == SecurityIdentification) {
			outputStream << L"Identification";
		}
		else if (tokenImpersonationLevel == SecurityImpersonation) {
			outputStream << L"Impersonation";
		}
		else if (tokenImpersonationLevel == SecurityDelegation) {
			outputStream << L"Delegation";
		}
		else {
			outputStream << L"Invalid";
		}
		outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenStatistics(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_STATISTICS tokenStatistics = EzGetTokenStatistics(token);

		outputStream << L"Token Statistics:" << std::endl;
		outputStream << L"    Token ID: "; EzPrintLuid(tokenStatistics.TokenId, outputStream); outputStream << std::endl;
		outputStream << L"    Authentication ID: "; EzPrintLuid(tokenStatistics.AuthenticationId, outputStream); outputStream << std::endl;
		outputStream << L"    Token Type: ";
		if (tokenStatistics.TokenType == TokenPrimary) {
			outputStream << L"Primary";
		}
		else if (tokenStatistics.TokenType == TokenImpersonation) {
			outputStream << L"Impersonation";
		}
		else {
			outputStream << L"Invalid";
		}
		outputStream << std::endl;
		if (tokenStatistics.TokenType != TokenImpersonation) {
			outputStream << L"    Impersonation Level: N/A" << std::endl;
		}
		else {
			outputStream << L"    Impersonation Level: ";
			if (tokenStatistics.ImpersonationLevel == SecurityAnonymous) {
				outputStream << L"Anonymous";
			}
			else if (tokenStatistics.ImpersonationLevel == SecurityIdentification) {
				outputStream << L"Identification";
			}
			else if (tokenStatistics.ImpersonationLevel == SecurityImpersonation) {
				outputStream << L"Impersonation";
			}
			else if (tokenStatistics.ImpersonationLevel == SecurityDelegation) {
				outputStream << L"Delegation";
			}
			else {
				outputStream << L"Invalid";
			}
			outputStream << std::endl;
		}
		outputStream << L"    Dynamic Charged: " << tokenStatistics.DynamicCharged << std::endl;
		outputStream << L"    Dynamic Available: " << tokenStatistics.DynamicAvailable << std::endl;
		outputStream << L"    Group Count: " << tokenStatistics.GroupCount << std::endl;
		outputStream << L"    Privilege Count: " << tokenStatistics.PrivilegeCount << std::endl;
		outputStream << L"    Modified ID: "; EzPrintLuid(tokenStatistics.ModifiedId, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenRestrictedSids(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_GROUPS* tokenRestrictedSids = EzGetTokenRestrictedSids(token);

		outputStream << L"Token Restricted SIDs [" << tokenRestrictedSids->GroupCount << L"]:"; if (tokenRestrictedSids->GroupCount == 0) { outputStream << L" None"; } outputStream << std::endl;
		for (DWORD i = 0; i < tokenRestrictedSids->GroupCount; i++)
		{
			if (i != 0) { outputStream << std::endl; }
			outputStream << L"    SID: "; EzPrintSid(tokenRestrictedSids->Groups[i].Sid, outputStream); outputStream << std::endl;
			outputStream << L"    Attributes: "; EzPrintBinary(reinterpret_cast<BYTE*>(&tokenRestrictedSids->Groups[i].Attributes), sizeof(DWORD), outputStream); outputStream << std::endl;
		}

		delete[] tokenRestrictedSids;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenSessionId(HANDLE token, std::wostream& outputStream) {
	try {
		DWORD tokenSessionId = EzGetTokenSessionId(token);

		outputStream << L"Token Session ID: " << tokenSessionId << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenGroupsAndPrivileges(HANDLE token, std::wostream& outputStream) {
	try {
		outputStream << L"Token Groups And Privileges: Information already printed." << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenSessionReference(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Session Refrences: Not supported" << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenSandBoxInert(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenSandBoxInert = EzGetTokenSandBoxInert(token);

		outputStream << L"Token Sand Box Inert: "; EzPrintBool(tokenSandBoxInert, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenAuditPolicy(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns access denied.
		outputStream << L"Token Audit Policy: Only accessible from kernel mode." << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenOrigin(HANDLE token, std::wostream& outputStream) {
	try {
		LUID tokenOrigin = EzGetTokenOrigin(token);

		outputStream << L"Token Origin: "; EzPrintLuid(tokenOrigin, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenElevationType(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_ELEVATION_TYPE tokenElevationType = EzGetTokenElevationType(token);

		outputStream << L"Token Elevation Type: ";
		if (tokenElevationType == TokenElevationTypeDefault) {
			outputStream << L"Default";
		}
		else if (tokenElevationType == TokenElevationTypeFull) {
			outputStream << L"Full";
		}
		else if (tokenElevationType == TokenElevationTypeLimited) {
			outputStream << L"Limited";
		}
		else {
			outputStream << L"Invalid";
		}
		outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenLinkedToken(HANDLE token, std::wostream& outputStream) {
	try {
		HANDLE tokenLinkedToken = EzGetTokenLinkedToken(token);

		outputStream << L"Token Linked Token: "; EzPrintHex(reinterpret_cast<BYTE*>(&tokenLinkedToken), sizeof(HANDLE), outputStream); outputStream << std::endl;

		EzCloseHandleSafely(tokenLinkedToken);
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenElevation(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenElevation = EzGetTokenElevation(token);

		outputStream << L"Token Is Elevated: "; EzPrintBool(tokenElevation, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenHasRestrictions(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenHasRestrictions = EzGetTokenHasRestrictions(token);

		outputStream << L"Token Has Restrictions: "; EzPrintBool(tokenHasRestrictions, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenAccessInformation(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_ACCESS_INFORMATION tokenAccessInformation = EzGetTokenAccessInformation(token);

		outputStream << L"Token Access Information: TODO" << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenVirtualizationAllowed(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenVirtualizationAllowed = EzGetTokenVirtualizationAllowed(token);

		outputStream << L"Token Virtualization Allowed: "; EzPrintBool(tokenVirtualizationAllowed, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenVirtualizationEnabled(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenVirtualizationEnabled = EzGetTokenVirtualizationEnabled(token);

		outputStream << L"Token Virtualization Enabled: "; EzPrintBool(tokenVirtualizationEnabled, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenIntegrityLevel(HANDLE token, std::wostream& outputStream) {
	try {
		SID_AND_ATTRIBUTES tokenIntegrityLevel = EzGetTokenIntegrityLevel(token);

		outputStream << L"Token Integrity Level:" << std::endl;
		outputStream << L"    SID: "; EzPrintSid(tokenIntegrityLevel.Sid, outputStream); outputStream << std::endl;
		outputStream << L"    Attributes: "; EzPrintBinary(reinterpret_cast<BYTE*>(&tokenIntegrityLevel.Attributes), sizeof(DWORD), outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenUIAccess(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenUIAccess = EzGetTokenUIAccess(token);

		outputStream << L"Token UI Access: "; EzPrintBool(tokenUIAccess, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenMandatoryPolicy(HANDLE token, std::wostream& outputStream) {
	try {
		DWORD tokenMandatoryPolicy = EzGetTokenMandatoryPolicy(token);

		outputStream << L"Token Mandatory Policy: " << tokenMandatoryPolicy << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenLogonSid(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_GROUPS* tokenLogonSid = EzGetTokenLogonSid(token);

		outputStream << L"Token Logon Sids [" << tokenLogonSid->GroupCount << L"]:"; if (tokenLogonSid->GroupCount == 0) { outputStream << L" None"; } outputStream << std::endl;
		for (DWORD i = 0; i < tokenLogonSid->GroupCount; i++)
		{
			if (i != 0) { outputStream << std::endl; }
			outputStream << L"    SID: "; EzPrintSid(tokenLogonSid->Groups[i].Sid, outputStream); outputStream << std::endl;
			outputStream << L"    Attributes: "; EzPrintBinary(reinterpret_cast<BYTE*>(&tokenLogonSid->Groups[i].Attributes), sizeof(DWORD), outputStream); outputStream << std::endl;
		}

		delete[] tokenLogonSid;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenIsAppContainer(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenIsAppContainer = EzGetTokenIsAppContainer(token);

		outputStream << L"Token Is App Container: "; EzPrintBool(tokenIsAppContainer, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenCapabilities(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_GROUPS* tokenCapabilities = EzGetTokenCapabilities(token);

		outputStream << L"Token Capabilities [" << tokenCapabilities->GroupCount << L"]:"; if (tokenCapabilities->GroupCount == 0) { outputStream << L" None"; } outputStream << std::endl;
		for (DWORD i = 0; i < tokenCapabilities->GroupCount; i++)
		{
			if (i != 0) { outputStream << std::endl; }
			outputStream << L"    SID: "; EzPrintSid(tokenCapabilities->Groups[i].Sid, outputStream); outputStream << std::endl;
			outputStream << L"    Attributes: "; EzPrintBinary(reinterpret_cast<BYTE*>(&tokenCapabilities->Groups[i].Attributes), sizeof(DWORD), outputStream); outputStream << std::endl;
		}

		delete[] tokenCapabilities;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenAppContainerSid(HANDLE token, std::wostream& outputStream) {
	try {
		PSID tokenAppContainerSid = EzGetTokenAppContainerSid(token);

		outputStream << L"Token App Container SID: "; EzPrintSid(tokenAppContainerSid, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenAppContainerNumber(HANDLE token, std::wostream& outputStream) {
	try {
		DWORD tokenAppContainerNumber = EzGetTokenAppContainerNumber(token);

		outputStream << L"Token App Container Number: " << tokenAppContainerNumber << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenUserClaimAttributes(HANDLE token, std::wostream& outputStream) {
	try {
		CLAIM_SECURITY_ATTRIBUTES_INFORMATION* tokenUserClaimAttributes = EzGetTokenUserClaimAttributes(token);

		outputStream << L"Token User Claim Attributes:" << std::endl;
		outputStream << L"    Version: " << tokenUserClaimAttributes->Version << std::endl;
		outputStream << L"    Reserved: " << tokenUserClaimAttributes->Reserved << std::endl;
		outputStream << L"    Attributes [" << tokenUserClaimAttributes->AttributeCount << L"]:"; if (tokenUserClaimAttributes->AttributeCount == 0) { outputStream << L" None"; } outputStream << std::endl;
		for (DWORD i = 0; i < tokenUserClaimAttributes->AttributeCount; i++)
		{
			CLAIM_SECURITY_ATTRIBUTE_V1 attribute = tokenUserClaimAttributes->Attribute.pAttributeV1[i];
			outputStream << L"        Name: " << attribute.Name << std::endl;
			outputStream << L"        Value Type: " << attribute.ValueType << std::endl;
			outputStream << L"        Reserved: " << attribute.Reserved << std::endl;
			outputStream << L"        Flags: "; EzPrintBinary(reinterpret_cast<BYTE*>(&attribute.Flags), sizeof(DWORD), outputStream); outputStream << std::endl;
			outputStream << L"        Value Count: " << attribute.ValueCount << std::endl;
			outputStream << L"        Values:" << std::endl;
			for (DWORD j = 0; j < attribute.ValueCount; j++)
			{
				outputStream << L"            TODO" << std::endl;
			}
			outputStream << std::endl;
		}

		delete[] tokenUserClaimAttributes;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenDeviceClaimAttributes(HANDLE token, std::wostream& outputStream) {
	try {
		CLAIM_SECURITY_ATTRIBUTES_INFORMATION* tokenDeviceClaimAttributes = EzGetTokenDeviceClaimAttributes(token);

		outputStream << L"Token Device Claim Attributes: TODO" << std::endl;

		delete[] tokenDeviceClaimAttributes;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenRestrictedUserClaimAttributes(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Restricted User Claim Attributes: Not supported" << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenRestrictedDeviceClaimAttributes(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Restricted Device Claim Attributes: Not supported" << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenDeviceGroups(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_GROUPS* tokenDeviceGroups = EzGetTokenDeviceGroups(token);

		outputStream << L"Token Device Groups [" << tokenDeviceGroups->GroupCount << L"]:"; if (tokenDeviceGroups->GroupCount == 0) { outputStream << L" None"; } outputStream << std::endl;
		for (DWORD i = 0; i < tokenDeviceGroups->GroupCount; i++)
		{
			if (i != 0) { outputStream << std::endl; }
			outputStream << L"    SID: "; EzPrintSid(tokenDeviceGroups->Groups[i].Sid, outputStream); outputStream << std::endl;
			outputStream << L"    Attributes: "; EzPrintBinary(reinterpret_cast<BYTE*>(&tokenDeviceGroups->Groups[i].Attributes), sizeof(DWORD), outputStream); outputStream << std::endl;
		}

		delete[] tokenDeviceGroups;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenRestrictedDeviceGroups(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Restricted Device Groups: Not supported" << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenSecurityAttributes(HANDLE token, std::wostream& outputStream) {
	try {
		CLAIM_SECURITY_ATTRIBUTES_INFORMATION* tokenSecurityAttributes = EzGetTokenSecurityAttributes(token);

		outputStream << L"Token Security Attributes: TODO" << std::endl;

		delete[] tokenSecurityAttributes;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenIsRestricted(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenIsRestricted = EzGetTokenIsRestricted(token);

		outputStream << L"Token Is Restricted: "; EzPrintBool(tokenIsRestricted, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenProcessTrustLevel(HANDLE token, std::wostream& outputStream) {
	try {
		PSID tokenProcessTrustLevel = EzGetTokenProcessTrustLevel(token);

		outputStream << L"Token Process Trust Level: "; EzPrintSid(tokenProcessTrustLevel, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenPrivateNameSpace(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenPrivateNameSpace = EzGetTokenPrivateNameSpace(token);

		outputStream << L"Token Private Name Space: "; EzPrintBool(tokenPrivateNameSpace, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenSingletonAttributes(HANDLE token, std::wostream& outputStream) {
	try {
		CLAIM_SECURITY_ATTRIBUTES_INFORMATION* tokenSingletonAttributes = EzGetTokenSingletonAttributes(token);

		outputStream << L"Token Singleton Attributes: TODO" << std::endl;

		delete[] tokenSingletonAttributes;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenBnoIsolation(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_BNO_ISOLATION_INFORMATION tokenBnoIsolation = EzGetTokenBnoIsolation(token);

		outputStream << L"Token BNO Isolation:" << std::endl;
		outputStream << L"    Prefix: "; if (tokenBnoIsolation.IsolationPrefix == NULL) { outputStream << L"NULL"; }
		else { outputStream << tokenBnoIsolation.IsolationPrefix; } outputStream << std::endl;
		outputStream << L"    Enabled: "; EzPrintBool(tokenBnoIsolation.IsolationEnabled, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenChildProcessFlags(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Child Process Flags: Not supported" << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenIsLessPrivilegedAppContainer(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Is Less Privileged App Container: Not supported" << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenIsSandboxed(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenIsSandboxed = EzGetTokenIsSandboxed(token);

		outputStream << L"Token Is Sandboxed: "; EzPrintBool(tokenIsSandboxed, outputStream); outputStream << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenIsAppSilo(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Is App Silo: Not supported" << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogTokenLoggingInformation(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Logging Information: Not supported" << std::endl;
	}
	catch (EzError error) { error.Print(); }
}
void EzLogMaxTokenInfoClass(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Max Token Info Class: Not supported" << std::endl;
	}
	catch (EzError error) { error.Print(); }
}

void EzLogTokenInfo(HANDLE token, std::wostream& outputStream) {
	outputStream << L"Token Handle: "; EzPrintHex(reinterpret_cast<BYTE*>(&token), sizeof(HANDLE), outputStream); outputStream << std::endl << std::endl;

	EzLogTokenUser(token, outputStream); outputStream << std::endl;
	EzLogTokenGroups(token, outputStream); outputStream << std::endl;
	EzLogTokenPrivileges(token, outputStream); outputStream << std::endl;
	EzLogTokenOwner(token, outputStream); outputStream << std::endl;
	EzLogTokenPrimaryGroup(token, outputStream); outputStream << std::endl;
	EzLogTokenDefaultDacl(token, outputStream); outputStream << std::endl;
	EzLogTokenSource(token, outputStream); outputStream << std::endl;
	EzLogTokenType(token, outputStream); outputStream << std::endl;
	EzLogTokenImpersonationLevel(token, outputStream); outputStream << std::endl;
	EzLogTokenStatistics(token, outputStream); outputStream << std::endl;
	EzLogTokenRestrictedSids(token, outputStream); outputStream << std::endl;
	EzLogTokenSessionId(token, outputStream); outputStream << std::endl;
	EzLogTokenGroupsAndPrivileges(token, outputStream); outputStream << std::endl;
	EzLogTokenSessionReference(token, outputStream); outputStream << std::endl;
	EzLogTokenSandBoxInert(token, outputStream); outputStream << std::endl;
	EzLogTokenAuditPolicy(token, outputStream); outputStream << std::endl;
	EzLogTokenOrigin(token, outputStream); outputStream << std::endl;
	EzLogTokenElevationType(token, outputStream); outputStream << std::endl;
	EzLogTokenLinkedToken(token, outputStream); outputStream << std::endl;
	EzLogTokenElevation(token, outputStream); outputStream << std::endl;
	EzLogTokenHasRestrictions(token, outputStream); outputStream << std::endl;
	EzLogTokenAccessInformation(token, outputStream); outputStream << std::endl;
	EzLogTokenVirtualizationAllowed(token, outputStream); outputStream << std::endl;
	EzLogTokenVirtualizationEnabled(token, outputStream); outputStream << std::endl;
	EzLogTokenIntegrityLevel(token, outputStream); outputStream << std::endl;
	EzLogTokenUIAccess(token, outputStream); outputStream << std::endl;
	EzLogTokenMandatoryPolicy(token, outputStream); outputStream << std::endl;
	EzLogTokenLogonSid(token, outputStream); outputStream << std::endl;
	EzLogTokenIsAppContainer(token, outputStream); outputStream << std::endl;
	EzLogTokenCapabilities(token, outputStream); outputStream << std::endl;
	EzLogTokenAppContainerSid(token, outputStream); outputStream << std::endl;
	EzLogTokenAppContainerNumber(token, outputStream); outputStream << std::endl;
	EzLogTokenUserClaimAttributes(token, outputStream); outputStream << std::endl;
	EzLogTokenDeviceClaimAttributes(token, outputStream); outputStream << std::endl;
	EzLogTokenRestrictedUserClaimAttributes(token, outputStream); outputStream << std::endl;
	EzLogTokenRestrictedDeviceClaimAttributes(token, outputStream); outputStream << std::endl;
	EzLogTokenDeviceGroups(token, outputStream); outputStream << std::endl;
	EzLogTokenRestrictedDeviceGroups(token, outputStream); outputStream << std::endl;
	EzLogTokenSecurityAttributes(token, outputStream); outputStream << std::endl;
	EzLogTokenIsRestricted(token, outputStream); outputStream << std::endl;
	EzLogTokenProcessTrustLevel(token, outputStream); outputStream << std::endl;
	EzLogTokenPrivateNameSpace(token, outputStream); outputStream << std::endl;
	EzLogTokenSingletonAttributes(token, outputStream); outputStream << std::endl;
	EzLogTokenBnoIsolation(token, outputStream); outputStream << std::endl;
	EzLogTokenChildProcessFlags(token, outputStream); outputStream << std::endl;
	EzLogTokenIsLessPrivilegedAppContainer(token, outputStream); outputStream << std::endl;
	EzLogTokenIsSandboxed(token, outputStream); outputStream << std::endl;
	EzLogTokenIsAppSilo(token, outputStream); outputStream << std::endl;
	EzLogTokenLoggingInformation(token, outputStream); outputStream << std::endl;
	EzLogMaxTokenInfoClass(token, outputStream); outputStream << std::endl;
}

// Working with the current token
HANDLE EzOpenCurrentToken() {
	HANDLE output;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &output)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	return output;
}
HANDLE EzDuplicateCurrentToken() {
	HANDLE currentToken = EzOpenCurrentToken();

	HANDLE currentTokenCopy;
	if (!DuplicateTokenEx(currentToken, TOKEN_ALL_ACCESS, NULL, SecurityDelegation, TokenPrimary, &currentTokenCopy)) {
		CloseHandle(currentToken);
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	CloseHandle(currentToken);
	
	return currentTokenCopy;
}

// Impersonating tokens
void EzImpersonate(HANDLE token) {
	if (!ImpersonateLoggedOnUser(token)) {
		if (!SetThreadToken(NULL, token)) {
			EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
	}
}
void EzStopImpersonating() {
	if (!SetThreadToken(NULL, NULL)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!RevertToSelf()) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
void EzImpersonateWinLogon() {
	DWORD lastError = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	DWORD winLogonPID = 0;
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(snapshot, &processEntry)) {
		lastError = GetLastError();
		CloseHandle(snapshot);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}
	do {
		if (lstrcmp(processEntry.szExeFile, L"winlogon.exe") == 0) {
			winLogonPID = processEntry.th32ProcessID;
			break;
		}
	} while (Process32Next(snapshot, &processEntry));
	lastError = GetLastError();
	if (lastError != 0 && lastError != ERROR_NO_MORE_FILES) {
		CloseHandle(snapshot);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}
	CloseHandle(snapshot);

	if (winLogonPID == 0) {
		throw EzError(L"WinLogon.exe could not be found in the list of running processes.", __FILE__, __LINE__);
	}

	HANDLE winLogon = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, winLogonPID);
	if (winLogon == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	HANDLE winLogonToken;
	if (!OpenProcessToken(winLogon, TOKEN_QUERY | TOKEN_DUPLICATE, &winLogonToken)) {
		lastError = GetLastError();
		CloseHandle(winLogon);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	try {
		EzImpersonate(winLogonToken);
	}
	catch (...) {
		CloseHandle(winLogonToken);
		CloseHandle(winLogon);
		throw;
	}

	EzCloseHandleSafely(winLogonToken);
	EzCloseHandleSafely(winLogon);
}
void EzImpersonateLsass() {
	DWORD lastError = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	DWORD lsassPID = 0;
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(snapshot, &processEntry)) {
		lastError = GetLastError();
		CloseHandle(snapshot);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}
	do {
		if (lstrcmp(processEntry.szExeFile, L"lsass.exe") == 0) {
			lsassPID = processEntry.th32ProcessID;
			break;
		}
	} while (Process32Next(snapshot, &processEntry));
	lastError = GetLastError();
	if (lastError != 0 && lastError != ERROR_NO_MORE_FILES) {
		CloseHandle(snapshot);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}
	CloseHandle(snapshot);

	if (lsassPID == 0) {
		throw EzError(L"Lsass.exe could not be found in the list of running processes.", __FILE__, __LINE__);
	}

	HANDLE lsass = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, lsassPID);
	if (lsass == NULL) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	HANDLE lsassToken;
	if (!OpenProcessToken(lsass, TOKEN_QUERY | TOKEN_DUPLICATE, &lsassToken)) {
		lastError = GetLastError();
		CloseHandle(lsass);
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	try {
		EzImpersonate(lsassToken);
	}
	catch (...) {
		CloseHandle(lsassToken);
		CloseHandle(lsass);
		throw;
	}

	EzCloseHandleSafely(lsassToken);
	EzCloseHandleSafely(lsass);
}

// Enabling/disabling token privileges
LUID EzLookupPrivilege(LPCWSTR privilege) {
	LUID output = { };
	if (!LookupPrivilegeValue(NULL, privilege, &output))
	{
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	return output;
}
void EzEnableAllPrivileges(HANDLE token) {
	DWORD lastError = 0;

	TOKEN_PRIVILEGES* privileges = EzGetTokenPrivileges(token);

	for (DWORD i = 0; i < privileges->PrivilegeCount; i++) {
		privileges->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
	}

	if (!AdjustTokenPrivileges(token, FALSE, privileges, sizeof(TOKEN_PRIVILEGES) * (sizeof(LUID_AND_ATTRIBUTES) * (privileges->PrivilegeCount - 1)), NULL, NULL))
	{
		delete[] privileges;
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	lastError = GetLastError();
	if (lastError == ERROR_NOT_ALL_ASSIGNED) {
		delete[] privileges;
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}
	delete[] privileges;
}
void EzDisableAllPrivileges(HANDLE token) {
	if (!AdjustTokenPrivileges(token, TRUE, NULL, 0, NULL, NULL))
	{
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
void EzEnablePrivilege(HANDLE token, LUID privilege) {
	DWORD lastError = 0;

	TOKEN_PRIVILEGES tp = { };
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = privilege;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	// Required secondarry check because AdjustTokenPrivileges returns successful if some but not all permissions were adjusted.
	lastError = GetLastError();
	if (lastError == ERROR_NOT_ALL_ASSIGNED) {
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}
}
void EzDisablePrivilege(HANDLE token, LUID privilege) {
	TOKEN_PRIVILEGES tp = { };
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = privilege;
	tp.Privileges[0].Attributes = 0;

	if (!AdjustTokenPrivileges(token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
BOOL EzTokenHasPrivilege(HANDLE token, LUID privilege) {
	TOKEN_PRIVILEGES* tokenPrivileges = EzGetTokenPrivileges(token);

	for (DWORD i = 0; i < tokenPrivileges->PrivilegeCount; i++)
	{
		if (tokenPrivileges->Privileges[i].Luid.LowPart == privilege.LowPart && tokenPrivileges->Privileges[i].Luid.HighPart == privilege.HighPart) {
			delete[] tokenPrivileges;
			return TRUE;
		}
	}

	delete[] tokenPrivileges;
	return FALSE;
}

// Starting processes with tokens
PROCESS_INFORMATION EzLaunchAsToken(HANDLE token, LPCWSTR exePath) {
	STARTUPINFO startupInfo = { };
	PROCESS_INFORMATION processInfo = { };
	if (!CreateProcessWithToken(token, LOGON_WITH_PROFILE, exePath, NULL, 0, NULL, NULL, &startupInfo, &processInfo)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return processInfo;
}
PROCESS_INFORMATION EzLaunchAsToken(HANDLE token) {
	LPWSTR currentExePath = EzGetCurrentExePath();
	PROCESS_INFORMATION processInfo = { };
	try {
		EzLaunchAsToken(token, currentExePath);
	}
	catch (...) {
		delete[] currentExePath;
		throw;
	}
	delete[] currentExePath;

	return processInfo;
}
PROCESS_INFORMATION EzLaunchAsUser(HANDLE token, LPCWSTR exePath) {
	STARTUPINFO startupInfo = { };
	PROCESS_INFORMATION processInfo = { };
	if (!CreateProcessAsUser(token, exePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return processInfo;
}
PROCESS_INFORMATION EzLaunchAsUser(HANDLE token) {
	LPWSTR currentExePath = EzGetCurrentExePath();
	PROCESS_INFORMATION processInfo = { };
	try {
		processInfo = EzLaunchAsUser(token, currentExePath);
	}
	catch (...) {
		delete[] currentExePath;
		throw;
	}
	delete[] currentExePath;

	return processInfo;
}
BOOL EzLaunchWithUAC(LPCWSTR exePath) {
	// NOTE: Returns TRUE if the user selects yes to the UAC else FALSE.
	DWORD lastError = 0;

	SHELLEXECUTEINFO shellExecuteInfo = { };
	shellExecuteInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shellExecuteInfo.fMask = 0;
	shellExecuteInfo.hwnd = 0;
	shellExecuteInfo.lpVerb = L"runas";
	shellExecuteInfo.lpFile = exePath;
	shellExecuteInfo.lpParameters = NULL;
	shellExecuteInfo.lpDirectory = NULL;
	shellExecuteInfo.nShow = SW_SHOWNORMAL;
	shellExecuteInfo.hInstApp = NULL;
	shellExecuteInfo.lpIDList = NULL;
	shellExecuteInfo.lpClass = NULL;
	shellExecuteInfo.hkeyClass = NULL;
	shellExecuteInfo.dwHotKey = 0;
	shellExecuteInfo.hIcon = NULL;
	shellExecuteInfo.hMonitor = NULL;
	shellExecuteInfo.hProcess = NULL;
	if (!ShellExecuteEx(&shellExecuteInfo)) {
		lastError = GetLastError();
		if (lastError == ERROR_CANCELLED) {
			return FALSE;
		}
		else {
			EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
		}
	}
	return TRUE;
}
BOOL EzLaunchWithUAC() {
	LPWSTR currentExePath = EzGetCurrentExePath();
	BOOL uacAccepted = FALSE;
	try {
		uacAccepted = EzLaunchWithUAC(currentExePath);
	}
	catch (...) {
		delete[] currentExePath;
		throw;
	}
	delete[] currentExePath;

	return uacAccepted;
}

// Token privilege escalation
void EzGrantUIAccessToToken(HANDLE token) {
	// Impersonate WinLogon.exe
	EzImpersonateWinLogon();

	// Give UIAccess to the token with the privilages we now have.
	EzSetTokenUIAccess(token, TRUE);

	// Stop impersonating WinLogon.exe's process token.
	EzStopImpersonating();
}
void EzMakeTokenInteractive(HANDLE token) {
	// Impersonate WinLogon.exe
	EzImpersonateWinLogon();

	// Get current session id.
	DWORD activeConsoleSessionId = WTSGetActiveConsoleSessionId();
	if (activeConsoleSessionId == 0xFFFFFFFF) {
		throw EzError(L"Could not create an interactive token because there is no active session currently.", __FILE__, __LINE__);
	}

	// Change the session ID of the token to the current session ID.
	EzSetTokenSessionId(token, activeConsoleSessionId);

	// Stop impersonating WinLogon.exe's process token.
	EzStopImpersonating();
}
void EzGiveTokenSystemIntegrity(HANDLE token) {
	// Lookup the system integrity level SID.
	PSID systemIntegritySid = NULL;
	if (!ConvertStringSidToSid(L"S-1-16-16384", &systemIntegritySid)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Assign system integrity level to the token.
	SID_AND_ATTRIBUTES tokenIntegrityLevel = { };
	tokenIntegrityLevel.Sid = systemIntegritySid;
	tokenIntegrityLevel.Attributes = SE_GROUP_INTEGRITY | SE_GROUP_INTEGRITY_ENABLED | SE_GROUP_MANDATORY;
	EzSetTokenIntegrityLevel(token, tokenIntegrityLevel);

	// Cleanup and return.
	LocalFree(systemIntegritySid);
}
void EzStealCreateTokenPermission(HANDLE token) {
	// Impersonate Lsass.exe
	EzImpersonateLsass();

	// Give UIAccess to the token with the privilages we now have.
	TOKEN_PRIVILEGES tp = {};
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = EzLookupPrivilege(SE_CREATE_TOKEN_NAME);
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED | SE_PRIVILEGE_ENABLED_BY_DEFAULT;
	EzSetTokenPrivileges(token, &tp);

	// Stop impersonating WinLogon.exe's process token.
	EzStopImpersonating();
}

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} UNICODE_STRING, * PUNICODE_STRING;
typedef struct OBJECT_ATTRIBUTES {
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;
typedef NTSYSAPI NTSTATUS(NTAPI* PNtCreateToken)(OUT PHANDLE TokenHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes, IN TOKEN_TYPE TokenType, IN PLUID AuthenticationId, IN PLARGE_INTEGER ExpirationTime, IN PTOKEN_USER TokenUser, IN PTOKEN_GROUPS TokenGroups, IN PTOKEN_PRIVILEGES TokenPrivileges, IN PTOKEN_OWNER TokenOwner, IN PTOKEN_PRIMARY_GROUP TokenPrimaryGroup, IN PTOKEN_DEFAULT_DACL TokenDefaultDacl, IN PTOKEN_SOURCE TokenSource);
HANDLE EzCreateGodToken() {
	/* KNOWN ISSUE
	NtCreateToken only works with pointers to stack memory or pointers to
	heap memory allocated with LocalAlloc or GlobalAlloc. C++ style new[]
	or C style malloc will not work.
	*/
	/* KNOWN ISSUE
	The SE_UNSOLICITED_INPUT_NAME privilege is not supported on Windows 10
	home edition and therefore is not given to the god token.
	*/

	DWORD lastError = 0;
	NTSTATUS nt = 0;

	// Impersonate Lsass to get the SE_CREATE_TOKEN_NAME privilege
	EzImpersonateLsass();

	// Load NtCreateToken function from ntdll.dll
	PNtCreateToken NtCreateToken = reinterpret_cast<PNtCreateToken>(EzGetFunctionAddress("NtCreateToken", L"ntdll.dll"));

	// Get sids for users, groups, and integrity levels we need.
	PSID systemUserSid = NULL;
	if (!ConvertStringSidToSid(L"S-1-5-18", &systemUserSid)) {
		lastError = GetLastError();
		EzStopImpersonating();
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}
	PSID administratorsGroupSid = NULL;
	if (!ConvertStringSidToSid(L"S-1-5-32-544", &administratorsGroupSid)) {
		lastError = GetLastError();
		EzStopImpersonating();
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}
	PSID authenticatedUsersGroupSid = NULL;
	if (!ConvertStringSidToSid(L"S-1-5-11", &authenticatedUsersGroupSid)) {
		lastError = GetLastError();
		EzStopImpersonating();
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}
	PSID everyoneGroupSid = NULL;
	if (!ConvertStringSidToSid(L"S-1-1-0", &everyoneGroupSid)) {
		lastError = GetLastError();
		EzStopImpersonating();
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}
	PSID systemIntegrityLevelSid = NULL;
	if (!ConvertStringSidToSid(L"S-1-16-16384", &systemIntegrityLevelSid)) {
		lastError = GetLastError();
		EzStopImpersonating();
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}
	PSID trustedInstallerUserSid = NULL;
	if (!ConvertStringSidToSid(L"S-1-5-80-956008885-3418522649-1831038044-1853292631-2271478464", &trustedInstallerUserSid)) {
		lastError = GetLastError();
		EzStopImpersonating();
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	ACCESS_MASK desiredAccess = TOKEN_ALL_ACCESS;

	SECURITY_QUALITY_OF_SERVICE securityQualityOfService = { };
	securityQualityOfService.Length = sizeof(SECURITY_QUALITY_OF_SERVICE);
	securityQualityOfService.ImpersonationLevel = SecurityAnonymous;
	securityQualityOfService.ContextTrackingMode = SECURITY_STATIC_TRACKING;
	securityQualityOfService.EffectiveOnly = FALSE;

	OBJECT_ATTRIBUTES objectAttributes = { };
	objectAttributes.Length = sizeof(OBJECT_ATTRIBUTES);
	objectAttributes.RootDirectory = NULL;
	objectAttributes.ObjectName = NULL;
	objectAttributes.Attributes = 0;
	objectAttributes.SecurityDescriptor = NULL;
	objectAttributes.SecurityQualityOfService = &securityQualityOfService;

	TOKEN_TYPE tokenType = TokenPrimary;

	LUID authenticationID = SYSTEM_LUID;

	LARGE_INTEGER expirationTime = { 0xFFFFFFFFFFFFFFFF };

	TOKEN_USER tokenUser = { };
	tokenUser.User.Sid = systemUserSid;
	tokenUser.User.Attributes = 0;

	constexpr DWORD tokenGroupsGroupCount = 5;
	PTOKEN_GROUPS tokenGroups = reinterpret_cast<PTOKEN_GROUPS>(LocalAlloc(LPTR, sizeof(TOKEN_GROUPS) + ((tokenGroupsGroupCount - 1) * sizeof(SID_AND_ATTRIBUTES))));
	tokenGroups->GroupCount = tokenGroupsGroupCount;
	tokenGroups->Groups[0].Sid = administratorsGroupSid;
	tokenGroups->Groups[0].Attributes = SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_MANDATORY | SE_GROUP_OWNER;
	tokenGroups->Groups[1].Sid = authenticatedUsersGroupSid;
	tokenGroups->Groups[1].Attributes = SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_MANDATORY;
	tokenGroups->Groups[2].Sid = everyoneGroupSid;
	tokenGroups->Groups[2].Attributes = SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_MANDATORY;
	tokenGroups->Groups[3].Sid = systemIntegrityLevelSid;
	tokenGroups->Groups[3].Attributes = SE_GROUP_INTEGRITY | SE_GROUP_INTEGRITY_ENABLED | SE_GROUP_MANDATORY;
	tokenGroups->Groups[4].Sid = trustedInstallerUserSid;
	tokenGroups->Groups[4].Attributes = SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_MANDATORY;

	constexpr DWORD tokenPrivilegesPrivilegeCount = 35;
	PTOKEN_PRIVILEGES tokenPrivileges = reinterpret_cast<PTOKEN_PRIVILEGES>(LocalAlloc(LPTR, sizeof(PTOKEN_PRIVILEGES) + ((tokenPrivilegesPrivilegeCount - 1) * sizeof(LUID_AND_ATTRIBUTES))));
	tokenPrivileges->PrivilegeCount = 35;
	for (int i = 0; i < tokenPrivilegesPrivilegeCount; i++)
	{
		tokenPrivileges->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED | SE_PRIVILEGE_ENABLED_BY_DEFAULT;
	}
	tokenPrivileges->Privileges[0].Luid = EzLookupPrivilege(SE_CREATE_TOKEN_NAME);
	tokenPrivileges->Privileges[1].Luid = EzLookupPrivilege(SE_ASSIGNPRIMARYTOKEN_NAME);
	tokenPrivileges->Privileges[2].Luid = EzLookupPrivilege(SE_LOCK_MEMORY_NAME);
	tokenPrivileges->Privileges[3].Luid = EzLookupPrivilege(SE_INCREASE_QUOTA_NAME);
	tokenPrivileges->Privileges[4].Luid = EzLookupPrivilege(SE_MACHINE_ACCOUNT_NAME);
	tokenPrivileges->Privileges[5].Luid = EzLookupPrivilege(SE_TCB_NAME);
	tokenPrivileges->Privileges[6].Luid = EzLookupPrivilege(SE_SECURITY_NAME);
	tokenPrivileges->Privileges[7].Luid = EzLookupPrivilege(SE_TAKE_OWNERSHIP_NAME);
	tokenPrivileges->Privileges[8].Luid = EzLookupPrivilege(SE_LOAD_DRIVER_NAME);
	tokenPrivileges->Privileges[9].Luid = EzLookupPrivilege(SE_SYSTEM_PROFILE_NAME);
	tokenPrivileges->Privileges[10].Luid = EzLookupPrivilege(SE_SYSTEMTIME_NAME);
	tokenPrivileges->Privileges[11].Luid = EzLookupPrivilege(SE_PROF_SINGLE_PROCESS_NAME);
	tokenPrivileges->Privileges[12].Luid = EzLookupPrivilege(SE_INC_BASE_PRIORITY_NAME);
	tokenPrivileges->Privileges[13].Luid = EzLookupPrivilege(SE_CREATE_PAGEFILE_NAME);
	tokenPrivileges->Privileges[14].Luid = EzLookupPrivilege(SE_CREATE_PERMANENT_NAME);
	tokenPrivileges->Privileges[15].Luid = EzLookupPrivilege(SE_BACKUP_NAME);
	tokenPrivileges->Privileges[16].Luid = EzLookupPrivilege(SE_RESTORE_NAME);
	tokenPrivileges->Privileges[17].Luid = EzLookupPrivilege(SE_SHUTDOWN_NAME);
	tokenPrivileges->Privileges[18].Luid = EzLookupPrivilege(SE_DEBUG_NAME);
	tokenPrivileges->Privileges[19].Luid = EzLookupPrivilege(SE_AUDIT_NAME);
	tokenPrivileges->Privileges[20].Luid = EzLookupPrivilege(SE_SYSTEM_ENVIRONMENT_NAME);
	tokenPrivileges->Privileges[21].Luid = EzLookupPrivilege(SE_CHANGE_NOTIFY_NAME);
	tokenPrivileges->Privileges[22].Luid = EzLookupPrivilege(SE_REMOTE_SHUTDOWN_NAME);
	tokenPrivileges->Privileges[23].Luid = EzLookupPrivilege(SE_UNDOCK_NAME);
	tokenPrivileges->Privileges[24].Luid = EzLookupPrivilege(SE_SYNC_AGENT_NAME);
	tokenPrivileges->Privileges[25].Luid = EzLookupPrivilege(SE_ENABLE_DELEGATION_NAME);
	tokenPrivileges->Privileges[26].Luid = EzLookupPrivilege(SE_MANAGE_VOLUME_NAME);
	tokenPrivileges->Privileges[27].Luid = EzLookupPrivilege(SE_IMPERSONATE_NAME);
	tokenPrivileges->Privileges[28].Luid = EzLookupPrivilege(SE_CREATE_GLOBAL_NAME);
	tokenPrivileges->Privileges[29].Luid = EzLookupPrivilege(SE_TRUSTED_CREDMAN_ACCESS_NAME);
	tokenPrivileges->Privileges[30].Luid = EzLookupPrivilege(SE_RELABEL_NAME);
	tokenPrivileges->Privileges[31].Luid = EzLookupPrivilege(SE_INC_WORKING_SET_NAME);
	tokenPrivileges->Privileges[32].Luid = EzLookupPrivilege(SE_TIME_ZONE_NAME);
	tokenPrivileges->Privileges[33].Luid = EzLookupPrivilege(SE_CREATE_SYMBOLIC_LINK_NAME);
	tokenPrivileges->Privileges[34].Luid = EzLookupPrivilege(SE_DELEGATE_SESSION_USER_IMPERSONATE_NAME);

	TOKEN_OWNER tokenOwner = { };
	tokenOwner.Owner = administratorsGroupSid;

	TOKEN_PRIMARY_GROUP tokenPrimaryGroup = { };
	tokenPrimaryGroup.PrimaryGroup = administratorsGroupSid;

	PTOKEN_DEFAULT_DACL tokenDefaultDacl = reinterpret_cast<PTOKEN_DEFAULT_DACL>(LocalAlloc(LPTR, sizeof(PTOKEN_DEFAULT_DACL)));
	tokenDefaultDacl->DefaultDacl = NULL;

	PTOKEN_SOURCE tokenSource = reinterpret_cast<PTOKEN_SOURCE>(LocalAlloc(LPTR, sizeof(TOKEN_SOURCE)));
	tokenSource->SourceIdentifier = { };
	tokenSource->SourceIdentifier.HighPart = 69;
	tokenSource->SourceIdentifier.LowPart = 420;
	memcpy(tokenSource->SourceName, "MYSTERY", TOKEN_SOURCE_LENGTH);

	HANDLE token = 0;
	nt = NtCreateToken(&token, desiredAccess, &objectAttributes, tokenType, &authenticationID, &expirationTime, &tokenUser, tokenGroups, tokenPrivileges, &tokenOwner, &tokenPrimaryGroup, tokenDefaultDacl, tokenSource);
	if (FAILED(nt)) {
		EzError::ThrowFromNT(nt, __FILE__, __LINE__);
	}

	DWORD activeConsoleSessionId = WTSGetActiveConsoleSessionId();
	if (activeConsoleSessionId != 0xFFFFFFFF) {
		EzSetTokenSessionId(token, activeConsoleSessionId);
	}

	EzSetTokenUIAccess(token, TRUE);

	EzSetTokenVirtualizationAllowed(token, TRUE);

	EzSetTokenMandatoryPolicy(token, TOKEN_MANDATORY_POLICY_OFF);

	// Cleanup and return
	EzStopImpersonating();

	LocalFree(systemUserSid);
	LocalFree(administratorsGroupSid);
	LocalFree(authenticatedUsersGroupSid);
	LocalFree(everyoneGroupSid);
	LocalFree(systemIntegrityLevelSid);
	LocalFree(trustedInstallerUserSid);

	LocalFree(tokenGroups);
	LocalFree(tokenPrivileges);
	LocalFree(tokenDefaultDacl);
	LocalFree(tokenSource);

	return token;
}

BOOL EzIsGodToken(HANDLE token) {
	TOKEN_SOURCE tokenSource = EzGetTokenSource(token);
	return tokenSource.SourceIdentifier.HighPart == 69 && tokenSource.SourceIdentifier.LowPart == 420 && lstrcmpA(tokenSource.SourceName, "MYSTERY") == 0;
}