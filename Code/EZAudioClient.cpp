/*
Known Issue: IAudioClient::GetMixFormat() returns the default format used by the audio mixer.
It does not return the default audio format for the underlying hardware. This means that the format
returned by IAudioClient::GetMixFormat() is guaranteed to work in shared mode but may not work in
exclusive mode if the hardware does not support it.
*/
#include "EZAudioClient.h"
#include "EZError.h"
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

void EZ::AudioClient::PlayExclusive(EZ::AudioAsset asset)
{
	// Initialize the COM component model if not already initialized.
	EZ::Error::ThrowFromHR(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY), __FILE__, __LINE__);

	// Create a device enumerator to loop over audio playback devices.
	IMMDeviceEnumerator* audioDeviceEnumerator = NULL;
	EZ::Error::ThrowFromHR(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&audioDeviceEnumerator)), __FILE__, __LINE__);

	// Get the default audio device from the enumerator.
	IMMDevice* audioDevice = NULL;
	EZ::Error::ThrowFromHR(audioDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice), __FILE__, __LINE__);

	// Create an audio client bound to the default device.
	IAudioClient* audioClient = NULL;
	EZ::Error::ThrowFromHR(audioDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioClient)), __FILE__, __LINE__);

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
	EZ::Error::ThrowFromHR(audioClient->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE, 0, 10000000 /* Internal Buffer Size In Ticks */, 0, &waveFormat, NULL), __FILE__, __LINE__);

	// Initialize an audio render client to be the sample provider.
	IAudioRenderClient* audioRenderClient = NULL;
	EZ::Error::ThrowFromHR(audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&audioRenderClient)), __FILE__, __LINE__);

	// Start playing the audio.
	EZ::Error::ThrowFromHR(audioClient->Start());

	// Calculate the time to stop playing based on the length of the audio clip.
	LONGLONG exitTime = 0;
	if (!QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&exitTime))) {
		EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}
	exitTime += (asset.Length * 10000000ll) / waveFormat.nAvgBytesPerSec;

	UINT32 position = 0;
	while (true)
	{
		LONGLONG timeNow = 0;
		if (!QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&timeNow))) {
			EZ::Error::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
		}
		if (timeNow >= exitTime) {
			break;
		}

		// Get buffer info
		UINT32 acbFrames = 0;
		EZ::Error::ThrowFromHR(audioClient->GetBufferSize(&acbFrames), __FILE__, __LINE__);
		UINT32 acbPaddingFrames = 0;
		EZ::Error::ThrowFromHR(audioClient->GetCurrentPadding(&acbPaddingFrames), __FILE__, __LINE__);

		UINT32 arcbFrames = acbFrames - acbPaddingFrames;
		UINT32 arcbLength = arcbFrames * waveFormat.nBlockAlign;
		BYTE* arcb = NULL;
		EZ::Error::ThrowFromHR(audioRenderClient->GetBuffer(arcbFrames, &arcb), __FILE__, __LINE__);

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

		throw EZ::Error(audioRenderClient->ReleaseBuffer(arcbFrames, 0), __FILE__, __LINE__);
	}

	// Stop playing, cleanup, and return.
	EZ::Error::ThrowFromHR(audioClient->Stop(), __FILE__, __LINE__);
	audioRenderClient->Release();
	audioClient->Release();
	audioDevice->Release();
	audioDeviceEnumerator->Release();
	CoUninitialize();
}
void EZ::AudioClient::PlayExclusiveLooping(EZ::AudioAsset asset)
{
	// Initialize the COM component model if not already initialized.
	EZ::Error::ThrowFromHR(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY), __FILE__, __LINE__);

	// Create a device enumerator to loop over audio playback devices.
	IMMDeviceEnumerator* audioDeviceEnumerator = NULL;
	EZ::Error::ThrowFromHR(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&audioDeviceEnumerator)), __FILE__, __LINE__);

	// Get the default audio device from the enumerator.
	IMMDevice* audioDevice = NULL;
	EZ::Error::ThrowFromHR(audioDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice), __FILE__, __LINE__);

	// Create an audio client bound to the default device.
	IAudioClient* audioClient = NULL;
	EZ::Error::ThrowFromHR(audioDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioClient)), __FILE__, __LINE__);

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
	EZ::Error::ThrowFromHR(audioClient->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE, 0, 10000000 /* Internal Buffer Size In Ticks */, 0, &waveFormat, NULL), __FILE__, __LINE__);

	// Initialize an audio render client to be the sample provider.
	IAudioRenderClient* audioRenderClient = NULL;
	EZ::Error::ThrowFromHR(audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&audioRenderClient)), __FILE__, __LINE__);

	// Start playing the audio.
	EZ::Error::ThrowFromHR(audioClient->Start(), __FILE__, __LINE__);

	UINT32 position = 0;
	while (true)
	{
		// Get buffer info
		UINT32 acbFrames = 0;
		EZ::Error::ThrowFromHR(audioClient->GetBufferSize(&acbFrames), __FILE__, __LINE__);
		UINT32 acbPaddingFrames = 0;
		EZ::Error::ThrowFromHR(audioClient->GetCurrentPadding(&acbPaddingFrames), __FILE__, __LINE__);

		UINT32 arcbFrames = acbFrames - acbPaddingFrames;
		UINT32 arcbLength = arcbFrames * waveFormat.nBlockAlign;
		BYTE* arcb = NULL;
		EZ::Error::ThrowFromHR(audioRenderClient->GetBuffer(arcbFrames, &arcb), __FILE__, __LINE__);

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

		EZ::Error::ThrowFromHR(audioRenderClient->ReleaseBuffer(arcbFrames, 0), __FILE__, __LINE__);
	}
}

BOOL EZ::AudioClient::GetMute() {
	// Initialize the COM component model if not already initialized.
	EZ::Error::ThrowFromHR(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY), __FILE__, __LINE__);

	// Create a device enumerator to loop over audio playback devices.
	IMMDeviceEnumerator* audioDeviceEnumerator = NULL;
	EZ::Error::ThrowFromHR(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&audioDeviceEnumerator)), __FILE__, __LINE__);

	// Get the default audio device from the enumerator.
	IMMDevice* audioDevice = NULL;
	EZ::Error::ThrowFromHR(audioDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice), __FILE__, __LINE__);

	// Create an audio endpoint volume bound to the default device.
	IAudioEndpointVolume* audioEndpointVolume = NULL;
	EZ::Error::ThrowFromHR(audioDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioEndpointVolume)), __FILE__, __LINE__);

	// Get the mute state.
	BOOL output = FALSE;
	EZ::Error::ThrowFromHR(audioEndpointVolume->GetMute(&output), __FILE__, __LINE__);

	// Cleanup and return
	audioEndpointVolume->Release();
	audioDevice->Release();
	audioDeviceEnumerator->Release();
	CoUninitialize();
	return output;
}
void EZ::AudioClient::SetMute(BOOL mute) {
	// Initialize the COM component model if not already initialized.
	EZ::Error::ThrowFromHR(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY), __FILE__, __LINE__);

	// Create a device enumerator to loop over audio playback devices.
	IMMDeviceEnumerator* audioDeviceEnumerator = NULL;
	EZ::Error::ThrowFromHR(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&audioDeviceEnumerator)), __FILE__, __LINE__);

	// Get the default audio device from the enumerator.
	IMMDevice* audioDevice = NULL;
	EZ::Error::ThrowFromHR(audioDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice), __FILE__, __LINE__);

	// Create an audio endpoint volume bound to the default device.
	IAudioEndpointVolume* audioEndpointVolume = NULL;
	EZ::Error::ThrowFromHR(audioDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioEndpointVolume)), __FILE__, __LINE__);

	// Set the mute state.
	EZ::Error::ThrowFromHR(audioEndpointVolume->SetMute(mute, NULL), __FILE__, __LINE__);

	// Cleanup and return
	audioEndpointVolume->Release();
	audioDevice->Release();
	audioDeviceEnumerator->Release();
	CoUninitialize();
}
FLOAT EZ::AudioClient::GetVolume() {
	// Initialize the COM component model if not already initialized.
	EZ::Error::ThrowFromHR(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY), __FILE__, __LINE__);

	// Create a device enumerator to loop over audio playback devices.
	IMMDeviceEnumerator* audioDeviceEnumerator = NULL;
	EZ::Error::ThrowFromHR(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&audioDeviceEnumerator)), __FILE__, __LINE__);

	// Get the default audio device from the enumerator.
	IMMDevice* audioDevice = NULL;
	EZ::Error::ThrowFromHR(audioDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice), __FILE__, __LINE__);

	// Create an audio endpoint volume bound to the default device.
	IAudioEndpointVolume* audioEndpointVolume = NULL;
	EZ::Error::ThrowFromHR(audioDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioEndpointVolume)), __FILE__, __LINE__);

	// Set the mute state.
	FLOAT output = 0.0f;
	EZ::Error::ThrowFromHR(audioEndpointVolume->GetMasterVolumeLevelScalar(&output), __FILE__, __LINE__);

	// Cleanup and return
	audioEndpointVolume->Release();
	audioDevice->Release();
	audioDeviceEnumerator->Release();
	CoUninitialize();
	return output;
}
void EZ::AudioClient::SetVolume(FLOAT volume) {
	// Initialize the COM component model if not already initialized.
	EZ::Error::ThrowFromHR(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY), __FILE__, __LINE__);

	// Create a device enumerator to loop over audio playback devices.
	IMMDeviceEnumerator* audioDeviceEnumerator = NULL;
	EZ::Error::ThrowFromHR(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&audioDeviceEnumerator)), __FILE__, __LINE__);

	// Get the default audio device from the enumerator.
	IMMDevice* audioDevice = NULL;
	EZ::Error::ThrowFromHR(audioDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice), __FILE__, __LINE__);

	// Create an audio endpoint volume bound to the default device.
	IAudioEndpointVolume* audioEndpointVolume = NULL;
	EZ::Error::ThrowFromHR(audioDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioEndpointVolume)), __FILE__, __LINE__);

	// Set the mute state.
	EZ::Error::ThrowFromHR(audioEndpointVolume->SetMasterVolumeLevelScalar(volume, NULL), __FILE__, __LINE__);

	// Cleanup and return
	audioEndpointVolume->Release();
	audioDevice->Release();
	audioDeviceEnumerator->Release();
	CoUninitialize();
}