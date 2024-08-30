del "C:\MysteryMemeware.exe"
reagentc /Enable
reg load "HKLM\OnlineSoftware" "C:\Windows\System32\config\SOFTWARE"
reg delete /f "HKLM\OnlineSoftware\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\LogonUI.exe"
reg delete /f "HKLM\OnlineSoftware\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\BootIm.exe"
reg unload "HKLM\OnlineSoftware"