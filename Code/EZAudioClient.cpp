#include "EZAudioClient.h"
#include "Helper.h"
#include <audioclient.h>
#include <mmdeviceapi.h>

void EZ::PlayWAVExclusive(EZ::AudioAsset asset)
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
	WAVEFORMATEX waveFormat = { };
	waveFormat.wFormatTag = asset.FormatTag;
	waveFormat.nChannels = asset.ChannelCount;
	waveFormat.nSamplesPerSec = asset.SampleRate;
	waveFormat.nAvgBytesPerSec = asset.AverageBytesPerSecond;
	waveFormat.nBlockAlign = asset.BlockAlign;
	waveFormat.wBitsPerSample = asset.BitsPerSample;
	waveFormat.cbSize = asset.ExtraSize;

	// Initialize the audio client to use that wave format
	ThrowSysError(audioClient->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE, 0, 10000000 /* Internal Buffer Size In Ticks */, 0, &waveFormat, NULL));

	// Initialize an audio render client to be the sample provider.
	IAudioRenderClient* audioRenderClient = NULL;
	ThrowSysError(audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&audioRenderClient)));

	// Get the size of the buffer in frames.
	UINT32 bufferFrameCount = 0;
	ThrowSysError(audioClient->GetBufferSize(&bufferFrameCount));

	// Start playing the audio.
	ThrowSysError(audioClient->Start());

	UINT32 position = 0;
	UINT32 bufferLength = sizeof(asset.Buffer);
	while (position < bufferLength)
	{
		UINT32 numFramesPadding = 0;
		ThrowSysError(audioClient->GetCurrentPadding(&numFramesPadding));

		UINT32 numFramesAvailable = 0;
		numFramesAvailable = bufferFrameCount - numFramesPadding;

		BYTE* wasApiBuffer = NULL;
		ThrowSysError(audioRenderClient->GetBuffer(numFramesAvailable, &wasApiBuffer));

		UINT32 bytesToWrite = numFramesAvailable * waveFormat.nBlockAlign;
		if (bytesToWrite > (bufferLength - position))
		{
			bytesToWrite = bufferLength - position;
		}

		memcpy(wasApiBuffer, asset.Buffer + position, bytesToWrite);
		position += bytesToWrite;

		ThrowSysError(audioRenderClient->ReleaseBuffer(bytesToWrite / waveFormat.nBlockAlign, 0));
	}

	ThrowSysError(audioClient->Stop());

	CoUninitialize();
}