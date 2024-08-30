#include "Program.h"
#include "EzError.h"
#include "MysteryControl.h"
#include "MysteryAudio.h"
#include "MysteryGraphics.h"
#include "EzHelper.h"

BOOL QuitRequested = FALSE;

int main(int argc, char** argv) {
	try {
		if (argc == 1 && lstrcmpA(argv[0], "BSOD") == 0) {
			IntentBSOD();
		}
		else if (argc == 1 && lstrcmpA(argv[0], "Install") == 0) {
			IntentInstall();
		}
		else if (argc == 1 && lstrcmpA(argv[0], "Uninstall") == 0) {
			IntentUninstall();
		}
		else {
			IntentRun();
		}		
	}
	catch (EzError error) {
		error.Print();
		return 1;
	}
}

void IntentBSOD() {
	EzSetProcessCritical(TRUE);
	ExitProcess(0);
}
void IntentInstall() {
	// TODO
}
void IntentRun() {
	HRESULT hr = 0;
	hr = CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	if (InitMysteryControl()) {
		CoUninitialize();
		return;
	}
	InitMysteryAudio();
	InitMysteryGraphics();

	while (!QuitRequested) {
		UpdateMysteryControl();
		UpdateMysteryAudio();
		UpdateMysteryGraphics();
	}

	FreeMysteryGraphics();
	FreeMysteryAudio();
	FreeMysteryControl();

	CoUninitialize();
	return;
}
void IntentUninstall() {
	// TODO
}