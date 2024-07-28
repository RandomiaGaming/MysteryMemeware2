/*
Known Issue: IAudioClient::GetMixFormat() returns the default format used by the audio mixer.
It does not return the default audio format for the underlying hardware. This means that the format
returned by IAudioClient::GetMixFormat() is guaranteed to work in shared mode but may not work in
exclusive mode if the hardware does not support it.
*/
#include "EZAudioClient.h"
#include "Helper.h"
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

void EZ::AudioClient::PlayExclusive(EZ::AudioAsset asset)
{
	// Initialize the COM component model if not already initialized.
	ThrowSysError(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY));

	// Create a device enumerator to loop over audio playback devices.
	IMMDeviceEnumerator* audioDeviceEnumerator = NULL;
	ThrowSysError(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&audioDeviceEnumerator)));

	// Get the default audio device from the enumerator.
	IMMDevice* audioDevice = NULL;
	ThrowSysError(audioDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice));

	// Create an audio client bound to the default device.
	IAudioClient* audioClient = NULL;
	ThrowSysError(audioDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioClient)));

	// Load the wave format from the asset.
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = 2;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Initialize the audio client to use that wave format
	ThrowSysError(audioClient->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE, 0, 10000000 /* Internal Buffer Size In Ticks */, 0, &waveFormat, NULL));

	// Initialize an audio render client to be the sample provider.
	IAudioRenderClient* audioRenderClient = NULL;
	ThrowSysError(audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&audioRenderClient)));

	// Start playing the audio.
	ThrowSysError(audioClient->Start());

	// Calculate the time to stop playing based on the length of the audio clip.
	LONGLONG exitTime = 0;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&exitTime));
	exitTime += (asset.Length * 10000000ll) / waveFormat.nAvgBytesPerSec;

	UINT32 position = 0;
	while (true)
	{
		LONGLONG timeNow = 0;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&timeNow));
		if (timeNow >= exitTime) {
			break;
		}

		// Get buffer info
		UINT32 acbFrames = 0;
		ThrowSysError(audioClient->GetBufferSize(&acbFrames));
		UINT32 acbPaddingFrames = 0;
		ThrowSysError(audioClient->GetCurrentPadding(&acbPaddingFrames));

		UINT32 arcbFrames = acbFrames - acbPaddingFrames;
		UINT32 arcbLength = arcbFrames * waveFormat.nBlockAlign;
		BYTE* arcb = NULL;
		ThrowSysError(audioRenderClient->GetBuffer(arcbFrames, &arcb));

		UINT32 rabLength = asset.Length - position;
		if (position >= asset.Length) {
			memset(arcb, 0, arcbLength);
		}
		else if (arcbLength > rabLength)
		{
			memcpy(arcb, asset.Buffer + position, rabLength);
			position += rabLength;
			memset(arcb + rabLength, 0, arcbLength - rabLength);
		}
		else {
			memcpy(arcb, asset.Buffer + position, arcbLength);
			position += arcbLength;
		}

		ThrowSysError(audioRenderClient->ReleaseBuffer(arcbFrames, 0));
	}

	// Stop playing, cleanup, and return.
	ThrowSysError(audioClient->Stop());
	audioRenderClient->Release();
	audioClient->Release();
	audioDevice->Release();
	audioDeviceEnumerator->Release();
	CoUninitialize();
}
void EZ::AudioClient::PlayExclusiveLooping(EZ::AudioAsset asset)
{
	// Initialize the COM component model if not already initialized.
	ThrowSysError(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY));

	// Create a device enumerator to loop over audio playback devices.
	IMMDeviceEnumerator* audioDeviceEnumerator = NULL;
	ThrowSysError(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&audioDeviceEnumerator)));

	// Get the default audio device from the enumerator.
	IMMDevice* audioDevice = NULL;
	ThrowSysError(audioDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice));

	// Create an audio client bound to the default device.
	IAudioClient* audioClient = NULL;
	ThrowSysError(audioDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioClient)));

	// Load the wave format from the asset.
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = 2;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Initialize the audio client to use that wave format
	ThrowSysError(audioClient->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE, 0, 10000000 /* Internal Buffer Size In Ticks */, 0, &waveFormat, NULL));

	// Initialize an audio render client to be the sample provider.
	IAudioRenderClient* audioRenderClient = NULL;
	ThrowSysError(audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&audioRenderClient)));

	// Start playing the audio.
	ThrowSysError(audioClient->Start());

	UINT32 position = 0;
	while (true)
	{
		// Get buffer info
		UINT32 acbFrames = 0;
		ThrowSysError(audioClient->GetBufferSize(&acbFrames));
		UINT32 acbPaddingFrames = 0;
		ThrowSysError(audioClient->GetCurrentPadding(&acbPaddingFrames));

		UINT32 arcbFrames = acbFrames - acbPaddingFrames;
		UINT32 arcbLength = arcbFrames * waveFormat.nBlockAlign;
		BYTE* arcb = NULL;
		ThrowSysError(audioRenderClient->GetBuffer(arcbFrames, &arcb));

		UINT32 rabLength = asset.Length - position;
		if (arcbLength > rabLength)
		{
			memcpy(arcb, asset.Buffer + position, rabLength);
			position = 0;
			memcpy(arcb + rabLength, asset.Buffer, arcbLength - rabLength);
		}
		else {
			memcpy(arcb, asset.Buffer + position, arcbLength);
			position += arcbLength;
		}

		ThrowSysError(audioRenderClient->ReleaseBuffer(arcbFrames, 0));
	}
}

BOOL EZ::AudioClient::GetMute() {
	// Initialize the COM component model if not already initialized.
	ThrowSysError(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY));

	// Create a device enumerator to loop over audio playback devices.
	IMMDeviceEnumerator* audioDeviceEnumerator = NULL;
	ThrowSysError(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&audioDeviceEnumerator)));

	// Get the default audio device from the enumerator.
	IMMDevice* audioDevice = NULL;
	ThrowSysError(audioDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice));

	// Create an audio endpoint volume bound to the default device.
	IAudioEndpointVolume* audioEndpointVolume = NULL;
	ThrowSysError(audioDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioEndpointVolume)));

	// Get the mute state.
	BOOL output = FALSE;
	audioEndpointVolume->GetMute(&output);

	// Cleanup and return
	audioEndpointVolume->Release();
	audioDevice->Release();
	audioDeviceEnumerator->Release();
	CoUninitialize();
	return output;
}
void EZ::AudioClient::SetMute(BOOL mute) {
	// Initialize the COM component model if not already initialized.
	ThrowSysError(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY));

	// Create a device enumerator to loop over audio playback devices.
	IMMDeviceEnumerator* audioDeviceEnumerator = NULL;
	ThrowSysError(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&audioDeviceEnumerator)));

	// Get the default audio device from the enumerator.
	IMMDevice* audioDevice = NULL;
	ThrowSysError(audioDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice));

	// Create an audio endpoint volume bound to the default device.
	IAudioEndpointVolume* audioEndpointVolume = NULL;
	ThrowSysError(audioDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioEndpointVolume)));

	// Set the mute state.
	audioEndpointVolume->SetMute(mute, NULL);

	// Cleanup and return
	audioEndpointVolume->Release();
	audioDevice->Release();
	audioDeviceEnumerator->Release();
	CoUninitialize();
}
FLOAT EZ::AudioClient::GetVolume() {
	// Initialize the COM component model if not already initialized.
	ThrowSysError(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY));

	// Create a device enumerator to loop over audio playback devices.
	IMMDeviceEnumerator* audioDeviceEnumerator = NULL;
	ThrowSysError(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&audioDeviceEnumerator)));

	// Get the default audio device from the enumerator.
	IMMDevice* audioDevice = NULL;
	ThrowSysError(audioDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice));

	// Create an audio endpoint volume bound to the default device.
	IAudioEndpointVolume* audioEndpointVolume = NULL;
	ThrowSysError(audioDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioEndpointVolume)));

	// Set the mute state.
	FLOAT output = 0.0f;
	audioEndpointVolume->GetMasterVolumeLevelScalar(&output);

	// Cleanup and return
	audioEndpointVolume->Release();
	audioDevice->Release();
	audioDeviceEnumerator->Release();
	CoUninitialize();
	return output;
}
void EZ::AudioClient::SetVolume(FLOAT volume) {
	// Initialize the COM component model if not already initialized.
	ThrowSysError(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY));

	// Create a device enumerator to loop over audio playback devices.
	IMMDeviceEnumerator* audioDeviceEnumerator = NULL;
	ThrowSysError(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&audioDeviceEnumerator)));

	// Get the default audio device from the enumerator.
	IMMDevice* audioDevice = NULL;
	ThrowSysError(audioDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice));

	// Create an audio endpoint volume bound to the default device.
	IAudioEndpointVolume* audioEndpointVolume = NULL;
	ThrowSysError(audioDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioEndpointVolume)));

	// Set the mute state.
	audioEndpointVolume->SetMasterVolumeLevelScalar(volume, NULL);

	// Cleanup and return
	audioEndpointVolume->Release();
	audioDevice->Release();
	audioDeviceEnumerator->Release();
	CoUninitialize();
}