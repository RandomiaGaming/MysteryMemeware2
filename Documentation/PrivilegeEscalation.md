The following is a comprehensive guide to privilege escalation exploits in windows 10/11 in the form of if you have this then you can do this to get this.

If you have:
SeCreateTokenPrivilege
Then you can:
Call the NtCreateToken undocumented function in ntdll.dll
To get:
A god token with all privileges and complete access to the system.

If you have:
SeDebugPrivilege and SeImporsonatePrivilege
Then you can:
Open the process token of lsass.exe and impersonate that token
To get:
SeCreateTokenPrivilege

If you have:
A user willing to press yes to a consent.exe popup
Then you can:
Restart your process with shellexecute and the runas verb
To get:
SeDebugPrivilege and SeImporsonatePrivilege

If you have:
The ability to create or hyjack a scheduled task
Then you can:
Create a scheduled task to run your process with the highest privileges
To get:
SeDebugPrivilege and SeImporsonatePrivilege

If you have:
The ability to create or hyjack a service
Then you can:
Create a service to run your process
To get:
SeDebugPrivilege and SeImporsonatePrivilege

If you have:
A process running as a user which is a member of the administrators localgroup
Then you can:
Set the value of "HKCU\Software\Classes\ms-settings\Shell\Open\command\(default)" to your executables path
And run FodHelper.exe
To get:
A process running as administrator

If you have:
A process running as administrator
Then you can:
Run the following powershell command:
powershell -Command Add-MpPreference -ExclusionPath "C:\PathToProgram.exe"
To get:
Windows defender to stop being a pain in your ass