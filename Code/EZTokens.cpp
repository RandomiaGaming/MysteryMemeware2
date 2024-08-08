// GOOD ENOUGH
#include "EZTokens.h"
#include "EZError.h"
#include <ostream>
#include <sddl.h>
#include <tlhelp32.h>
#include <iomanip>
#pragma comment(lib, "Advapi32.lib")

// Collapse All: Ctrl+M+O
// Toggle Collapse: Ctrl+M+L

// Getting info about tokens
void* GetTokenInfo(HANDLE token, TOKEN_INFORMATION_CLASS desiredInfo) {
	DWORD length = 0;
	GetTokenInformation(token, desiredInfo, NULL, 0, &length);
	if (length == 0) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	void* output = new BYTE[length];
	if (!GetTokenInformation(token, desiredInfo, output, length, &length)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return output;
}

SID_AND_ATTRIBUTES EZ::GetTokenUser(HANDLE token) {
	TOKEN_USER* outputPtr = reinterpret_cast<TOKEN_USER*>(GetTokenInfo(token, TokenUser));
	SID_AND_ATTRIBUTES output = outputPtr->User;
	delete[] outputPtr;
	return output;
}
TOKEN_GROUPS* EZ::GetTokenGroups(HANDLE token) {
	return reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenGroups));
}
TOKEN_PRIVILEGES* EZ::GetTokenPrivileges(HANDLE token) {
	return reinterpret_cast<TOKEN_PRIVILEGES*>(GetTokenInfo(token, TokenPrivileges));
}
PSID EZ::GetTokenOwner(HANDLE token) {
	TOKEN_OWNER* outputPtr = reinterpret_cast<TOKEN_OWNER*>(GetTokenInfo(token, TokenOwner));
	PSID output = outputPtr->Owner;
	delete[] outputPtr;
	return output;
}
PSID EZ::GetTokenPrimaryGroup(HANDLE token) {
	TOKEN_PRIMARY_GROUP* outputPtr = reinterpret_cast<TOKEN_PRIMARY_GROUP*>(GetTokenInfo(token, TokenPrimaryGroup));
	PSID output = outputPtr->PrimaryGroup;
	delete[] outputPtr;
	return output;
}
TOKEN_DEFAULT_DACL* EZ::GetTokenDefaultDacl(HANDLE token) {
	return reinterpret_cast<TOKEN_DEFAULT_DACL*>(GetTokenInfo(token, TokenDefaultDacl));
}
TOKEN_SOURCE EZ::GetTokenSource(HANDLE token) {
	TOKEN_SOURCE* outputPtr = reinterpret_cast<TOKEN_SOURCE*>(GetTokenInfo(token, TokenSource));
	TOKEN_SOURCE output = *outputPtr;
	delete[] outputPtr;
	return output;
}
TOKEN_TYPE EZ::GetTokenType(HANDLE token) {
	TOKEN_TYPE* outputPtr = reinterpret_cast<TOKEN_TYPE*>(GetTokenInfo(token, TokenType));
	TOKEN_TYPE output = *outputPtr;
	delete[] outputPtr;
	return output;
}
SECURITY_IMPERSONATION_LEVEL EZ::GetTokenImpersonationLevel(HANDLE token) {
	SECURITY_IMPERSONATION_LEVEL* outputPtr = reinterpret_cast<SECURITY_IMPERSONATION_LEVEL*>(GetTokenInfo(token, TokenImpersonationLevel));
	SECURITY_IMPERSONATION_LEVEL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
TOKEN_STATISTICS EZ::GetTokenStatistics(HANDLE token) {
	TOKEN_STATISTICS* outputPtr = reinterpret_cast<TOKEN_STATISTICS*>(GetTokenInfo(token, TokenStatistics));
	TOKEN_STATISTICS output = *outputPtr;
	delete[] outputPtr;
	return output;
}
TOKEN_GROUPS* EZ::GetTokenRestrictedSids(HANDLE token) {
	return reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenRestrictedSids));
}
DWORD EZ::GetTokenSessionId(HANDLE token) {
	DWORD* outputPtr = reinterpret_cast<DWORD*>(GetTokenInfo(token, TokenSessionId));
	DWORD output = *outputPtr;
	delete[] outputPtr;
	return output;
}
TOKEN_GROUPS_AND_PRIVILEGES EZ::GetTokenGroupsAndPrivileges(HANDLE token) {
	TOKEN_GROUPS_AND_PRIVILEGES* outputPtr = reinterpret_cast<TOKEN_GROUPS_AND_PRIVILEGES*>(GetTokenInfo(token, TokenGroupsAndPrivileges));
	TOKEN_GROUPS_AND_PRIVILEGES output = *outputPtr;
	delete[] outputPtr;
	return output;
}
BOOL EZ::GetTokenSandBoxInert(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenSandBoxInert));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
LUID EZ::GetTokenOrigin(HANDLE token) {
	TOKEN_ORIGIN* outputPtr = reinterpret_cast<TOKEN_ORIGIN*>(GetTokenInfo(token, TokenOrigin));
	LUID output = outputPtr->OriginatingLogonSession;
	delete[] outputPtr;
	return output;
}
TOKEN_ELEVATION_TYPE EZ::GetTokenElevationType(HANDLE token) {
	TOKEN_ELEVATION_TYPE* outputPtr = reinterpret_cast<TOKEN_ELEVATION_TYPE*>(GetTokenInfo(token, TokenElevationType));
	TOKEN_ELEVATION_TYPE output = *outputPtr;
	delete[] outputPtr;
	return output;
}
HANDLE EZ::GetTokenLinkedToken(HANDLE token) {
	TOKEN_LINKED_TOKEN* outputPtr = reinterpret_cast<TOKEN_LINKED_TOKEN*>(GetTokenInfo(token, TokenLinkedToken));
	HANDLE output = outputPtr->LinkedToken;
	delete[] outputPtr;
	return output;
}
BOOL EZ::GetTokenElevation(HANDLE token) {
	TOKEN_ELEVATION* outputPtr = reinterpret_cast<TOKEN_ELEVATION*>(GetTokenInfo(token, TokenElevation));
	BOOL output = outputPtr->TokenIsElevated;
	delete[] outputPtr;
	return output;
}
BOOL EZ::GetTokenHasRestrictions(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenHasRestrictions));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
TOKEN_ACCESS_INFORMATION EZ::GetTokenAccessInformation(HANDLE token) {
	TOKEN_ACCESS_INFORMATION* outputPtr = reinterpret_cast<TOKEN_ACCESS_INFORMATION*>(GetTokenInfo(token, TokenAccessInformation));
	TOKEN_ACCESS_INFORMATION output = *outputPtr;
	delete[] outputPtr;
	return output;
}
BOOL EZ::GetTokenVirtualizationAllowed(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenVirtualizationAllowed));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
BOOL EZ::GetTokenVirtualizationEnabled(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenVirtualizationEnabled));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
SID_AND_ATTRIBUTES EZ::GetTokenIntegrityLevel(HANDLE token) {
	TOKEN_MANDATORY_LABEL* outputPtr = reinterpret_cast<TOKEN_MANDATORY_LABEL*>(GetTokenInfo(token, TokenIntegrityLevel));
	SID_AND_ATTRIBUTES output = outputPtr->Label;
	delete[] outputPtr;
	return output;
}
BOOL EZ::GetTokenUIAccess(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenUIAccess));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
DWORD EZ::GetTokenMandatoryPolicy(HANDLE token) {
	TOKEN_MANDATORY_POLICY* outputPtr = reinterpret_cast<TOKEN_MANDATORY_POLICY*>(GetTokenInfo(token, TokenMandatoryPolicy));
	DWORD output = outputPtr->Policy;
	delete[] outputPtr;
	return output;
}
TOKEN_GROUPS* EZ::GetTokenLogonSid(HANDLE token) {
	return reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenLogonSid));
}
BOOL EZ::GetTokenIsAppContainer(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenIsAppContainer));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
TOKEN_GROUPS* EZ::GetTokenCapabilities(HANDLE token) {
	return reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenCapabilities));
}
PSID EZ::GetTokenAppContainerSid(HANDLE token) {
	TOKEN_APPCONTAINER_INFORMATION* outputPtr = reinterpret_cast<TOKEN_APPCONTAINER_INFORMATION*>(GetTokenInfo(token, TokenAppContainerSid));
	PSID output = outputPtr->TokenAppContainer;
	delete[] outputPtr;
	return output;
}
DWORD EZ::GetTokenAppContainerNumber(HANDLE token) {
	DWORD* outputPtr = reinterpret_cast<DWORD*>(GetTokenInfo(token, TokenAppContainerNumber));
	DWORD output = *outputPtr;
	delete[] outputPtr;
	return output;
}
CLAIM_SECURITY_ATTRIBUTES_INFORMATION* EZ::GetTokenUserClaimAttributes(HANDLE token) {
	return reinterpret_cast<CLAIM_SECURITY_ATTRIBUTES_INFORMATION*>(GetTokenInfo(token, TokenUserClaimAttributes));
}
CLAIM_SECURITY_ATTRIBUTES_INFORMATION* EZ::GetTokenDeviceClaimAttributes(HANDLE token) {
	return reinterpret_cast<CLAIM_SECURITY_ATTRIBUTES_INFORMATION*>(GetTokenInfo(token, TokenDeviceClaimAttributes));
}
TOKEN_GROUPS* EZ::GetTokenDeviceGroups(HANDLE token) {
	return reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenDeviceGroups));
}
TOKEN_GROUPS* EZ::GetTokenRestrictedDeviceGroups(HANDLE token) {
	return reinterpret_cast<TOKEN_GROUPS*>(GetTokenInfo(token, TokenRestrictedDeviceGroups));
}
CLAIM_SECURITY_ATTRIBUTES_INFORMATION* EZ::GetTokenSecurityAttributes(HANDLE token) {
	return reinterpret_cast<CLAIM_SECURITY_ATTRIBUTES_INFORMATION*>(GetTokenInfo(token, TokenSecurityAttributes));
}
BOOL EZ::GetTokenIsRestricted(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenIsRestricted));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
PSID EZ::GetTokenProcessTrustLevel(HANDLE token) {
	PSID* outputPtr = reinterpret_cast<PSID*>(GetTokenInfo(token, TokenProcessTrustLevel));
	PSID output = *outputPtr;
	delete[] outputPtr;
	return output;
}
BOOL EZ::GetTokenPrivateNameSpace(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenPrivateNameSpace));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}
CLAIM_SECURITY_ATTRIBUTES_INFORMATION* EZ::GetTokenSingletonAttributes(HANDLE token) {
	return reinterpret_cast<CLAIM_SECURITY_ATTRIBUTES_INFORMATION*>(GetTokenInfo(token, TokenSingletonAttributes));
}
EZ::TOKEN_BNO_ISOLATION_INFORMATION EZ::GetTokenBnoIsolation(HANDLE token) {
	TOKEN_BNO_ISOLATION_INFORMATION* outputPtr = reinterpret_cast<TOKEN_BNO_ISOLATION_INFORMATION*>(GetTokenInfo(token, TokenBnoIsolation));
	TOKEN_BNO_ISOLATION_INFORMATION output = *outputPtr;
	delete[] outputPtr;
	return output;
}
BOOL EZ::GetTokenIsSandboxed(HANDLE token) {
	BOOL* outputPtr = reinterpret_cast<BOOL*>(GetTokenInfo(token, TokenIsSandboxed));
	BOOL output = *outputPtr;
	delete[] outputPtr;
	return output;
}

// Setting info about tokens
void SetTokenInfo(HANDLE token, TOKEN_INFORMATION_CLASS targetClass, void* newInfo, DWORD newInfoLength) {
	if (!SetTokenInformation(token, targetClass, newInfo, newInfoLength)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

void EZ::SetTokenUser(HANDLE token, SID_AND_ATTRIBUTES value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(SID_AND_ATTRIBUTES));
}
void EZ::SetTokenGroups(HANDLE token, TOKEN_GROUPS* value) {
	SetTokenInfo(token, TokenUser, value, sizeof(TOKEN_GROUPS) + ((value->GroupCount - 1) * sizeof(SID_AND_ATTRIBUTES)));
}
void EZ::SetTokenPrivileges(HANDLE token, TOKEN_PRIVILEGES* value) {
	SetTokenInfo(token, TokenUser, value, sizeof(TOKEN_PRIVILEGES) + ((value->PrivilegeCount - 1) * sizeof(LUID_AND_ATTRIBUTES)));
}
void EZ::SetTokenOwner(HANDLE token, PSID value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(PSID));
}
void EZ::SetTokenPrimaryGroup(HANDLE token, PSID value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(PSID));
}
void EZ::SetTokenDefaultDacl(HANDLE token, TOKEN_DEFAULT_DACL* value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(TOKEN_DEFAULT_DACL));
}
void EZ::SetTokenSource(HANDLE token, TOKEN_SOURCE value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(TOKEN_SOURCE));
}
void EZ::SetTokenType(HANDLE token, TOKEN_TYPE value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(TOKEN_TYPE));
}
void EZ::SetTokenImpersonationLevel(HANDLE token, SECURITY_IMPERSONATION_LEVEL value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(SECURITY_IMPERSONATION_LEVEL));
}
void EZ::SetTokenStatistics(HANDLE token, TOKEN_STATISTICS value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(TOKEN_STATISTICS));
}
void EZ::SetTokenRestrictedSids(HANDLE token, TOKEN_GROUPS* value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(TOKEN_GROUPS) + ((value->GroupCount - 1) * sizeof(SID_AND_ATTRIBUTES)));
}
void EZ::SetTokenSessionId(HANDLE token, DWORD value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(DWORD));
}
void EZ::SetTokenGroupsAndPrivileges(HANDLE token, TOKEN_GROUPS_AND_PRIVILEGES value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(TOKEN_GROUPS_AND_PRIVILEGES));
}
void EZ::SetTokenSandBoxInert(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(BOOL));
}
void EZ::SetTokenOrigin(HANDLE token, LUID value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(LUID));
}
void EZ::SetTokenElevationType(HANDLE token, TOKEN_ELEVATION_TYPE value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(TOKEN_ELEVATION_TYPE));
}
void EZ::SetTokenLinkedToken(HANDLE token, HANDLE value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(HANDLE));
}
void EZ::SetTokenElevation(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(BOOL));
}
void EZ::SetTokenHasRestrictions(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(BOOL));
}
void EZ::SetTokenAccessInformation(HANDLE token, TOKEN_ACCESS_INFORMATION value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(TOKEN_ACCESS_INFORMATION));
}
void EZ::SetTokenVirtualizationAllowed(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(BOOL));
}
void EZ::SetTokenVirtualizationEnabled(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(BOOL));
}
void EZ::SetTokenIntegrityLevel(HANDLE token, SID_AND_ATTRIBUTES value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(SID_AND_ATTRIBUTES));
}
void EZ::SetTokenUIAccess(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(BOOL));
}
void EZ::SetTokenMandatoryPolicy(HANDLE token, DWORD value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(DWORD));
}
void EZ::SetTokenLogonSid(HANDLE token, TOKEN_GROUPS* value) {
	SetTokenInfo(token, TokenUser, value, sizeof(TOKEN_GROUPS) + ((value->GroupCount - 1) * sizeof(SID_AND_ATTRIBUTES)));
}
void EZ::SetTokenIsAppContainer(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(BOOL));
}
void EZ::SetTokenCapabilities(HANDLE token, TOKEN_GROUPS* value) {
	SetTokenInfo(token, TokenUser, value, sizeof(TOKEN_GROUPS) + ((value->GroupCount - 1) * sizeof(SID_AND_ATTRIBUTES)));
}
void EZ::SetTokenAppContainerSid(HANDLE token, PSID value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(PSID));
}
void EZ::SetTokenAppContainerNumber(HANDLE token, DWORD value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(DWORD));
}
void EZ::SetTokenUserClaimAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value) {
	SetTokenInfo(token, TokenUser, value, sizeof(CLAIM_SECURITY_ATTRIBUTES_INFORMATION));
}
void EZ::SetTokenDeviceClaimAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value) {
	SetTokenInfo(token, TokenUser, value, sizeof(CLAIM_SECURITY_ATTRIBUTES_INFORMATION));
}
void EZ::SetTokenDeviceGroups(HANDLE token, TOKEN_GROUPS* value) {
	SetTokenInfo(token, TokenUser, value, sizeof(TOKEN_GROUPS) + ((value->GroupCount - 1) * sizeof(SID_AND_ATTRIBUTES)));
}
void EZ::SetTokenRestrictedDeviceGroups(HANDLE token, TOKEN_GROUPS* value) {
	SetTokenInfo(token, TokenUser, value, sizeof(TOKEN_GROUPS) + ((value->GroupCount - 1) * sizeof(SID_AND_ATTRIBUTES)));
}
void EZ::SetTokenSecurityAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value) {
	SetTokenInfo(token, TokenUser, value, sizeof(CLAIM_SECURITY_ATTRIBUTES_INFORMATION));
}
void EZ::SetTokenIsRestricted(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(BOOL));
}
void EZ::SetTokenProcessTrustLevel(HANDLE token, PSID value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(PSID));
}
void EZ::SetTokenPrivateNameSpace(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(BOOL));
}
void EZ::SetTokenSingletonAttributes(HANDLE token, CLAIM_SECURITY_ATTRIBUTES_INFORMATION* value) {
	SetTokenInfo(token, TokenUser, value, sizeof(CLAIM_SECURITY_ATTRIBUTES_INFORMATION));
}
void EZ::SetTokenBnoIsolation(HANDLE token, EZ::TOKEN_BNO_ISOLATION_INFORMATION value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(EZ::TOKEN_BNO_ISOLATION_INFORMATION));
}
void EZ::SetTokenIsSandboxed(HANDLE token, BOOL value) {
	SetTokenInfo(token, TokenUser, &value, sizeof(BOOL));
}


// Logging info about tokens as text to a specified output stream
void FormatHex(BYTE* value, DWORD length, std::wostream& outputStream) {
	outputStream << L"0x" << std::hex << std::uppercase << std::setw(2) << std::setfill(L'0');
	for (DWORD i = length - 1; i != 0xFFFFFFFF; i--)
	{
		outputStream << value[i];
	}
	outputStream << std::dec;
}
void FormatBinary(BYTE* value, DWORD length, std::wostream& outputStream) {
	for (DWORD i = 0; i < length; i++)
	{
		if (i != 0) { outputStream << L" "; }
		if ((value[i] & 128) == 0) { outputStream << L"0"; }
		else { outputStream << L"1"; }
		if ((value[i] & 64) == 0) { outputStream << L"0"; }
		else { outputStream << L"1"; }
		if ((value[i] & 32) == 0) { outputStream << L"0"; }
		else { outputStream << L"1"; }
		if ((value[i] & 16) == 0) { outputStream << L"0"; }
		else { outputStream << L"1"; }
		if ((value[i] & 8) == 0) { outputStream << L"0"; }
		else { outputStream << L"1"; }
		if ((value[i] & 4) == 0) { outputStream << L"0"; }
		else { outputStream << L"1"; }
		if ((value[i] & 2) == 0) { outputStream << L"0"; }
		else { outputStream << L"1"; }
		if ((value[i] & 1) == 0) { outputStream << L"0"; }
		else { outputStream << L"1"; }
	}
}
void FormatBool(BOOL value, std::wostream& outputStream) {
	if (value) {
		outputStream << L"True";
	}
	else {
		outputStream << L"False";
	}
}
void FormatSID(PSID value, std::wostream& outputStream) {
	TCHAR name[256];
	TCHAR domain[256];
	DWORD nameLen = sizeof(name) / sizeof(TCHAR);
	DWORD domainLen = sizeof(domain) / sizeof(TCHAR);
	SID_NAME_USE sidType;
	BOOL valid = LookupAccountSid(NULL, value, name, &nameLen, domain, &domainLen, &sidType);

	if (valid) {
		if (domainLen != 0)
		{
			outputStream << domain << L"/";
		}
		outputStream << name;
	}
	else {
		sidType = SidTypeInvalid;
	}

	switch (sidType) {
	case SidTypeUser:
		outputStream << L" (User)";
		break;
	case SidTypeGroup:
		outputStream << L" (Group)";
		break;
	case SidTypeDomain:
		outputStream << L" (Domain)";
		break;
	case SidTypeAlias:
		outputStream << L" (Alias)";
		break;
	case SidTypeWellKnownGroup:
		outputStream << L" (Well Known Group)";
		break;
	case SidTypeDeletedAccount:
		outputStream << L" (Deleted Account)";
		break;
	case SidTypeComputer:
		outputStream << L" (Computer)";
		break;
	case SidTypeLabel:
		outputStream << L" (Label)";
		break;
	case SidTypeLogonSession:
		outputStream << L" (Logon Session)";
		break;
	case SidTypeInvalid:
	case SidTypeUnknown:
	default:
		if (value == 0) { outputStream << L"Null SID"; }
		else { outputStream << L"Invalid/Unknown SID"; }
		break;
	}
}
void FormatLUID(LUID value, std::wostream& outputStream) {
	WCHAR privilegeName[256];
	DWORD nameLen = sizeof(privilegeName) / sizeof(privilegeName[0]);

	if (LookupPrivilegeName(NULL, &value, privilegeName, &nameLen)) {
		outputStream << privilegeName;
	}
	else {
		FormatHex(reinterpret_cast<BYTE*>(&value), 8, outputStream);
	}
}

void EZ::LogTokenUser(HANDLE token, std::wostream& outputStream) {
	try {
		SID_AND_ATTRIBUTES tokenUser = GetTokenUser(token);

		outputStream << L"Token User:" << std::endl;
		outputStream << L"    SID: "; FormatSID(tokenUser.Sid, outputStream); outputStream << std::endl;
		outputStream << L"    Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenUser.Attributes), 4, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenGroups(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_GROUPS* tokenGroups = GetTokenGroups(token);

		outputStream << L"Token Groups [" << tokenGroups->GroupCount << L"]:"; if (tokenGroups->GroupCount == 0) { outputStream << L" None"; } outputStream << std::endl;
		for (DWORD i = 0; i < tokenGroups->GroupCount; i++)
		{
			if (i != 0) { outputStream << std::endl; }
			outputStream << L"   SID: "; FormatSID(tokenGroups->Groups[i].Sid, outputStream); outputStream << std::endl;
			outputStream << L"   Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenGroups->Groups[i].Attributes), 4, outputStream); outputStream << std::endl;
		}

		delete[] tokenGroups;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenPrivileges(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_PRIVILEGES* tokenPrivileges = GetTokenPrivileges(token);

		outputStream << L"Token Privileges [" << tokenPrivileges->PrivilegeCount << L"]:"; if (tokenPrivileges->PrivilegeCount == 0) { outputStream << L" None"; } outputStream << std::endl;
		for (DWORD i = 0; i < tokenPrivileges->PrivilegeCount; i++)
		{
			if (i != 0) { outputStream << std::endl; }
			outputStream << "    LUID: "; FormatLUID(tokenPrivileges->Privileges[i].Luid, outputStream); outputStream << std::endl;
			outputStream << "    Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenPrivileges->Privileges[i].Attributes), 4, outputStream); outputStream << std::endl;
		}

		delete[] tokenPrivileges;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenOwner(HANDLE token, std::wostream& outputStream) {
	try {
		PSID tokenOwner = GetTokenOwner(token);

		outputStream << L"Token Owner: "; FormatSID(tokenOwner, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenPrimaryGroup(HANDLE token, std::wostream& outputStream) {
	try {
		PSID tokenPrimaryGroup = GetTokenPrimaryGroup(token);

		outputStream << L"Token Primary Group: "; FormatSID(tokenPrimaryGroup, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenDefaultDacl(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_DEFAULT_DACL* tokenDefaultDacl = GetTokenDefaultDacl(token);

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
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenSource(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_SOURCE tokenSource = GetTokenSource(token);

		CHAR safeSourceName[9];
		memcpy(safeSourceName, tokenSource.SourceName, 8);
		safeSourceName[8] = '\0';

		outputStream << L"Token Source:" << std::endl;
		outputStream << L"    Name: " << safeSourceName << std::endl;
		outputStream << L"    Identifier: "; FormatLUID(tokenSource.SourceIdentifier, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenType(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_TYPE tokenType = GetTokenType(token);

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
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenImpersonationLevel(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_TYPE tokenType = GetTokenType(token);
		if (tokenType != TokenImpersonation) {
			outputStream << L"Token Impersonation Level: N/A" << std::endl;
			return;
		}

		SECURITY_IMPERSONATION_LEVEL tokenImpersonationLevel = GetTokenImpersonationLevel(token);

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
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenStatistics(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_STATISTICS tokenStatistics = GetTokenStatistics(token);

		outputStream << L"Token Statistics:" << std::endl;
		outputStream << L"    Token ID: "; FormatLUID(tokenStatistics.TokenId, outputStream); outputStream << std::endl;
		outputStream << L"    Authentication ID: "; FormatLUID(tokenStatistics.AuthenticationId, outputStream); outputStream << std::endl;
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
		outputStream << L"    Modified ID: "; FormatLUID(tokenStatistics.ModifiedId, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenRestrictedSids(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_GROUPS* tokenRestrictedSids = GetTokenRestrictedSids(token);

		outputStream << L"Token Restricted SIDs [" << tokenRestrictedSids->GroupCount << L"]:"; if (tokenRestrictedSids->GroupCount == 0) { outputStream << L" None"; } outputStream << std::endl;
		for (DWORD i = 0; i < tokenRestrictedSids->GroupCount; i++)
		{
			if (i != 0) { outputStream << std::endl; }
			outputStream << L"    SID: "; FormatSID(tokenRestrictedSids->Groups[i].Sid, outputStream); outputStream << std::endl;
			outputStream << L"    Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenRestrictedSids->Groups[i].Attributes), 4, outputStream); outputStream << std::endl;
		}

		delete[] tokenRestrictedSids;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenSessionId(HANDLE token, std::wostream& outputStream) {
	try {
		DWORD tokenSessionId = GetTokenSessionId(token);

		outputStream << L"Token Session ID: " << tokenSessionId << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenGroupsAndPrivileges(HANDLE token, std::wostream& outputStream) {
	try {
		outputStream << L"Token Groups And Privileges: Information already printed." << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenSessionReference(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Session Refrences: Not supported" << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenSandBoxInert(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenSandBoxInert = GetTokenSandBoxInert(token);

		outputStream << L"Token Sand Box Inert: "; FormatBool(tokenSandBoxInert, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenAuditPolicy(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns access denied.
		outputStream << L"Token Audit Policy: Only accessible from kernel mode." << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenOrigin(HANDLE token, std::wostream& outputStream) {
	try {
		LUID tokenOrigin = GetTokenOrigin(token);

		outputStream << L"Token Origin: "; FormatLUID(tokenOrigin, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenElevationType(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_ELEVATION_TYPE tokenElevationType = GetTokenElevationType(token);

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
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenLinkedToken(HANDLE token, std::wostream& outputStream) {
	try {
		HANDLE tokenLinkedToken = GetTokenLinkedToken(token);

		outputStream << L"Token Linked Token: "; FormatHex(reinterpret_cast<BYTE*>(&tokenLinkedToken), 8, outputStream); outputStream << std::endl;

		CloseToken(tokenLinkedToken);
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenElevation(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenElevation = GetTokenElevation(token);

		outputStream << L"Token Is Elevated: "; FormatBool(tokenElevation, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenHasRestrictions(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenHasRestrictions = GetTokenHasRestrictions(token);

		outputStream << L"Token Has Restrictions: "; FormatBool(tokenHasRestrictions, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenAccessInformation(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_ACCESS_INFORMATION tokenAccessInformation = GetTokenAccessInformation(token);

		outputStream << L"Token Access Information: TODO" << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenVirtualizationAllowed(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenVirtualizationAllowed = GetTokenVirtualizationAllowed(token);

		outputStream << L"Token Virtualization Allowed: "; FormatBool(tokenVirtualizationAllowed, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenVirtualizationEnabled(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenVirtualizationEnabled = GetTokenVirtualizationEnabled(token);

		outputStream << L"Token Virtualization Enabled: "; FormatBool(tokenVirtualizationEnabled, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenIntegrityLevel(HANDLE token, std::wostream& outputStream) {
	try {
		SID_AND_ATTRIBUTES tokenIntegrityLevel = GetTokenIntegrityLevel(token);

		outputStream << L"Token Integrity Level:" << std::endl;
		outputStream << "    SID: "; FormatSID(tokenIntegrityLevel.Sid, outputStream); outputStream << std::endl;
		outputStream << "    Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenIntegrityLevel.Attributes), 4, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenUIAccess(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenUIAccess = GetTokenUIAccess(token);

		outputStream << L"Token UI Access: "; FormatBool(tokenUIAccess, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenMandatoryPolicy(HANDLE token, std::wostream& outputStream) {
	try {
		DWORD tokenMandatoryPolicy = GetTokenMandatoryPolicy(token);

		outputStream << L"Token Mandatory Policy: " << tokenMandatoryPolicy << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenLogonSid(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_GROUPS* tokenLogonSid = GetTokenLogonSid(token);

		outputStream << L"Token Logon Sids [" << tokenLogonSid->GroupCount << L"]:"; if (tokenLogonSid->GroupCount == 0) { outputStream << L" None"; } outputStream << std::endl;
		for (DWORD i = 0; i < tokenLogonSid->GroupCount; i++)
		{
			if (i != 0) { outputStream << std::endl; }
			outputStream << "    SID: "; FormatSID(tokenLogonSid->Groups[i].Sid, outputStream); outputStream << std::endl;
			outputStream << "    Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenLogonSid->Groups[i].Attributes), 4, outputStream); outputStream << std::endl;
		}

		delete[] tokenLogonSid;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenIsAppContainer(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenIsAppContainer = GetTokenIsAppContainer(token);

		outputStream << L"Token Is App Container: "; FormatBool(tokenIsAppContainer, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenCapabilities(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_GROUPS* tokenCapabilities = GetTokenCapabilities(token);

		outputStream << L"Token Capabilities [" << tokenCapabilities->GroupCount << L"]:"; if (tokenCapabilities->GroupCount == 0) { outputStream << L" None"; } outputStream << std::endl;
		for (DWORD i = 0; i < tokenCapabilities->GroupCount; i++)
		{
			if (i != 0) { outputStream << std::endl; }
			outputStream << "    SID: "; FormatSID(tokenCapabilities->Groups[i].Sid, outputStream); outputStream << std::endl;
			outputStream << "    Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenCapabilities->Groups[i].Attributes), 4, outputStream); outputStream << std::endl;
		}

		delete[] tokenCapabilities;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenAppContainerSid(HANDLE token, std::wostream& outputStream) {
	try {
		PSID tokenAppContainerSid = GetTokenAppContainerSid(token);

		outputStream << L"Token App Container SID: "; FormatSID(tokenAppContainerSid, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenAppContainerNumber(HANDLE token, std::wostream& outputStream) {
	try {
		DWORD tokenAppContainerNumber = GetTokenAppContainerNumber(token);

		outputStream << L"Token App Container Number: " << tokenAppContainerNumber << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenUserClaimAttributes(HANDLE token, std::wostream& outputStream) {
	try {
		CLAIM_SECURITY_ATTRIBUTES_INFORMATION* tokenUserClaimAttributes = GetTokenUserClaimAttributes(token);

		outputStream << L"Token User Claim Attributes:" << std::endl;
		outputStream << L"    Version: " << tokenUserClaimAttributes->Version << std::endl;
		outputStream << L"    Reserved: " << tokenUserClaimAttributes->Reserved << std::endl;
		outputStream << L"    Attributes [" << tokenUserClaimAttributes->AttributeCount << "]:"; if (tokenUserClaimAttributes->AttributeCount == 0) { outputStream << " None"; } outputStream << std::endl;
		for (DWORD i = 0; i < tokenUserClaimAttributes->AttributeCount; i++)
		{
			CLAIM_SECURITY_ATTRIBUTE_V1 attribute = tokenUserClaimAttributes->Attribute.pAttributeV1[i];
			outputStream << L"        Name: " << attribute.Name << std::endl;
			outputStream << L"        Value Type: " << attribute.ValueType << std::endl;
			outputStream << L"        Reserved: " << attribute.Reserved << std::endl;
			outputStream << L"        Flags: "; FormatBinary(reinterpret_cast<BYTE*>(&attribute.Flags), 4, outputStream); outputStream << std::endl;
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
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenDeviceClaimAttributes(HANDLE token, std::wostream& outputStream) {
	try {
		CLAIM_SECURITY_ATTRIBUTES_INFORMATION* tokenDeviceClaimAttributes = GetTokenDeviceClaimAttributes(token);

		outputStream << L"Token Device Claim Attributes: TODO" << std::endl;

		delete[] tokenDeviceClaimAttributes;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenRestrictedUserClaimAttributes(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Restricted User Claim Attributes: Not supported" << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenRestrictedDeviceClaimAttributes(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Restricted Device Claim Attributes: Not supported" << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenDeviceGroups(HANDLE token, std::wostream& outputStream) {
	try {
		TOKEN_GROUPS* tokenDeviceGroups = GetTokenDeviceGroups(token);

		outputStream << L"Token Device Groups [" << tokenDeviceGroups->GroupCount << L"]:"; if (tokenDeviceGroups->GroupCount == 0) { outputStream << L" None"; } outputStream << std::endl;
		for (DWORD i = 0; i < tokenDeviceGroups->GroupCount; i++)
		{
			if (i != 0) { outputStream << std::endl; }
			outputStream << "    SID: "; FormatSID(tokenDeviceGroups->Groups[i].Sid, outputStream); outputStream << std::endl;
			outputStream << "    Attributes: "; FormatBinary(reinterpret_cast<BYTE*>(&tokenDeviceGroups->Groups[i].Attributes), 4, outputStream); outputStream << std::endl;
		}

		delete[] tokenDeviceGroups;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenRestrictedDeviceGroups(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Restricted Device Groups: Not supported" << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenSecurityAttributes(HANDLE token, std::wostream& outputStream) {
	try {
		CLAIM_SECURITY_ATTRIBUTES_INFORMATION* tokenSecurityAttributes = GetTokenSecurityAttributes(token);

		outputStream << L"Token Security Attributes: TODO" << std::endl;

		delete[] tokenSecurityAttributes;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenIsRestricted(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenIsRestricted = GetTokenIsRestricted(token);

		outputStream << L"Token Is Restricted: "; FormatBool(tokenIsRestricted, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenProcessTrustLevel(HANDLE token, std::wostream& outputStream) {
	try {
		PSID tokenProcessTrustLevel = GetTokenProcessTrustLevel(token);

		outputStream << L"Token Process Trust Level: "; FormatSID(tokenProcessTrustLevel, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenPrivateNameSpace(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenPrivateNameSpace = GetTokenPrivateNameSpace(token);

		outputStream << L"Token Private Name Space: "; FormatBool(tokenPrivateNameSpace, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenSingletonAttributes(HANDLE token, std::wostream& outputStream) {
	try {
		CLAIM_SECURITY_ATTRIBUTES_INFORMATION* tokenSingletonAttributes = GetTokenSingletonAttributes(token);

		outputStream << L"Token Singleton Attributes: TODO" << std::endl;

		delete[] tokenSingletonAttributes;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenBnoIsolation(HANDLE token, std::wostream& outputStream) {
	try {
		EZ::TOKEN_BNO_ISOLATION_INFORMATION tokenBnoIsolation = GetTokenBnoIsolation(token);

		outputStream << L"Token BNO Isolation:" << std::endl;
		outputStream << L"    Enabled: "; FormatBool(tokenBnoIsolation.IsolationEnabled, outputStream); outputStream << std::endl;
		outputStream << L"    Prefix: "; FormatSID(tokenBnoIsolation.IsolationPrefixSid, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenChildProcessFlags(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Child Process Flags: Not supported" << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenIsLessPrivilegedAppContainer(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Is Less Privileged App Container: Not supported" << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenIsSandboxed(HANDLE token, std::wostream& outputStream) {
	try {
		BOOL tokenIsSandboxed = GetTokenIsSandboxed(token);

		outputStream << L"Token Is Sandboxed: "; FormatBool(tokenIsSandboxed, outputStream); outputStream << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenIsAppSilo(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Is App Silo: Not supported" << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogTokenLoggingInformation(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Token Logging Information: Not supported" << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}
void EZ::LogMaxTokenInfoClass(HANDLE token, std::wostream& outputStream) {
	try {
		// Always returns parameter incorrect.
		outputStream << L"Max Token Info Class: Not supported" << std::endl;
	}
	catch (EZ::Error error) { error.Print(); }
}

void EZ::LogTokenInfo(HANDLE token, std::wostream& outputStream) {
	outputStream << L"Token Handle: "; FormatHex(reinterpret_cast<BYTE*>(&token), 8, outputStream); outputStream << std::endl << std::endl;

	LogTokenUser(token, outputStream); outputStream << std::endl;
	LogTokenGroups(token, outputStream); outputStream << std::endl;
	LogTokenPrivileges(token, outputStream); outputStream << std::endl;
	LogTokenOwner(token, outputStream); outputStream << std::endl;
	LogTokenPrimaryGroup(token, outputStream); outputStream << std::endl;
	LogTokenDefaultDacl(token, outputStream); outputStream << std::endl;
	LogTokenSource(token, outputStream); outputStream << std::endl;
	LogTokenType(token, outputStream); outputStream << std::endl;
	LogTokenImpersonationLevel(token, outputStream); outputStream << std::endl;
	LogTokenStatistics(token, outputStream); outputStream << std::endl;
	LogTokenRestrictedSids(token, outputStream); outputStream << std::endl;
	LogTokenSessionId(token, outputStream); outputStream << std::endl;
	LogTokenGroupsAndPrivileges(token, outputStream); outputStream << std::endl;
	LogTokenSessionReference(token, outputStream); outputStream << std::endl;
	LogTokenSandBoxInert(token, outputStream); outputStream << std::endl;
	LogTokenAuditPolicy(token, outputStream); outputStream << std::endl;
	LogTokenOrigin(token, outputStream); outputStream << std::endl;
	LogTokenElevationType(token, outputStream); outputStream << std::endl;
	LogTokenLinkedToken(token, outputStream); outputStream << std::endl;
	LogTokenElevation(token, outputStream); outputStream << std::endl;
	LogTokenHasRestrictions(token, outputStream); outputStream << std::endl;
	LogTokenAccessInformation(token, outputStream); outputStream << std::endl;
	LogTokenVirtualizationAllowed(token, outputStream); outputStream << std::endl;
	LogTokenVirtualizationEnabled(token, outputStream); outputStream << std::endl;
	LogTokenIntegrityLevel(token, outputStream); outputStream << std::endl;
	LogTokenUIAccess(token, outputStream); outputStream << std::endl;
	LogTokenMandatoryPolicy(token, outputStream); outputStream << std::endl;
	LogTokenLogonSid(token, outputStream); outputStream << std::endl;
	LogTokenIsAppContainer(token, outputStream); outputStream << std::endl;
	LogTokenCapabilities(token, outputStream); outputStream << std::endl;
	LogTokenAppContainerSid(token, outputStream); outputStream << std::endl;
	LogTokenAppContainerNumber(token, outputStream); outputStream << std::endl;
	LogTokenUserClaimAttributes(token, outputStream); outputStream << std::endl;
	LogTokenDeviceClaimAttributes(token, outputStream); outputStream << std::endl;
	LogTokenRestrictedUserClaimAttributes(token, outputStream); outputStream << std::endl;
	LogTokenRestrictedDeviceClaimAttributes(token, outputStream); outputStream << std::endl;
	LogTokenDeviceGroups(token, outputStream); outputStream << std::endl;
	LogTokenRestrictedDeviceGroups(token, outputStream); outputStream << std::endl;
	LogTokenSecurityAttributes(token, outputStream); outputStream << std::endl;
	LogTokenIsRestricted(token, outputStream); outputStream << std::endl;
	LogTokenProcessTrustLevel(token, outputStream); outputStream << std::endl;
	LogTokenPrivateNameSpace(token, outputStream); outputStream << std::endl;
	LogTokenSingletonAttributes(token, outputStream); outputStream << std::endl;
	LogTokenBnoIsolation(token, outputStream); outputStream << std::endl;
	LogTokenChildProcessFlags(token, outputStream); outputStream << std::endl;
	LogTokenIsLessPrivilegedAppContainer(token, outputStream); outputStream << std::endl;
	LogTokenIsSandboxed(token, outputStream); outputStream << std::endl;
	LogTokenIsAppSilo(token, outputStream); outputStream << std::endl;
	LogTokenLoggingInformation(token, outputStream); outputStream << std::endl;
	LogMaxTokenInfoClass(token, outputStream); outputStream << std::endl;
}

// Working with the current token
HANDLE EZ::OpenCurrentToken() {
	HANDLE output;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &output)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	return output;
}
HANDLE EZ::DuplicateCurrentToken() {
	HANDLE currentToken = OpenCurrentToken();

	HANDLE currentTokenCopy;
	if (!DuplicateTokenEx(currentToken, TOKEN_ALL_ACCESS, NULL, SecurityDelegation, TokenPrimary, &currentTokenCopy)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return currentTokenCopy;
}
void EZ::CloseToken(HANDLE token) {
	if (!CloseHandle(token)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

// Impersonating tokens
void EZ::Impersonate(HANDLE token) {
	if (!ImpersonateLoggedOnUser(token)) {
		if (!SetThreadToken(NULL, token)) {
			EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
	}
}
void EZ::StopImpersonating() {
	if (!SetThreadToken(NULL, NULL)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!RevertToSelf()) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
void EZ::ImpersonateWinLogon() {
	// Create a snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Search through running processes using the snapshot.
	DWORD winLogonPID = 0; // PID 0 is always the System Idle Process so its a safe null value.
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(snapshot, &processEntry)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	do {
		// If the current process is WinLogon.exe return its PID.
		if (lstrcmpW(processEntry.szExeFile, L"winlogon.exe") == 0) {
			winLogonPID = processEntry.th32ProcessID;
			break;
		}
	} while (Process32Next(snapshot, &processEntry));

	// Throw an error if WinLogon.exe could not be found.
	if (winLogonPID == 0) {
		throw EZ::Error(L"WinLogon.exe could not be found in the list of running processes.", __FILE__, __LINE__);
	}

	// Open a handle to WinLogon.exe.
	HANDLE winLogon = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, winLogonPID);
	if (winLogon == NULL) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Open a handle to WinLogon.exe's token.
	HANDLE winLogonToken;
	if (!OpenProcessToken(winLogon, TOKEN_QUERY | TOKEN_DUPLICATE, &winLogonToken)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Start impersonating WinLogon.exe's token.
	Impersonate(winLogonToken);

	//Return and cleanup
	if (!CloseHandle(winLogonToken)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!CloseHandle(winLogon)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!CloseHandle(snapshot)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
void EZ::ImpersonateLsass() {
	// Create a snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == INVALID_HANDLE_VALUE) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Search through running processes using the snapshot.
	DWORD lsassPID = 0; // PID 0 is always the System Idle Process so its a safe null value.
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(snapshot, &processEntry)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	do {
		// If the current process is Lsass.exe return its PID.
		if (lstrcmpW(processEntry.szExeFile, L"lsass.exe") == 0) {
			lsassPID = processEntry.th32ProcessID;
			break;
		}
	} while (Process32Next(snapshot, &processEntry));

	// Throw an error if Lsass.exe could not be found.
	if (lsassPID == 0) {
		throw EZ::Error(L"Lsass.exe could not be found in the list of running processes.", __FILE__, __LINE__);
	}

	// Open a handle to Lsass.exe.
	HANDLE lsass = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, lsassPID);
	if (lsass == NULL) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Open a handle to Lsass.exe's token.
	HANDLE lsassToken;
	if (!OpenProcessToken(lsass, TOKEN_QUERY | TOKEN_DUPLICATE, &lsassToken)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Start impersonating Lsass.exe's token.
	Impersonate(lsassToken);

	//Return and cleanup
	if (!CloseHandle(lsassToken)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!CloseHandle(lsass)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!CloseHandle(snapshot)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}

// Enabling/disabling token privileges
LUID EZ::LookupPrivilege(LPCWSTR privilege) {
	LUID output;
	if (!LookupPrivilegeValue(NULL, privilege, &output))
	{
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	return output;
}
void EZ::EnableAllPrivileges(HANDLE token) {
	TOKEN_PRIVILEGES* privileges = GetTokenPrivileges(token);

	DWORD tpSize = sizeof(TOKEN_PRIVILEGES) + ((privileges->PrivilegeCount - 1) * (sizeof(LUID_AND_ATTRIBUTES))); // Default structure with 1 privilege plus an additional 35 privileges.
	TOKEN_PRIVILEGES* tp = reinterpret_cast<TOKEN_PRIVILEGES*>(new BYTE[tpSize]);
	tp->PrivilegeCount = privileges->PrivilegeCount;

	for (DWORD i = 0; i < tp->PrivilegeCount; i++) {
		tp->Privileges[i].Luid = privileges->Privileges[i].Luid;
		tp->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
	}

	if (!AdjustTokenPrivileges(token, FALSE, tp, tpSize, NULL, NULL))
	{
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	delete[] tp;
}
void EZ::DisableAllPrivileges(HANDLE token) {
	if (!AdjustTokenPrivileges(token, TRUE, NULL, 0, NULL, NULL))
	{
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
void EZ::EnablePrivilege(HANDLE token, LUID privilege) {
	TOKEN_PRIVILEGES tp = { };
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = privilege;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!AdjustTokenPrivileges(token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	// Required secondarry check because AdjustTokenPrivileges returns successful if some but not all permissions were adjusted.
	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
void EZ::DisablePrivilege(HANDLE token, LUID privilege) {
	TOKEN_PRIVILEGES tp = { };
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = privilege;
	tp.Privileges[0].Attributes = 0;

	if (!AdjustTokenPrivileges(token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL))
	{
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
BOOL EZ::TokenHasPrivilege(HANDLE token, LUID privilege) {
	TOKEN_PRIVILEGES* tokenPrivileges = GetTokenPrivileges(token);

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
LPWSTR EZ::GetCurrentExePath() {
	LPWSTR currentExePath = new WCHAR[MAX_PATH];
	if (GetModuleFileName(NULL, currentExePath, MAX_PATH) == 0) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	return currentExePath;
}
void EZ::CloseProcessInformation(PROCESS_INFORMATION processInfo) {
	if (!CloseHandle(processInfo.hThread)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	if (!CloseHandle(processInfo.hProcess)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
}
PROCESS_INFORMATION EZ::LaunchAsToken(HANDLE token, LPCWSTR exePath) {
	STARTUPINFO startupInfo = { };
	PROCESS_INFORMATION processInfo = { };
	if (!CreateProcessWithTokenW(token, LOGON_WITH_PROFILE, exePath, NULL, 0, NULL, NULL, &startupInfo, &processInfo)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return processInfo;
}
PROCESS_INFORMATION EZ::LaunchAsToken(HANDLE token) {
	LPWSTR currentExePath = GetCurrentExePath();

	STARTUPINFO startupInfo = { };
	GetStartupInfo(&startupInfo);
	PROCESS_INFORMATION processInfo = { };
	if (!CreateProcessWithTokenW(token, LOGON_WITH_PROFILE, currentExePath, NULL, 0, NULL, NULL, &startupInfo, &processInfo)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	delete[] currentExePath;

	return processInfo;
}
PROCESS_INFORMATION EZ::LaunchAsUser(HANDLE token, LPCWSTR exePath) {
	STARTUPINFO startupInfo = { };
	PROCESS_INFORMATION processInfo = { };
	if (!CreateProcessAsUser(token, exePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	return processInfo;
}
PROCESS_INFORMATION EZ::LaunchAsUser(HANDLE token) {
	LPWSTR currentExePath = GetCurrentExePath();

	STARTUPINFO startupInfo = { };
	GetStartupInfo(&startupInfo);
	PROCESS_INFORMATION processInfo = { };
	if (!CreateProcessAsUser(token, currentExePath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	delete[] currentExePath;

	return processInfo;
}
void EZ::RestartWithToken(HANDLE token) {
	try {
		CloseProcessInformation(LaunchAsToken(token));
	}
	catch (...) {
		CloseProcessInformation(LaunchAsUser(token));
	}

	// Close token because there is no way we will need it after closing the process.
	CloseToken(token);
	ExitProcess(0);
}

// Token privilege escalation
void EZ::GrantUIAccessToToken(HANDLE token) {
	// Impersonate WinLogon.exe
	ImpersonateWinLogon();

	// Give UIAccess to the token with the privilages we now have.
	SetTokenUIAccess(token, TRUE);

	// Stop impersonating WinLogon.exe's process token.
	StopImpersonating();
}
void EZ::MakeTokenInteractive(HANDLE token) {
	// Impersonate WinLogon.exe
	ImpersonateWinLogon();

	// Get current session id.
	DWORD activeConsoleSessionId = WTSGetActiveConsoleSessionId();
	if (activeConsoleSessionId == 0xFFFFFFFF) {
		throw EZ::Error(L"Could not create an interactive token because there is no active session currently.", __FILE__, __LINE__);
	}

	// Change the session ID of the token to the current session ID.
	SetTokenSessionId(token, activeConsoleSessionId);

	// Stop impersonating WinLogon.exe's process token.
	StopImpersonating();
}
void EZ::GiveTokenSystemIntegrity(HANDLE token) {
	// Lookup the system integrity level SID.
	PSID systemIntegritySid = NULL;
	if (!ConvertStringSidToSid(L"S-1-16-16384", &systemIntegritySid)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Assign system integrity level to the token.
	SID_AND_ATTRIBUTES tokenIntegrityLevel = { };
	tokenIntegrityLevel.Sid = systemIntegritySid;
	tokenIntegrityLevel.Attributes = SE_GROUP_INTEGRITY | SE_GROUP_INTEGRITY_ENABLED | SE_GROUP_MANDATORY;
	SetTokenIntegrityLevel(token, tokenIntegrityLevel);

	// Cleanup and return.
	LocalFree(systemIntegritySid);
}
void EZ::StealCreateTokenPermission(HANDLE token) {
	// Impersonate Lsass.exe
	ImpersonateLsass();

	// Give UIAccess to the token with the privilages we now have.
	TOKEN_PRIVILEGES tp = {};
	tp.PrivilegeCount = 1;
	if (!LookupPrivilegeValue(NULL, SE_CREATE_TOKEN_NAME, &tp.Privileges[0].Luid)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED | SE_PRIVILEGE_ENABLED_BY_DEFAULT;
	EZ::SetTokenPrivileges(token, &tp);

	// Stop impersonating WinLogon.exe's process token.
	EZ::StopImpersonating();
}

// TODO: Setup code to allow a supervisor process in session 0 to monitor if the interactive process has died and restart it in the current session.
#define PRIVCOUNT 35
#define GROUPCOUNT 5
const wchar_t* privs[] = { L"SeCreateTokenPrivilege", L"SeAssignPrimaryTokenPrivilege", L"SeLockMemoryPrivilege", L"SeIncreaseQuotaPrivilege", L"SeMachineAccountPrivilege", L"SeTcbPrivilege", L"SeSecurityPrivilege", L"SeTakeOwnershipPrivilege", L"SeLoadDriverPrivilege", L"SeSystemProfilePrivilege", L"SeSystemtimePrivilege", L"SeProfileSingleProcessPrivilege", L"SeIncreaseBasePriorityPrivilege", L"SeCreatePagefilePrivilege", L"SeCreatePermanentPrivilege", L"SeBackupPrivilege", L"SeRestorePrivilege", L"SeShutdownPrivilege", L"SeDebugPrivilege", L"SeAuditPrivilege", L"SeSystemEnvironmentPrivilege", L"SeChangeNotifyPrivilege", L"SeRemoteShutdownPrivilege", L"SeUndockPrivilege", L"SeSyncAgentPrivilege", L"SeEnableDelegationPrivilege", L"SeManageVolumePrivilege", L"SeImpersonatePrivilege", L"SeCreateGlobalPrivilege", L"SeTrustedCredManAccessPrivilege", L"SeRelabelPrivilege", L"SeIncreaseWorkingSetPrivilege", L"SeTimeZonePrivilege", L"SeCreateSymbolicLinkPrivilege", L"SeDelegateSessionUserImpersonatePrivilege" };
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
typedef NTSYSAPI NTSTATUS(NTAPI* _NtCreateToken)(OUT PHANDLE TokenHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes, IN TOKEN_TYPE TokenType, IN PLUID AuthenticationId, IN PLARGE_INTEGER ExpirationTime, IN PTOKEN_USER TokenUser, IN PTOKEN_GROUPS TokenGroups, IN PTOKEN_PRIVILEGES TokenPrivileges, IN PTOKEN_OWNER TokenOwner, IN PTOKEN_PRIMARY_GROUP TokenPrimaryGroup, IN PTOKEN_DEFAULT_DACL TokenDefaultDacl, IN PTOKEN_SOURCE TokenSource);
typedef NTSYSAPI NTSTATUS(NTAPI* _NtAllocateLocallyUniqueId)(OUT PLUID LocallyUniqueId);
HANDLE EZ::CreateGodToken() {
	/* KNOWN ISSUE
	NtCreateToken only works with pointers to stack memory or pointers to
	heap memory allocated with LocalAlloc or GlobalAlloc. C++ style new[]
	or C style malloc will not work.
	*/
	/* KNOWN ISSUE
	The SE_UNSOLICITED_INPUT_NAME privilege is not supported on Windows 10
	home edition and therefore is not given to the god token.
	*/

	// Impersonate Lsass to get the SE_CREATE_TOKEN_NAME privilege
	ImpersonateLsass();

	// Load ntdll.dll
	HMODULE ntdll = LoadLibrary(L"ntdll.dll");
	if (ntdll == NULL) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	// Load NtCreateToken function from ntdll.dll
	_NtCreateToken NtCreateToken = reinterpret_cast<_NtCreateToken>(GetProcAddress(ntdll, "NtCreateToken"));
	if (NtCreateToken == NULL) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	// Load NtAllocateLocallyUniqueId function from ntdll.dll
	_NtAllocateLocallyUniqueId NtAllocateLocallyUniqueId = reinterpret_cast<_NtAllocateLocallyUniqueId>(GetProcAddress(ntdll, "NtAllocateLocallyUniqueId"));
	if (NtAllocateLocallyUniqueId == NULL) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	// Get sids for users, groups, and integrity levels we need.
	PSID systemUserSid = NULL;
	if (!ConvertStringSidToSidW(L"S-1-5-18", &systemUserSid)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	PSID administratorsGroupSid = NULL;
	if (!ConvertStringSidToSidW(L"S-1-5-32-544", &administratorsGroupSid)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	PSID authenticatedUsersGroupSid = NULL;
	if (!ConvertStringSidToSidW(L"S-1-5-11", &authenticatedUsersGroupSid)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	PSID everyoneGroupSid = NULL;
	if (!ConvertStringSidToSidW(L"S-1-1-0", &everyoneGroupSid)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	PSID systemIntegrityLevelSid = NULL;
	if (!ConvertStringSidToSidW(L"S-1-16-16384", &systemIntegrityLevelSid)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	PSID trustedInstallerUserSid = NULL;
	if (!ConvertStringSidToSidW(L"S-1-5-80-956008885-3418522649-1831038044-1853292631-2271478464", &trustedInstallerUserSid)) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
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
	tokenPrivileges->Privileges[0].Luid = LookupPrivilege(SE_CREATE_TOKEN_NAME);
	tokenPrivileges->Privileges[1].Luid = LookupPrivilege(SE_ASSIGNPRIMARYTOKEN_NAME);
	tokenPrivileges->Privileges[2].Luid = LookupPrivilege(SE_LOCK_MEMORY_NAME);
	tokenPrivileges->Privileges[3].Luid = LookupPrivilege(SE_INCREASE_QUOTA_NAME);
	tokenPrivileges->Privileges[4].Luid = LookupPrivilege(SE_MACHINE_ACCOUNT_NAME);
	tokenPrivileges->Privileges[5].Luid = LookupPrivilege(SE_TCB_NAME);
	tokenPrivileges->Privileges[6].Luid = LookupPrivilege(SE_SECURITY_NAME);
	tokenPrivileges->Privileges[7].Luid = LookupPrivilege(SE_TAKE_OWNERSHIP_NAME);
	tokenPrivileges->Privileges[8].Luid = LookupPrivilege(SE_LOAD_DRIVER_NAME);
	tokenPrivileges->Privileges[9].Luid = LookupPrivilege(SE_SYSTEM_PROFILE_NAME);
	tokenPrivileges->Privileges[10].Luid = LookupPrivilege(SE_SYSTEMTIME_NAME);
	tokenPrivileges->Privileges[11].Luid = LookupPrivilege(SE_PROF_SINGLE_PROCESS_NAME);
	tokenPrivileges->Privileges[12].Luid = LookupPrivilege(SE_INC_BASE_PRIORITY_NAME);
	tokenPrivileges->Privileges[13].Luid = LookupPrivilege(SE_CREATE_PAGEFILE_NAME);
	tokenPrivileges->Privileges[14].Luid = LookupPrivilege(SE_CREATE_PERMANENT_NAME);
	tokenPrivileges->Privileges[15].Luid = LookupPrivilege(SE_BACKUP_NAME);
	tokenPrivileges->Privileges[16].Luid = LookupPrivilege(SE_RESTORE_NAME);
	tokenPrivileges->Privileges[17].Luid = LookupPrivilege(SE_SHUTDOWN_NAME);
	tokenPrivileges->Privileges[18].Luid = LookupPrivilege(SE_DEBUG_NAME);
	tokenPrivileges->Privileges[19].Luid = LookupPrivilege(SE_AUDIT_NAME);
	tokenPrivileges->Privileges[20].Luid = LookupPrivilege(SE_SYSTEM_ENVIRONMENT_NAME);
	tokenPrivileges->Privileges[21].Luid = LookupPrivilege(SE_CHANGE_NOTIFY_NAME);
	tokenPrivileges->Privileges[22].Luid = LookupPrivilege(SE_REMOTE_SHUTDOWN_NAME);
	tokenPrivileges->Privileges[23].Luid = LookupPrivilege(SE_UNDOCK_NAME);
	tokenPrivileges->Privileges[24].Luid = LookupPrivilege(SE_SYNC_AGENT_NAME);
	tokenPrivileges->Privileges[25].Luid = LookupPrivilege(SE_ENABLE_DELEGATION_NAME);
	tokenPrivileges->Privileges[26].Luid = LookupPrivilege(SE_MANAGE_VOLUME_NAME);
	tokenPrivileges->Privileges[27].Luid = LookupPrivilege(SE_IMPERSONATE_NAME);
	tokenPrivileges->Privileges[28].Luid = LookupPrivilege(SE_CREATE_GLOBAL_NAME);
	tokenPrivileges->Privileges[29].Luid = LookupPrivilege(SE_TRUSTED_CREDMAN_ACCESS_NAME);
	tokenPrivileges->Privileges[30].Luid = LookupPrivilege(SE_RELABEL_NAME);
	tokenPrivileges->Privileges[31].Luid = LookupPrivilege(SE_INC_WORKING_SET_NAME);
	tokenPrivileges->Privileges[32].Luid = LookupPrivilege(SE_TIME_ZONE_NAME);
	tokenPrivileges->Privileges[33].Luid = LookupPrivilege(SE_CREATE_SYMBOLIC_LINK_NAME);
	tokenPrivileges->Privileges[34].Luid = LookupPrivilege(SE_DELEGATE_SESSION_USER_IMPERSONATE_NAME);

	TOKEN_OWNER tokenOwner = { };
	tokenOwner.Owner = administratorsGroupSid;

	TOKEN_PRIMARY_GROUP tokenPrimaryGroup = { };
	tokenPrimaryGroup.PrimaryGroup = administratorsGroupSid;

	PTOKEN_DEFAULT_DACL tokenDefaultDacl = reinterpret_cast<PTOKEN_DEFAULT_DACL>(LocalAlloc(LPTR, sizeof(PTOKEN_DEFAULT_DACL)));
	tokenDefaultDacl->DefaultDacl = NULL;

	PTOKEN_SOURCE tokenSource = reinterpret_cast<PTOKEN_SOURCE>(LocalAlloc(LPTR, sizeof(TOKEN_SOURCE)));
	//EZ::Error::ThrowFromNT(NtAllocateLocallyUniqueId(&tokenSource->SourceIdentifier));
	tokenSource->SourceIdentifier = { 0x00000000, 0x00000000 };
	memcpy(tokenSource->SourceName, "MYSTERY\0", TOKEN_SOURCE_LENGTH);

	HANDLE token;
	EZ::Error::ThrowFromNT(NtCreateToken(&token, desiredAccess, &objectAttributes, tokenType, &authenticationID, &expirationTime, &tokenUser, tokenGroups, tokenPrivileges, &tokenOwner, &tokenPrimaryGroup, tokenDefaultDacl, tokenSource));


	DWORD activeConsoleSessionId = WTSGetActiveConsoleSessionId();
	if (activeConsoleSessionId != 0xFFFFFFFF) {
		SetTokenSessionId(token, activeConsoleSessionId);
	}

	DWORD uiAccess = TRUE;
	SetTokenUIAccess(token, uiAccess);

	DWORD virtualizationAllowed = TRUE;
	SetTokenVirtualizationAllowed(token, virtualizationAllowed);

	// Cleanup and return
	StopImpersonating();

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