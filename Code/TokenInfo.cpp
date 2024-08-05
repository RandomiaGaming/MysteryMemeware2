// GOOD ENOUGH
#include "TokenInfo.h"
#include "EZError.h"
#include <fstream>
#include <sddl.h>
#include <iostream>
#include <iomanip>
#pragma comment(lib, "Advapi32.lib")

// Collapse All: Ctrl+M+O
// Toggle Collapse: Ctrl+M+L

void* GetTokenInfo(HANDLE token, TOKEN_INFORMATION_CLASS desiredInfo) {
	DWORD length = 0;
	GetTokenInformation(token, desiredInfo, NULL, 0, &length);
	if (length == 0) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	void* output = malloc(length);
	if (!GetTokenInformation(token, desiredInfo, output, length, &length)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return output;
}

void FormatHex(BYTE* value, DWORD length, std::wostream& output) {
	output << L"0x" << std::hex << std::uppercase << std::setw(2) << std::setfill(L'0');
	for (DWORD i = length - 1; i != 0xFFFFFFFF; i--)
	{
		output << value[i];
	}
	output << std::dec;
}
void FormatBinary(BYTE* value, DWORD length, std::wostream& output) {
	for (DWORD i = 0; i < length; i++)
	{
		if (i != 0) { output << L" "; }
		if ((value[i] & 128) == 0) { output << L"0"; }
		else { output << L"1"; }
		if ((value[i] & 64) == 0) { output << L"0"; }
		else { output << L"1"; }
		if ((value[i] & 32) == 0) { output << L"0"; }
		else { output << L"1"; }
		if ((value[i] & 16) == 0) { output << L"0"; }
		else { output << L"1"; }
		if ((value[i] & 8) == 0) { output << L"0"; }
		else { output << L"1"; }
		if ((value[i] & 4) == 0) { output << L"0"; }
		else { output << L"1"; }
		if ((value[i] & 2) == 0) { output << L"0"; }
		else { output << L"1"; }
		if ((value[i] & 1) == 0) { output << L"0"; }
		else { output << L"1"; }
	}
}
void FormatBool(DWORD value, std::wostream& output) {
	if (value == 0) {
		output << L"False";
	}
	else {
		output << L"True";
	}
}
void FormatSID(PSID value, std::wostream& output) {
	TCHAR name[256];
	TCHAR domain[256];
	DWORD nameLen = sizeof(name) / sizeof(TCHAR);
	DWORD domainLen = sizeof(domain) / sizeof(TCHAR);
	SID_NAME_USE sidType;
	BOOL valid = LookupAccountSid(NULL, value, name, &nameLen, domain, &domainLen, &sidType);

	if (valid) {
		if (domainLen != 0)
		{
			output << domain << L"/";
		}
		output << name;
	}
	else {
		sidType = SidTypeInvalid;
	}

	switch (sidType) {
	case SidTypeUser:
		output << L" (User)";
		break;
	case SidTypeGroup:
		output << L" (Group)";
		break;
	case SidTypeDomain:
		output << L" (Domain)";
		break;
	case SidTypeAlias:
		output << L" (Alias)";
		break;
	case SidTypeWellKnownGroup:
		output << L" (Well Known Group)";
		break;
	case SidTypeDeletedAccount:
		output << L" (Deleted Account)";
		break;
	case SidTypeComputer:
		output << L" (Computer)";
		break;
	case SidTypeLabel:
		output << L" (Label)";
		break;
	case SidTypeLogonSession:
		output << L" (Logon Session)";
		break;
	case SidTypeInvalid:
	case SidTypeUnknown:
	default:
		if (value == 0) { output << L"Null SID"; }
		else { output << L"Invalid/Unknown SID"; }
		break;
	}
}
void FormatLUID(LUID value, std::wostream& output) {
	WCHAR privilegeName[256];
	DWORD nameLen = sizeof(privilegeName) / sizeof(privilegeName[0]);

	if (LookupPrivilegeName(NULL, &value, privilegeName, &nameLen)) {
		output << privilegeName;
	}
	else {
		FormatHex(reinterpret_cast<BYTE*>(&value), 8, output);
	}
}

void PrintTokenHandle(HANDLE token, std::wostream& output) {
	output << L"Token Handle: "; FormatHex(reinterpret_cast<BYTE*>(&token), 8, output); output << std::endl;
}

void PrintTokenUser(HANDLE token, std::wostream& output) {
	try {
		TOKEN_USER* tokenUser = reinterpret_cast<TOKEN_USER*>(GetTokenInfo(token, TokenUser));

		output << L"Token User:" << std::endl;
		output << L"    SID: "; FormatSID(tokenUser->User.Sid, output); output << std::endl;
		output << L"    Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenUser->User.Attributes), 4, output); output << std::endl;

		free(tokenUser);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenGroups(HANDLE token, std::wostream& output) {
	try {
		TOKEN_GROUPS* tokenGroups = reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenGroups));

		output << L"Token Groups [" << tokenGroups->GroupCount << L"]:"; if (tokenGroups->GroupCount == 0) { output << L" None"; } output << std::endl;
		for (DWORD i = 0; i < tokenGroups->GroupCount; i++)
		{
			if (i != 0) { output << std::endl; }
			output << L"   SID: "; FormatSID(tokenGroups->Groups[i].Sid, output); output << std::endl;
			output << L"   Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenGroups->Groups[i].Attributes), 4, output); output << std::endl;
		}

		free(tokenGroups);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenPrivileges(HANDLE token, std::wostream& output) {
	try {
		TOKEN_PRIVILEGES* tokenPrivileges = reinterpret_cast<TOKEN_PRIVILEGES*>(GetTokenInfo(token, TokenPrivileges));

		output << L"Token Privileges [" << tokenPrivileges->PrivilegeCount << L"]:"; if (tokenPrivileges->PrivilegeCount == 0) { output << L" None"; } output << std::endl;
		for (DWORD i = 0; i < tokenPrivileges->PrivilegeCount; i++)
		{
			if (i != 0) { output << std::endl; }
			output << "    LUID: "; FormatLUID(tokenPrivileges->Privileges[i].Luid, output); output << std::endl;
			output << "    Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenPrivileges->Privileges[i].Attributes), 4, output); output << std::endl;
		}

		free(tokenPrivileges);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenOwner(HANDLE token, std::wostream& output) {
	try {
		TOKEN_OWNER* tokenOwner = reinterpret_cast<TOKEN_OWNER*>(GetTokenInfo(token, TokenOwner));

		output << L"Token Owner: "; FormatSID(tokenOwner->Owner, output); output << std::endl;

		free(tokenOwner);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenPrimaryGroup(HANDLE token, std::wostream& output) {
	try {
		TOKEN_PRIMARY_GROUP* tokenPrimaryGroup = reinterpret_cast<TOKEN_PRIMARY_GROUP*>(GetTokenInfo(token, TokenPrimaryGroup));

		output << L"Token Primary Group: "; FormatSID(tokenPrimaryGroup->PrimaryGroup, output); output << std::endl;

		free(tokenPrimaryGroup);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenDefaultDacl(HANDLE token, std::wostream& output) {
	try {
		TOKEN_DEFAULT_DACL* tokenDefaultDacl = reinterpret_cast<TOKEN_DEFAULT_DACL*>(GetTokenInfo(token, TokenDefaultDacl));

		output << L"Token Default DACL:" << std::endl;
		output << L"    ACL Revision: " << tokenDefaultDacl->DefaultDacl->AclRevision << std::endl;
		output << L"    ACL Size: " << tokenDefaultDacl->DefaultDacl->AclSize << std::endl;
		output << L"    ACE List [" << tokenDefaultDacl->DefaultDacl->AceCount << L"]:"; if (tokenDefaultDacl->DefaultDacl->AceCount == 0) { output << L" None"; } output << std::endl;
		for (DWORD i = 0; i < tokenDefaultDacl->DefaultDacl->AceCount; i++)
		{
			if (i != 0) { output << std::endl; }
			ACE_HEADER* currentACE = NULL;
			GetAce(tokenDefaultDacl->DefaultDacl, i, reinterpret_cast<void**>(&currentACE));
			output << L"    Type: " << currentACE->AceType << std::endl;
			output << L"    Flags: " << currentACE->AceFlags << std::endl;
			output << L"    Size: " << currentACE->AceSize << std::endl;
		}

		free(tokenDefaultDacl);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenSource(HANDLE token, std::wostream& output) {
	try {
		TOKEN_SOURCE* tokenSource = reinterpret_cast<TOKEN_SOURCE*>(GetTokenInfo(token, TokenSource));

		output << L"Token Source: " << tokenSource->SourceName << std::endl;

		free(tokenSource);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenType(HANDLE token, std::wostream& output) {
	try {
		TOKEN_TYPE* tokenType = reinterpret_cast<TOKEN_TYPE*>(GetTokenInfo(token, TokenType));

		output << L"Token Type: ";
		if (*tokenType == TokenPrimary) {
			output << L"Primary";
		}
		else if (*tokenType == TokenImpersonation) {
			output << L"Impersonation";
		}
		else {
			output << L"Invalid";
		}
		output << std::endl;

		free(tokenType);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenImpersonationLevel(HANDLE token, std::wostream& output) {
	try {
		TOKEN_TYPE* tokenType = reinterpret_cast<TOKEN_TYPE*>(GetTokenInfo(token, TokenType));
		if (*tokenType != TokenImpersonation) {
			output << L"Token Impersonation Level: N/A" << std::endl;
			free(tokenType);
			return;
		}
		free(tokenType);

		SECURITY_IMPERSONATION_LEVEL* tokenImpersonationLevel = reinterpret_cast<SECURITY_IMPERSONATION_LEVEL*>(GetTokenInfo(token, TokenImpersonationLevel));

		output << L"Token Impersonation Level: ";
		if (*tokenImpersonationLevel == SecurityAnonymous) {
			output << L"Anonymous";
		}
		else if (*tokenImpersonationLevel == SecurityIdentification) {
			output << L"Identification";
		}
		else if (*tokenImpersonationLevel == SecurityImpersonation) {
			output << L"Impersonation";
		}
		else if (*tokenImpersonationLevel == SecurityDelegation) {
			output << L"Delegation";
		}
		else {
			output << L"Invalid";
		}
		output << std::endl;

		free(tokenImpersonationLevel);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenStatistics(HANDLE token, std::wostream& output) {
	try {
		TOKEN_STATISTICS* tokenStatistics = reinterpret_cast<TOKEN_STATISTICS*>(GetTokenInfo(token, TokenStatistics));

		output << L"Token Statistics:" << std::endl;
		output << L"    Token ID: "; FormatLUID(tokenStatistics->TokenId, output); output << std::endl;
		output << L"    Authentication ID: "; FormatLUID(tokenStatistics->AuthenticationId, output); output << std::endl;
		output << L"    Token Type: ";
		if (tokenStatistics->TokenType == TokenPrimary) {
			output << L"Primary";
		}
		else if (tokenStatistics->TokenType == TokenImpersonation) {
			output << L"Impersonation";
		}
		else {
			output << L"Invalid";
		}
		output << std::endl;
		if (tokenStatistics->TokenType != TokenImpersonation) {
			output << L"    Impersonation Level: N/A" << std::endl;
		}
		else {
			output << L"    Impersonation Level: ";
			if (tokenStatistics->ImpersonationLevel == SecurityAnonymous) {
				output << L"Anonymous";
			}
			else if (tokenStatistics->ImpersonationLevel == SecurityIdentification) {
				output << L"Identification";
			}
			else if (tokenStatistics->ImpersonationLevel == SecurityImpersonation) {
				output << L"Impersonation";
			}
			else if (tokenStatistics->ImpersonationLevel == SecurityDelegation) {
				output << L"Delegation";
			}
			else {
				output << L"Invalid";
			}
			output << std::endl;
		}
		output << L"    Dynamic Charged: " << tokenStatistics->DynamicCharged << std::endl;
		output << L"    Dynamic Available: " << tokenStatistics->DynamicAvailable << std::endl;
		output << L"    Group Count: " << tokenStatistics->GroupCount << std::endl;
		output << L"    Privilege Count: " << tokenStatistics->PrivilegeCount << std::endl;
		output << L"    Modified ID: "; FormatLUID(tokenStatistics->ModifiedId, output); output << std::endl;

		free(tokenStatistics);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenRestrictedSids(HANDLE token, std::wostream& output) {
	try {
		TOKEN_GROUPS* tokenRestrictedSids = reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenRestrictedSids));

		output << L"Token Restricted SIDs [" << tokenRestrictedSids->GroupCount << L"]:"; if (tokenRestrictedSids->GroupCount == 0) { output << L" None"; } output << std::endl;
		for (DWORD i = 0; i < tokenRestrictedSids->GroupCount; i++)
		{
			if (i != 0) { output << std::endl; }
			output << L"    SID: "; FormatSID(tokenRestrictedSids->Groups[i].Sid, output); output << std::endl;
			output << L"    Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenRestrictedSids->Groups[i].Attributes), 4, output); output << std::endl;
		}

		free(tokenRestrictedSids);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenSessionID(HANDLE token, std::wostream& output) {
	try {
		DWORD* tokenSessionId = reinterpret_cast<DWORD*>(GetTokenInfo(token, TokenSessionId));

		output << L"Token Session ID: " << *tokenSessionId << std::endl;

		free(tokenSessionId);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenGroupsAndPrivileges(HANDLE token, std::wostream& output) {
	try {
		TOKEN_GROUPS_AND_PRIVILEGES* tokenGroupsAndPrivileges = reinterpret_cast<TOKEN_GROUPS_AND_PRIVILEGES*>(GetTokenInfo(token, TokenGroupsAndPrivileges));

		output << L"Token Groups And Privileges: TODO" << std::endl;

		free(tokenGroupsAndPrivileges);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenSessionReference(HANDLE token, std::wostream& output) {
	try {
		void* tokenSessionReference = reinterpret_cast<void*>(GetTokenInfo(token, TokenSessionReference));

		output << L"Token Session Refrence: TODO" << std::endl;

		free(tokenSessionReference);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenSandBoxInert(HANDLE token, std::wostream& output) {
	try {
		DWORD* tokenSandBoxInert = reinterpret_cast<DWORD*>(GetTokenInfo(token, TokenSandBoxInert));

		output << L"Token Sand Box Inert: "; FormatBool(*tokenSandBoxInert, output); output << std::endl;

		free(tokenSandBoxInert);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenAuditPolicy(HANDLE token, std::wostream& output) {
	try {
		void* tokenAuditPolicy = reinterpret_cast<void*>(GetTokenInfo(token, TokenAuditPolicy));

		output << L"Token Audit Policy: TODO" << std::endl;

		free(tokenAuditPolicy);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenOrigin(HANDLE token, std::wostream& output) {
	try {
		TOKEN_ORIGIN* tokenOrigin = reinterpret_cast<TOKEN_ORIGIN*>(GetTokenInfo(token, TokenOrigin));

		output << L"Token Origin: "; FormatLUID(tokenOrigin->OriginatingLogonSession, output); output << std::endl;

		free(tokenOrigin);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenElevationType(HANDLE token, std::wostream& output) {
	try {
		TOKEN_ELEVATION_TYPE* tokenElevationType = reinterpret_cast<TOKEN_ELEVATION_TYPE*>(GetTokenInfo(token, TokenElevationType));

		output << L"Token Elevation Type: ";
		if (*tokenElevationType == TokenElevationTypeDefault) {
			output << L"Default";
		}
		else if (*tokenElevationType == TokenElevationTypeFull) {
			output << L"Full";
		}
		else if (*tokenElevationType == TokenElevationTypeLimited) {
			output << L"Limited";
		}
		else {
			output << L"Invalid";
		}
		output << std::endl;

		free(tokenElevationType);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenLinkedToken(HANDLE token, std::wostream& output) {
	try {
		TOKEN_LINKED_TOKEN* tokenLinkedToken = reinterpret_cast<TOKEN_LINKED_TOKEN*>(GetTokenInfo(token, TokenLinkedToken));

		output << L"Token Linked Token: "; FormatHex(reinterpret_cast<BYTE*>(&tokenLinkedToken->LinkedToken), 8, output); output << std::endl;

		free(tokenLinkedToken);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenElevation(HANDLE token, std::wostream& output) {
	try {
		TOKEN_ELEVATION* tokenElevation = reinterpret_cast<TOKEN_ELEVATION*>(GetTokenInfo(token, TokenElevation));

		output << L"Token Is Elevated: "; FormatBool(tokenElevation->TokenIsElevated, output); output << std::endl;

		free(tokenElevation);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenHasRestrictions(HANDLE token, std::wostream& output) {
	try {
		DWORD* tokenHasRestrictions = reinterpret_cast<DWORD*>(GetTokenInfo(token, TokenHasRestrictions));

		output << L"Token Has Restrictions: "; FormatBool(*tokenHasRestrictions, output); output << std::endl;

		free(tokenHasRestrictions);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenAccessInformation(HANDLE token, std::wostream& output) {
	try {
		TOKEN_ACCESS_INFORMATION* tokenAccessInformation = reinterpret_cast<TOKEN_ACCESS_INFORMATION*>(GetTokenInfo(token, TokenAccessInformation));

		output << L"Token Access Information: TODO" << std::endl;

		free(tokenAccessInformation);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenVirtualizationAllowed(HANDLE token, std::wostream& output) {
	try {
		DWORD* tokenVirtualizationAllowed = reinterpret_cast<DWORD*>(GetTokenInfo(token, TokenVirtualizationAllowed));

		output << L"Token Virtualization Allowed: "; FormatBool(*tokenVirtualizationAllowed, output); output << std::endl;

		free(tokenVirtualizationAllowed);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenVirtualizationEnabled(HANDLE token, std::wostream& output) {
	try {
		DWORD* tokenVirtualizationEnabled = reinterpret_cast<DWORD*>(GetTokenInfo(token, TokenVirtualizationEnabled));

		output << L"Token Virtualization Enabled: "; FormatBool(*tokenVirtualizationEnabled, output); output << std::endl;

		free(tokenVirtualizationEnabled);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenIntegrityLevel(HANDLE token, std::wostream& output) {
	try {
		TOKEN_MANDATORY_LABEL* tokenIntegrityLevel = reinterpret_cast<TOKEN_MANDATORY_LABEL*>(GetTokenInfo(token, TokenIntegrityLevel));

		output << L"Token Integrity Level:" << std::endl;
		output << "    SID: "; FormatSID(tokenIntegrityLevel->Label.Sid, output); output << std::endl;
		output << "    Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenIntegrityLevel->Label.Attributes), 4, output); output << std::endl;

		free(tokenIntegrityLevel);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenUIAccess(HANDLE token, std::wostream& output) {
	try {
		DWORD* tokenUIAccess = reinterpret_cast<DWORD*>(GetTokenInfo(token, TokenUIAccess));

		output << L"Token UI Access: "; FormatBool(*tokenUIAccess, output); output << std::endl;

		free(tokenUIAccess);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenMandatoryPolicy(HANDLE token, std::wostream& output) {
	try {
		TOKEN_MANDATORY_POLICY* tokenMandatoryPolicy = reinterpret_cast<TOKEN_MANDATORY_POLICY*>(GetTokenInfo(token, TokenMandatoryPolicy));

		output << L"Token Mandatory Policy: " << tokenMandatoryPolicy->Policy << std::endl;

		free(tokenMandatoryPolicy);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenLogonSid(HANDLE token, std::wostream& output) {
	try {
		TOKEN_GROUPS* tokenLogonSid = reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenLogonSid));

		output << L"Token Logon Sids [" << tokenLogonSid->GroupCount << L"]:"; if (tokenLogonSid->GroupCount == 0) { output << L" None"; } output << std::endl;
		for (DWORD i = 0; i < tokenLogonSid->GroupCount; i++)
		{
			if (i != 0) { output << std::endl; }
			output << "    SID: "; FormatSID(tokenLogonSid->Groups[i].Sid, output); output << std::endl;
			output << "    Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenLogonSid->Groups[i].Attributes), 4, output); output << std::endl;
		}

		free(tokenLogonSid);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenIsAppContainer(HANDLE token, std::wostream& output) {
	try {
		DWORD* tokenIsAppContainer = reinterpret_cast<DWORD*>(GetTokenInfo(token, TokenIsAppContainer));

		output << L"Token Is App Container: "; FormatBool(*tokenIsAppContainer, output); output << std::endl;

		free(tokenIsAppContainer);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenCapabilities(HANDLE token, std::wostream& output) {
	try {
		TOKEN_GROUPS* tokenCapabilities = reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenCapabilities));

		output << L"Token Capabilities [" << tokenCapabilities->GroupCount << L"]:"; if (tokenCapabilities->GroupCount == 0) { output << L" None"; } output << std::endl;
		for (DWORD i = 0; i < tokenCapabilities->GroupCount; i++)
		{
			if (i != 0) { output << std::endl; }
			output << "    SID: "; FormatSID(tokenCapabilities->Groups[i].Sid, output); output << std::endl;
			output << "    Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenCapabilities->Groups[i].Attributes), 4, output); output << std::endl;
		}

		free(tokenCapabilities);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenAppContainerSid(HANDLE token, std::wostream& output) {
	try {
		TOKEN_APPCONTAINER_INFORMATION* tokenAppContainerSid = reinterpret_cast<TOKEN_APPCONTAINER_INFORMATION*>(GetTokenInfo(token, TokenAppContainerSid));

		output << L"Token App Container SID: "; FormatSID(tokenAppContainerSid->TokenAppContainer, output); output << std::endl;

		free(tokenAppContainerSid);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenAppContainerNumber(HANDLE token, std::wostream& output) {
	try {
		DWORD* tokenAppContainerNumber = reinterpret_cast<DWORD*>(GetTokenInfo(token, TokenAppContainerNumber));

		output << L"Token App Container Number: " << *tokenAppContainerNumber << std::endl;

		free(tokenAppContainerNumber);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenUserClaimAttributes(HANDLE token, std::wostream& output) {
	try {
		CLAIM_SECURITY_ATTRIBUTES_INFORMATION* tokenUserClaimAttributes = reinterpret_cast<CLAIM_SECURITY_ATTRIBUTES_INFORMATION*>(GetTokenInfo(token, TokenUserClaimAttributes));

		output << L"Token User Claim Attributes:" << std::endl;
		output << L"    Version: " << tokenUserClaimAttributes->Version << std::endl;
		output << L"    Reserved: " << tokenUserClaimAttributes->Reserved << std::endl;
		output << L"    Attributes [" << tokenUserClaimAttributes->AttributeCount << "]:"; if (tokenUserClaimAttributes->AttributeCount == 0) { output << " None"; } output << std::endl;
		for (DWORD i = 0; i < tokenUserClaimAttributes->AttributeCount; i++)
		{
			CLAIM_SECURITY_ATTRIBUTE_V1 attribute = tokenUserClaimAttributes->Attribute.pAttributeV1[i];
			output << L"        Name: " << attribute.Name << std::endl;
			output << L"        Value Type: " << attribute.ValueType << std::endl;
			output << L"        Reserved: " << attribute.Reserved << std::endl;
			output << L"        Flags: "; FormatBinary(reinterpret_cast<BYTE*>(&attribute.Flags), 4, output); output << std::endl;
			output << L"        Value Count: " << attribute.ValueCount << std::endl;
			output << L"        Values:" << std::endl;
			for (DWORD j = 0; j < attribute.ValueCount; j++)
			{
				output << L"            TODO" << std::endl;
			}
			output << std::endl;
		}

		free(tokenUserClaimAttributes);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenDeviceClaimAttributes(HANDLE token, std::wostream& output) {
	try {
		CLAIM_SECURITY_ATTRIBUTES_INFORMATION* tokenDeviceClaimAttributes = reinterpret_cast<CLAIM_SECURITY_ATTRIBUTES_INFORMATION*>(GetTokenInfo(token, TokenDeviceClaimAttributes));

		output << L"Token Device Claim Attributes: TODO" << std::endl;

		free(tokenDeviceClaimAttributes);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenRestrictedUserClaimAttributes(HANDLE token, std::wostream& output) {
	try {
		void* tokenRestrictedUserClaimAttributes = reinterpret_cast<void*>(GetTokenInfo(token, TokenRestrictedUserClaimAttributes));

		output << L"Token Restricted User Claim Attributes: TODO" << std::endl;

		free(tokenRestrictedUserClaimAttributes);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenRestrictedDeviceClaimAttributes(HANDLE token, std::wostream& output) {
	try {
		void* tokenRestrictedDeviceClaimAttributes = reinterpret_cast<void*>(GetTokenInfo(token, TokenRestrictedDeviceClaimAttributes));

		output << L"Token Restricted Device Claim Attributes: TODO" << std::endl;

		free(tokenRestrictedDeviceClaimAttributes);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenDeviceGroups(HANDLE token, std::wostream& output) {
	try {
		TOKEN_GROUPS* tokenDeviceGroups = reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenDeviceGroups));

		output << L"Token Device Groups [" << tokenDeviceGroups->GroupCount << L"]:"; if (tokenDeviceGroups->GroupCount == 0) { output << L" None"; } output << std::endl;
		for (DWORD i = 0; i < tokenDeviceGroups->GroupCount; i++)
		{
			if (i != 0) { output << std::endl; }
			output << "    SID: "; FormatSID(tokenDeviceGroups->Groups[i].Sid, output); output << std::endl;
			output << "    Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenDeviceGroups->Groups[i].Attributes), 4, output); output << std::endl;
		}

		free(tokenDeviceGroups);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenRestrictedDeviceGroups(HANDLE token, std::wostream& output) {
	try {
		output << L"Token Restricted Device Groups: TODO Seems to be broken at a system level." << std::endl;
		return;

		TOKEN_GROUPS* tokenRestrictedDeviceGroups = reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenRestrictedDeviceGroups));

		output << L"Token Restricted Device Groups (L" << tokenRestrictedDeviceGroups->GroupCount << L"):" << std::endl;

		for (DWORD i = 0; i < tokenRestrictedDeviceGroups->GroupCount; i++)
		{
			if (i != 0) {
				output << std::endl;
			}

			output << "    SID: ";
			FormatSID(tokenRestrictedDeviceGroups->Groups[i].Sid, output);
			output << std::endl;

			output << "    Attributes: ";
			FormatBinary(reinterpret_cast<BYTE*>(&tokenRestrictedDeviceGroups->Groups[i].Attributes), 4, output);
			output << std::endl;
		}

		free(tokenRestrictedDeviceGroups);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenSecurityAttributes(HANDLE token, std::wostream& output) {
	try {
		void* tokenSecurityAttributes = reinterpret_cast<void*>(GetTokenInfo(token, TokenSecurityAttributes));

		output << L"Token Security Attributes: TODO" << std::endl;

		free(tokenSecurityAttributes);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenIsRestricted(HANDLE token, std::wostream& output) {
	try {
		void* tokenIsRestricted = reinterpret_cast<void*>(GetTokenInfo(token, TokenIsRestricted));

		output << L"Token Is Restricted: TODO" << std::endl;

		free(tokenIsRestricted);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenProcessTrustLevel(HANDLE token, std::wostream& output) {
	try {
		void* tokenProcessTrustLevel = reinterpret_cast<void*>(GetTokenInfo(token, TokenProcessTrustLevel));

		output << L"Token Process Trust Level: TODO" << std::endl;

		free(tokenProcessTrustLevel);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenPrivateNameSpace(HANDLE token, std::wostream& output) {
	try {
		void* tokenPrivateNameSpace = reinterpret_cast<void*>(GetTokenInfo(token, TokenPrivateNameSpace));

		output << L"Token Private Name Space: TODO" << std::endl;

		free(tokenPrivateNameSpace);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenSingletonAttributes(HANDLE token, std::wostream& output) {
	try {
		void* tokenSingletonAttributes = reinterpret_cast<void*>(GetTokenInfo(token, TokenSingletonAttributes));

		output << L"Token Singleton Attributes: TODO" << std::endl;

		free(tokenSingletonAttributes);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenBnoIsolation(HANDLE token, std::wostream& output) {
	try {
		void* tokenBnoIsolation = reinterpret_cast<void*>(GetTokenInfo(token, TokenBnoIsolation));

		output << L"Token BNO Isolation: TODO" << std::endl;

		free(tokenBnoIsolation);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenChildProcessFlags(HANDLE token, std::wostream& output) {
	try {
		void* tokenChildProcessFlags = reinterpret_cast<void*>(GetTokenInfo(token, TokenChildProcessFlags));

		output << L"Token Child Process Flags: TODO" << std::endl;

		free(tokenChildProcessFlags);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenIsLessPrivilegedAppContainer(HANDLE token, std::wostream& output) {
	try {
		void* tokenIsLessPrivilegedAppContainer = reinterpret_cast<void*>(GetTokenInfo(token, TokenIsLessPrivilegedAppContainer));

		output << L"Token Is Less Privileged App Container: TODO" << std::endl;

		free(tokenIsLessPrivilegedAppContainer);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenIsSandboxed(HANDLE token, std::wostream& output) {
	try {
		void* tokenIsSandboxed = reinterpret_cast<void*>(GetTokenInfo(token, TokenIsSandboxed));

		output << L"Token Is Sandboxed: TODO" << std::endl;

		free(tokenIsSandboxed);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenIsAppSilo(HANDLE token, std::wostream& output) {
	try {
		void* tokenIsAppSilo = reinterpret_cast<void*>(GetTokenInfo(token, TokenIsAppSilo));

		output << L"Token Is App Silo: TODO" << std::endl;

		free(tokenIsAppSilo);
	}
	catch (EZ::Error error) { error.Print(); }
}
void PrintTokenLoggingInformation(HANDLE token, std::wostream& output) {
	output << L"Token Logging Information: TODO (Not supported by SDK)" << std::endl;
	/*
	try {
		void* tokenLoggingInformation = reinterpret_cast<void*>(GetTokenInfo(token, TokenLoggingInformation));

		output << L"Token Logging Information: TODO" << std::endl;

		free(tokenLoggingInformation);
	}
	catch (EZ::Error error) { error.Print();  }
	*/

}
void PrintMaxTokenInfoClass(HANDLE token, std::wostream& output) {
	try {
		void* maxTokenInfoClass = reinterpret_cast<void*>(GetTokenInfo(token, MaxTokenInfoClass));

		output << L"Max Token Info Class: TODO" << std::endl;

		free(maxTokenInfoClass);
	}
	catch (EZ::Error error) { error.Print(); }
}

void PrintTokenInfo(HANDLE token, std::wostream& output) {
	PrintTokenHandle(token, output); output << std::endl;

	PrintTokenUser(token, output); output << std::endl;
	PrintTokenGroups(token, output); output << std::endl;
	PrintTokenPrivileges(token, output); output << std::endl;
	PrintTokenOwner(token, output); output << std::endl;
	PrintTokenPrimaryGroup(token, output); output << std::endl;
	PrintTokenDefaultDacl(token, output); output << std::endl;
	PrintTokenSource(token, output); output << std::endl;
	PrintTokenType(token, output); output << std::endl;
	PrintTokenImpersonationLevel(token, output); output << std::endl;
	PrintTokenStatistics(token, output); output << std::endl;
	PrintTokenRestrictedSids(token, output); output << std::endl;
	PrintTokenSessionID(token, output); output << std::endl;
	PrintTokenGroupsAndPrivileges(token, output); output << std::endl;
	PrintTokenSessionReference(token, output); output << std::endl;
	PrintTokenSandBoxInert(token, output); output << std::endl;
	PrintTokenAuditPolicy(token, output); output << std::endl;
	PrintTokenOrigin(token, output); output << std::endl;
	PrintTokenElevationType(token, output); output << std::endl;
	PrintTokenLinkedToken(token, output); output << std::endl;
	PrintTokenElevation(token, output); output << std::endl;
	PrintTokenHasRestrictions(token, output); output << std::endl;
	PrintTokenAccessInformation(token, output); output << std::endl;
	PrintTokenVirtualizationAllowed(token, output); output << std::endl;
	PrintTokenVirtualizationEnabled(token, output); output << std::endl;
	PrintTokenIntegrityLevel(token, output); output << std::endl;
	PrintTokenUIAccess(token, output); output << std::endl;
	PrintTokenMandatoryPolicy(token, output); output << std::endl;
	PrintTokenLogonSid(token, output); output << std::endl;
	PrintTokenIsAppContainer(token, output); output << std::endl;
	PrintTokenCapabilities(token, output); output << std::endl;
	PrintTokenAppContainerSid(token, output); output << std::endl;
	PrintTokenAppContainerNumber(token, output); output << std::endl;
	PrintTokenUserClaimAttributes(token, output); output << std::endl;
	PrintTokenDeviceClaimAttributes(token, output); output << std::endl;
	PrintTokenRestrictedUserClaimAttributes(token, output); output << std::endl;
	PrintTokenRestrictedDeviceClaimAttributes(token, output); output << std::endl;
	PrintTokenDeviceGroups(token, output); output << std::endl;
	PrintTokenRestrictedDeviceGroups(token, output); output << std::endl;
	PrintTokenSecurityAttributes(token, output); output << std::endl;
	PrintTokenIsRestricted(token, output); output << std::endl;
	PrintTokenProcessTrustLevel(token, output); output << std::endl;
	PrintTokenPrivateNameSpace(token, output); output << std::endl;
	PrintTokenSingletonAttributes(token, output); output << std::endl;
	PrintTokenBnoIsolation(token, output); output << std::endl;
	PrintTokenChildProcessFlags(token, output); output << std::endl;
	PrintTokenIsLessPrivilegedAppContainer(token, output); output << std::endl;
	PrintTokenIsSandboxed(token, output); output << std::endl;
	PrintTokenIsAppSilo(token, output); output << std::endl;
	PrintTokenLoggingInformation(token, output); output << std::endl;
	PrintMaxTokenInfoClass(token, output); output << std::endl;
}

void SaveTokenInfo() {
	std::wofstream tokenOutputFile(L"C:\\ProgramData\\MysteryToken.txt");

	HANDLE token = GetCurrentProcessToken();

	PrintTokenInfo(token, tokenOutputFile);

	CloseHandle(token);
}