#include <windows.h>

namespace EZ {
	struct AudioAsset {
		WORD FormatTag;
		WORD ChannelCount;
		DWORD SampleRate;
		DWORD AverageBytesPerSecond;
		WORD BlockAlign;
		WORD BitsPerSample;
		WORD ExtraSize;
		const BYTE* Buffer;
	};

	void PlayWAVExclusive(EZ::AudioAsset asset);
}