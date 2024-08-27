#include "Program.h"
#include "EzError.h"
#include "MysteryControl.h"
#include "MysteryAudio.h"
#include "MysteryGraphics.h"

BOOL QuitRequested = FALSE;

int main() {
	try {
		EzError::ThrowFromHR(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY));

		//InitMysteryControl();
		InitMysteryAudio();
		//InitMysteryGraphics();

		while (!QuitRequested) {
			//UpdateMysteryControl();
			UpdateMysteryAudio();
			//UpdateMysteryGraphics();
		}

		//FreeMysteryGraphics();
		FreeMysteryAudio();
		//FreeMysteryControl();

		CoUninitialize();

		return 0;
	}
	catch (EzError error) {
		error.Print();
		return 1;
	}
}