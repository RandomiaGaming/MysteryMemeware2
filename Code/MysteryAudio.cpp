#include "MysteryAudio.h"
#include "EzError.h"
#include "MysterySong.h"
#include "EzAudioPlayer.h"

struct AudioContext {
	IMMDevice* device;
	LPWSTR deviceID;
	LPWSTR deviceName;
	WAVEFORMATEX* waveFormat;
	UINT32 bufferLength;
	BYTE* buffer;
	IAudioClient* audioClient;
	IAudioRenderClient* audioRenderClient;
	UINT32 position;
};
UINT32 audioContextCount = 0;
AudioContext* audioContexts = NULL;
void InitMysteryAudio() {
	IMMDevice** devices = NULL;
	audioContextCount = EzGetAudioDevices(&devices);
	audioContextCount = 1;
	devices[0] = devices[1];
	audioContexts = new AudioContext[audioContextCount];
	memset(audioContexts, 0, sizeof(AudioContext) * audioContextCount);

	for (UINT32 i = 0; i < audioContextCount; i++) {
		audioContexts[i].device = devices[i];

		audioContexts[i].deviceID = EzGetAudioDeviceId(audioContexts[i].device);

		audioContexts[i].deviceName = EzGetAudioDeviceName(audioContexts[i].device);

		audioContexts[i].waveFormat = EzGetAudioDeviceFormat(audioContexts[i].device);

		WAVEFORMATEX* assetFormat = EzGetAssetWaveFormat(&MysterySong_Asset);
		audioContexts[i].buffer = EzTranscodeAudio(assetFormat, audioContexts[i].waveFormat, MysterySong_Asset.Buffer, MysterySong_BufferLength, &audioContexts[i].bufferLength);
		delete[] assetFormat;

		EzCreateAudioContext(audioContexts[i].device, audioContexts[i].waveFormat, TRUE, &audioContexts[i].audioClient, &audioContexts[i].audioRenderClient);

		EzFillAudioBuffer(audioContexts[i].audioClient, audioContexts[i].audioRenderClient, audioContexts[i].waveFormat, audioContexts[i].buffer, audioContexts[i].bufferLength, &audioContexts[i].position, FALSE);

		EzStartAudioPlayer(audioContexts[i].audioClient);
	}
}
void UpdateMysteryAudio() {
	for (UINT32 i = 0; i < audioContextCount; i++)
	{
		try {
			EzFillAudioBuffer(audioContexts[i].audioClient, audioContexts[i].audioRenderClient, audioContexts[i].waveFormat, audioContexts[i].buffer, audioContexts[i].bufferLength, &audioContexts[i].position, FALSE);

			//EzSetVolume(audioContexts[i].device, 0.25f);
			//EzSetMute(audioContexts[i].device, FALSE);
		}
		catch (EzError error) {
			error.Print();

		}
	}
}
void FreeMysteryAudio() {
	for (UINT32 i = 0; i < audioContextCount; i++)
	{
		AudioContext audioContext = audioContexts[i];

		EzError::ThrowFromHR(audioContext.audioClient->Stop(), __FILE__, __LINE__);

		audioContext.audioRenderClient->Release();
		audioContext.audioClient->Release();
		audioContext.device->Release();

		delete[] audioContext.buffer;
		delete[] audioContext.waveFormat;
	}
	audioContextCount = 0;
	delete[] audioContexts;
	audioContexts = NULL;
}