#pragma once
#include <windows.h>
#include <audioclient.h>
#include <mmdeviceapi.h>

struct EzAudioAsset {
	WORD FormatTag;
	WORD ChannelCount;
	DWORD SampleRate;
	DWORD AverageBytesPerSecond;
	WORD BlockAlign;
	WORD BitsPerSample;
	WORD ExtraDataLength;
	const BYTE* ExtraData;
	UINT32 BufferLength;
	const BYTE* Buffer;
};

IMMDevice* EzGetDefaultAudioDevice();
WAVEFORMATEX* EzGetAudioMixFormat(IMMDevice* device);
WAVEFORMATEX* EzGetAudioDeviceFormat(IMMDevice* device);
UINT32 EzGetAudioDevices(IMMDevice*** pDevices);
LPWSTR EzGetAudioDeviceId(IMMDevice* device);
LPWSTR EzGetAudioDeviceName(IMMDevice* device);
WAVEFORMATEX* EzGetAssetWaveFormat(const EzAudioAsset* asset);
void EzCreateAudioContext(IMMDevice* device, const EzAudioAsset* asset, BOOL exclusive, IAudioClient** pAudioClient, IAudioRenderClient** pAudioRenderClient);
void EzCreateAudioContext(IMMDevice* device, const WAVEFORMATEX* waveFormat, BOOL exclusive, IAudioClient** pAudioClient, IAudioRenderClient** pAudioRenderClient);
LONGLONG EzStartAudioPlayer(IAudioClient* audioClient);
BOOL EzStopAudioAtTime(IAudioClient* audioClient, LONGLONG stopTime);
void EzFillAudioBuffer(IAudioClient* audioClient, IAudioRenderClient* audioRenderClient, const EzAudioAsset* asset, UINT32* position, BOOL looping);
void EzFillAudioBuffer(IAudioClient* audioClient, IAudioRenderClient* audioRenderClient, const WAVEFORMATEX* waveFormat, const BYTE* buffer, UINT32 bufferLength, UINT32* position, BOOL looping);
BYTE* EzTranscodeAudio(const WAVEFORMATEX* inputFormat, const WAVEFORMATEX* outputFormat, const BYTE* inputBuffer, UINT32 inputLength, UINT32* pOutputLength);
BOOL EzGetMute(IMMDevice* device);
void EzSetMute(IMMDevice* device, BOOL mute);
FLOAT EzGetVolume(IMMDevice* device);
void EzSetVolume(IMMDevice* device, FLOAT volume);