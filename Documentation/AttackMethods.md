# Attack Methods
This document details the actions taken by MysteryMemeware to ensure proper functionality and proper system and user complience.
This document is layed out in the following form. To achive blank MysteryMemeware will do blank.

If MysteryMemeware does not have administrator
Then it will try to get administrator by exploiting FodHelper.exe for UAC bypass

If that fails
Then MysteryMemeware will try to get administrator by exploiting task scheduler UAC bypass

If that fails
Then MysteryMemeware will ask for administrator via Consent.exe UAC popup

If the user decliens
Then MysteryMemeware will crash

If MysteryMemeware is not running with a God token
Then MysteryMemeware will use NtCreateToken to restart with a God token

If MysteryMemeware is not marked as installed in the registries
Then the install routine will be running

If the install routine is run
Then MysteryMemeware will copy itself into the root of the OS boot drive

If the install routine is run
Then MysteryMemeware will set itself as the debugger for LogonUI.exe

If the install routine is run
Then MysteryMemeware will set itself as the debugger for BootIM.exe

If the install routine is run
Then MysteryMemeware will run ReAgentC.exe /disable

If the install routine is run
Then MysteryMemeware will run 





# General Control Flow
The general control flow is in the form blank does blank

MysteryEcalation ensures the process is running under a full access god token.

MysteryInstaller ensures that the program is installed properly.

MysterySupervisor keeps a constant watch on the system to block attempts to close the process.

MysteryAudio plays the beautiful music.

MysteryGraphics shows the pretty graphics.

If program.cpp detects the fail safe then MysteryGraphics, MtysteryAudio, and MysterySupervisor are shut down and MysteryInstaller uninstalls the software.