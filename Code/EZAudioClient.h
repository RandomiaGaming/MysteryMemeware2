#pragma once
#include <windows.h>

namespace EZ {
	struct AudioAsset {
		UINT32 Length;
		const BYTE* Buffer;
	};

	namespace AudioClient {
		void PlayExclusive(EZ::AudioAsset asset);
		void PlayExclusiveLooping(EZ::AudioAsset asset);

		BOOL GetMute();
		void SetMute(BOOL mute);
		FLOAT GetVolume();
		void SetVolume(FLOAT volume);
	}
}