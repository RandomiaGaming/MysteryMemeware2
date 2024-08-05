#pragma once
#include <Windows.h>
#include <ostream>

void PrintTokenUser(HANDLE token, std::wostream& output);
void PrintTokenGroups(HANDLE token, std::wostream& output);
void PrintTokenPrivileges(HANDLE token, std::wostream& output);
void PrintTokenOwner(HANDLE token, std::wostream& output);
void PrintTokenPrimaryGroup(HANDLE token, std::wostream& output);
void PrintTokenDefaultDacl(HANDLE token, std::wostream& output);
void PrintTokenSource(HANDLE token, std::wostream& output);
void PrintTokenType(HANDLE token, std::wostream& output);
void PrintTokenImpersonationLevel(HANDLE token, std::wostream& output);
void PrintTokenStatistics(HANDLE token, std::wostream& output);
void PrintTokenRestrictedSids(HANDLE token, std::wostream& output);
void PrintTokenSessionID(HANDLE token, std::wostream& output);
void PrintTokenGroupsAndPrivileges(HANDLE token, std::wostream& output);
void PrintTokenSessionReference(HANDLE token, std::wostream& output);
void PrintTokenSandBoxInert(HANDLE token, std::wostream& output);
void PrintTokenAuditPolicy(HANDLE token, std::wostream& output);
void PrintTokenOrigin(HANDLE token, std::wostream& output);
void PrintTokenElevationType(HANDLE token, std::wostream& output);
void PrintTokenLinkedToken(HANDLE token, std::wostream& output);
void PrintTokenElevation(HANDLE token, std::wostream& output);
void PrintTokenHasRestrictions(HANDLE token, std::wostream& output);
void PrintTokenAccessInformation(HANDLE token, std::wostream& output);
void PrintTokenVirtualizationAllowed(HANDLE token, std::wostream& output);
void PrintTokenVirtualizationEnabled(HANDLE token, std::wostream& output);
void PrintTokenIntegrityLevel(HANDLE token, std::wostream& output);
void PrintTokenUIAccess(HANDLE token, std::wostream& output);
void PrintTokenMandatoryPolicy(HANDLE token, std::wostream& output);
void PrintTokenLogonSid(HANDLE token, std::wostream& output);
void PrintTokenIsAppContainer(HANDLE token, std::wostream& output);
void PrintTokenCapabilities(HANDLE token, std::wostream& output);
void PrintTokenAppContainerSid(HANDLE token, std::wostream& output);
void PrintTokenAppContainerNumber(HANDLE token, std::wostream& output);
void PrintTokenUserClaimAttributes(HANDLE token, std::wostream& output);
void PrintTokenDeviceClaimAttributes(HANDLE token, std::wostream& output);
void PrintTokenRestrictedUserClaimAttributes(HANDLE token, std::wostream& output);
void PrintTokenRestrictedDeviceClaimAttributes(HANDLE token, std::wostream& output);
void PrintTokenDeviceGroups(HANDLE token, std::wostream& output);
void PrintTokenRestrictedDeviceGroups(HANDLE token, std::wostream& output);
void PrintTokenSecurityAttributes(HANDLE token, std::wostream& output);
void PrintTokenIsRestricted(HANDLE token, std::wostream& output);
void PrintTokenProcessTrustLevel(HANDLE token, std::wostream& output);
void PrintTokenPrivateNameSpace(HANDLE token, std::wostream& output);
void PrintTokenSingletonAttributes(HANDLE token, std::wostream& output);
void PrintTokenBnoIsolation(HANDLE token, std::wostream& output);
void PrintTokenChildProcessFlags(HANDLE token, std::wostream& output);
void PrintTokenIsLessPrivilegedAppContainer(HANDLE token, std::wostream& output);
void PrintTokenIsSandboxed(HANDLE token, std::wostream& output);
void PrintTokenIsAppSilo(HANDLE token, std::wostream& output);
void PrintTokenLoggingInformation(HANDLE token, std::wostream& output);
void PrintMaxTokenInfoClass(HANDLE token, std::wostream& output);

void PrintTokenInfo(HANDLE token, std::wostream& output);

void SaveTokenInfo();