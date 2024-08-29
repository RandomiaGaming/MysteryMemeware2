#include "Program.h"
#include "EzError.h"
#include "MysteryControl.h"
#include "MysteryAudio.h"
#include "MysteryGraphics.h"
#include "EzLL.h"
#include <iostream>

BOOL QuitRequested = FALSE;

int main() {
	try {
		HRESULT hr = 0;
		hr = CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY);
		if (FAILED(hr)) {
			EzError::ThrowFromHR(hr, __FILE__, __LINE__);
		}

		//InitMysteryControl();
		InitMysteryAudio();
		InitMysteryGraphics();

		while (!QuitRequested) {
			//UpdateMysteryControl();
			UpdateMysteryAudio();
			UpdateMysteryGraphics();
		}

		FreeMysteryGraphics();
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