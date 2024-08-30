/* KNOWN ISSUE
The windows audio driver for my USB type-C to 3.5mm headphone jack adapter has
a glitch where it reports the incorrect number of free frames to the user when
calling IAudioClient::GetCurrentPadding(). Normally this function returns the
number of frames in the current buffer which are padding. Padding in this context
refers to frames of audio data which have not been played or are currently being
played by the sound card and therefore should not be overwritten. As such
Buffer Total Size In Frames - GetCurrentPadding() = Number Of Frames Safe To Write To
Unfortunately this audio driver incorrectly counts only frames which have not been
played yet as padding and crutually does not include those frames which are currently
being played as padding. This leads to the buffers looking like this.
<-----------------------overall buffer-------------------------->
<------buffer being written to------|
								  |-----buffer being played----->
Crutuially they slightly overlap. Now in normal circumstances this wont matter because
the user code doesn't actually write directly to the driver buffer but instead writes
to an intermediarry buffer which is then copied into the driver buffer by WASAPI.
But if the user code is super duper fast in filling its buffer and WASAPI is also
super fast in copying that data into the driver buffer then there is a chance it will
be fast enough to overwrite some audio data in the driver buffer while the sound card
is still playing that part. This causes a sharp jump from one sample to the next which
sounds like a pop or static. With a super optimized audio renderer like the one below
this glitch is all the more likely to occur and can happen so frequently that the pops
are deafening. That is also why the pops occur more frequently later on once the visual
c++ runtime has completed it's background tasks. This allows the audio renderer to use
100% of the CPU to go even faster and catch up to the sound card's play head even more
often.
*/

/* NOTE ON FREEING WAVEFORMATEX* STRUCTURES
All methods which return WAVEFORMATEX* are returning pointers to heap allocated
structures of variable size. Due to their variable sizes these structures must be
allocated as BYTE[]s which means they MUST be deleted with delete[].
Ensure array delete[] is used for all WAVEFORMATEX* pointers.
*/

#pragma once
#include <windows.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

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
DEFINE_PROPERTYKEY(PKEY_Device_FriendlyName, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 14); // Taken from Functiondiscoverykeys_devpkey.h
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