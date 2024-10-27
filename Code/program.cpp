// Approved 10/26/2024

#include "EzCpp/EzError.h"
#include "MysteryControl.h"
#include "MysteryAudio.h"
#include "MysteryGraphics.h"
#include "EzCpp/EzHelper.h"
#include "EzCpp/EzLL.h"
#include "Program.h"

BOOL QuitRequested = FALSE;

int main(int argc, char** argv) {
	try {
		HRESULT hr = 0;
		hr = CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY);
		if (FAILED(hr)) {
			EzError::ThrowFromHR(hr, __FILE__, __LINE__);
		}

		if (InitMysteryControl()) {
			CoUninitialize();
			return 1;
		}
		InitMysteryAudio();
		//InitMysteryGraphics();

		while (!QuitRequested) {
			UpdateMysteryControl();
			UpdateMysteryAudio();
			//UpdateMysteryGraphics();
		}

		//FreeMysteryGraphics();
		FreeMysteryAudio();
		FreeMysteryControl();

		CoUninitialize();

		throw EzError("No errors program succeeded!", __FILE__, __LINE__);
	}
	catch (EzError error) {
		error.Print();
		return 1;
	}
}