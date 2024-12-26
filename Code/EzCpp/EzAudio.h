// Approved 11/14/2024

// KNOWN ISSUE
// The audio driver for my USB type-C to 3.5mm headphone jack adapter has a known
// issue where the driver reports a that the currently playing audio frame is free.
// This confuses EzAudio because it rightfully assumes that the free frames are safe
// to write new audio data too. However, writting audio data to the frame currently
// being played causes discontinuity errors and crackling. The real issue is with the
// audio driver but we can temporarily patch our code so that it always assumes slightly
// less frames are free then is reported by the driver to ensure we don't overwrite
// the currently playing frame.

// KNOWN ISSUE
// IAudioClient::GetMixFormat() returns the default format used by the audio mixer.
// It does not return the default audio format for the underlying hardware. This means that while
// the format returned by IAudioClient::GetMixFormat() is guaranteed to work in shared mode
// it likely will not work in exclusive mode unless the underlying hardware also supports that format.

// NOTE
// All methods which return pointers to WAVEFORMATEX structures have allocated
// those structures as BYTE arrays. This is the only way to handle the inconsistent
// size of the WAVEFORMATEX structure however it means that these pointers must
// be freed with delete[] not delete.

#pragma once
#include <windows.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

// Used by the EzAssetManager to store audio assets
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

// Better version of IsFormatSupported
BOOL EzAudioClientSupportsFormat(IAudioClient* client, const WAVEFORMATEX* format, BOOL exclusive);

// Devices and DeviceEnumerators
IMMDeviceEnumerator* EzAudioGetDeviceEnumerator();
IMMDevice* EzAudioGetDefaultDevice(IMMDeviceEnumerator* deviceEnumerator);
UINT32 EzAudioGetDevices(IMMDeviceEnumerator* deviceEnumerator, IMMDevice*** pDevices);
LPWSTR EzAudioGetDeviceID(IMMDevice* device);
LPWSTR EzAudioGetDeviceName(IMMDevice* device);

// Clients and WaveFormats
IAudioClient* EzAudioGetClient(IMMDevice* device);
WAVEFORMATEX* EzAudioGetClientMixFormat(IAudioClient* client);
WAVEFORMATEX* EzAudioGetDeviceFormat(IAudioClient* client);
WAVEFORMATEX* EzAudioGetAssetFormat(const EzAudioAsset* asset);

// Contexts
void EzAudioInitClient(IAudioClient* client, const WAVEFORMATEX* format, BOOL exclusive);
IAudioRenderClient* EzAudioGetRenderer(IAudioClient* client);
LONGLONG EzAudioStartClient(IAudioClient* client);
BOOL EzAudioStopClientAtTime(IAudioClient* client, LONGLONG stopTime);
void EzAudioFillBuffer(IAudioClient* client, IAudioRenderClient* renderer, const WAVEFORMATEX* format, const BYTE* buffer, UINT32 bufferLength, UINT32* position, BOOL looping);

// Transcoding
BYTE* EzAudioTranscode(const WAVEFORMATEX* inputFormat, const WAVEFORMATEX* outputFormat, const BYTE* inputBuffer, UINT32 inputLength, UINT32* pOutputLength);

// Volume and Mute
IAudioEndpointVolume* EzAudioGetVolumeController(IMMDevice* device);
FLOAT EzAudioGetVolume(IAudioEndpointVolume* volumeController);
void EzAudioSetVolume(IAudioEndpointVolume* volumeController, FLOAT volume);
BOOL EzAudioGetMute(IAudioEndpointVolume* volumeController);
void EzAudioSetMute(IAudioEndpointVolume* volumeController, BOOL mute);