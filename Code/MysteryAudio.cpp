#include "MysteryAudio.h"
#include "EzError.h"
#include "MysterySong.h"
#include "EzAudio.h"
#include "EzLL.h"
#include <iostream>

IMMDeviceEnumerator* deviceEnumerator = NULL;
struct AudioContext {
	BOOL disabled;
	LONGLONG disabledTime;

	IMMDevice* device;
	LPWSTR deviceID;
	LPWSTR deviceName;

	IAudioEndpointVolume* volumeController;
	IAudioClient* client;
	IAudioRenderClient* renderer;

	WAVEFORMATEX* format;
	UINT32 bufferLength;
	BYTE* buffer;
	UINT32 position;
};
EzLL audioContexts = { };

AudioContext* GetContextFromDeviceID(LPWSTR deviceID) {
	UINT32 audioContextCount = EzLLCount(&audioContexts);
	EzLLEnumStart(&audioContexts);
	for (UINT32 i = 0; i < audioContextCount; i++)
	{
		AudioContext* current = reinterpret_cast<AudioContext*>(EzLLEnumGet(&audioContexts));
		EzLLEnumNext(&audioContexts);

		if (lstrcmp(deviceID, current->deviceID) == 0) {
			return current;
		}
	}
	return NULL;
}
void AddContext(IMMDevice* device) {
	AudioContext* context = new AudioContext();
	memset(context, 0, sizeof(AudioContext));

	context->disabled = FALSE;
	context->disabledTime = 0;

	context->device = device;
	context->deviceID = EzAudioGetDeviceID(context->device);
	context->deviceName = EzAudioGetDeviceName(context->device);

	context->volumeController = EzAudioGetVolumeController(context->device);
	context->client = EzAudioGetClient(context->device);

	context->format = EzAudioGetDeviceFormat(context->client);
	WAVEFORMATEX* assetFormat = EzAudioGetAssetFormat(&MysterySong_Asset);
	context->buffer = EzAudioTranscode(assetFormat, context->format, MysterySong_Asset.Buffer, MysterySong_BufferLength, &context->bufferLength);
	delete[] assetFormat;
	context->position = 0;

	EzAudioInitClient(context->client, context->format, TRUE);
	context->renderer = EzAudioGetRenderer(context->client);
	EzAudioFillBuffer(context->client, context->renderer, context->format, context->buffer, context->bufferLength, &context->position, TRUE);
	EzAudioStartClient(context->client);

	EzLLAdd(&audioContexts, context);
	std::wcout << L"Added new audio player for device " << context->deviceName << std::endl;
}
void DisableContext(AudioContext* context) {
	std::wcout << L"Disabling audio player due to error " << context->deviceName << std::endl;

	context->disabled = TRUE;
	if (!QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&context->disabledTime))) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	EzAudioStopClientAtTime(context->client, 0);
	context->renderer->Release();
	context->renderer = NULL;
	context->client->Release();
	context->client = NULL;
	context->volumeController->Release();
	context->volumeController = NULL;

	delete[] context->buffer;
	context->buffer = NULL;
	context->bufferLength = 0;
	context->position = 0;
	delete[] context->format;
	context->format = NULL;
}
void ReinitContext(AudioContext* context) {
	context->volumeController = EzAudioGetVolumeController(context->device);
	context->client = EzAudioGetClient(context->device);

	context->format = EzAudioGetDeviceFormat(context->client);
	WAVEFORMATEX* assetFormat = EzAudioGetAssetFormat(&MysterySong_Asset);
	context->buffer = EzAudioTranscode(assetFormat, context->format, MysterySong_Asset.Buffer, MysterySong_BufferLength, &context->bufferLength);
	delete[] assetFormat;
	context->position = 0;

	EzAudioInitClient(context->client, context->format, TRUE);
	context->renderer = EzAudioGetRenderer(context->client);
	EzAudioFillBuffer(context->client, context->renderer, context->format, context->buffer, context->bufferLength, &context->position, TRUE);
	EzAudioStartClient(context->client);

	context->disabled = FALSE;
	context->disabledTime = 0;

	std::wcout << L"Reinitialized audio player for device " << context->deviceName << std::endl;
}
void RemoveContext(AudioContext* context) {
	context->disabled = TRUE;
	context->disabledTime = 0;

	EzAudioStopClientAtTime(context->client, 0);
	context->renderer->Release();
	context->renderer = NULL;
	context->client->Release();
	context->client = NULL;
	context->volumeController->Release();
	context->volumeController = NULL;

	delete[] context->buffer;
	context->buffer = NULL;
	context->bufferLength = 0;
	context->position = 0;
	delete[] context->format;
	context->format = NULL;

	context->device->Release();
	context->device = NULL;
	delete[] context->deviceName;
	delete[] context->deviceID;

	EzLLRemove(&audioContexts, context);

	delete context;
}

void InitMysteryAudio() {
	deviceEnumerator = EzAudioGetDeviceEnumerator();
}
void UpdateMysteryAudio() {
	IMMDevice** devices = NULL;
	UINT32 deviceCount = EzAudioGetDevices(deviceEnumerator, &devices);
	for (UINT32 i = 0; i < deviceCount; i++)
	{
		try {
			LPWSTR deviceID = EzAudioGetDeviceID(devices[i]);
			AudioContext* context = GetContextFromDeviceID(deviceID);
			if (context == NULL) {
				AddContext(devices[i]);
			}
			else {
				devices[i]->Release();
				if (context->disabled) {
					LONGLONG timeNow = 0;
					if (!QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&timeNow))) {
						EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
					}
					if (timeNow - context->disabledTime > 10000000) {
						ReinitContext(context);
					}
				}
			}
			delete[] deviceID;
		}
		catch (EzError error) {
			error.Print();
		}
	}
	delete[] devices;

	UINT32 audioContextCount = EzLLCount(&audioContexts);
	EzLLEnumStart(&audioContexts);
	for (UINT32 i = 0; i < audioContextCount; i++)
	{
		AudioContext* context = reinterpret_cast<AudioContext*>(EzLLEnumGet(&audioContexts));
		EzLLEnumNext(&audioContexts);
		if (!context->disabled) {
			try {
				EzAudioFillBuffer(context->client, context->renderer, context->format, context->buffer, context->bufferLength, &context->position, TRUE);

				EzAudioSetVolume(context->volumeController, 0.15f);
				EzAudioSetMute(context->volumeController, FALSE);
			}
			catch (EzError error) {
				error.Print();
				DisableContext(context);
			}
		}
	}
}
void FreeMysteryAudio() {
	while (EzLLCount(&audioContexts) > 0)
	{
		AudioContext* context = reinterpret_cast<AudioContext*>(EzLLGetHead(&audioContexts));
		RemoveContext(context);
	}
	deviceEnumerator->Release();
}