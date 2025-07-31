// Approved 10/29/2024

#pragma once
#include <Windows.h>
#include "EzCpp/EzAudio.h"

namespace MysterySong {
	extern const WORD FormatTag;
	extern const WORD ChannelCount;
	extern const DWORD SampleRate;
	extern const DWORD AverageBytesPerSecond;
	extern const WORD BlockAlign;
	extern const WORD BitsPerSample;
	extern const WORD ExtraDataLength;
	extern const BYTE* ExtraData;
	extern const UINT32 BufferLength;
	extern const BYTE Buffer[];
	extern const EzAudioAsset Asset;
}