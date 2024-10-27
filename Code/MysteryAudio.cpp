// Approved 10/26/2024

#include "MysteryAudio.h"
#include "EzCpp/EzError.h"
#include "MysterySong.h"
#include "EzCpp/EzAudio.h"
#include "EzCpp/EzLL.h"
#include <iostream>


struct Context {
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
static EzLL<Context*> contexts = { };
static IMMDeviceEnumerator* deviceEnumerator = NULL;

static Context* GetContextFromDeviceID(LPWSTR deviceID) {
	UINT32 contextCount = contexts.Count();
	contexts.EnumHead();
	for (UINT32 i = 0; i < contextCount; i++)
	{
		Context* context = contexts.EnumGet();
		contexts.EnumNext();

		if (lstrcmp(deviceID, context->deviceID) == 0) {
			return context;
		}
	}
	return NULL;
}
static void AddContext(IMMDevice* device) {
	Context* context = new Context();
	memset(context, 0, sizeof(Context));

	context->disabled = FALSE;
	context->disabledTime = 0;

	context->device = device;
	context->deviceID = EzAudioGetDeviceID(context->device);
	context->deviceName = EzAudioGetDeviceName(context->device);

	context->volumeController = EzAudioGetVolumeController(context->device);
	context->client = EzAudioGetClient(context->device);

	WAVEFORMATEX* assetFormat = EzAudioGetAssetFormat(&MysterySong_Asset);
	if (EzAudioClientSupportsFormat(context->client, assetFormat, TRUE)) {
		context->format = assetFormat;
		context->bufferLength = MysterySong_Asset.BufferLength;
		context->buffer = new BYTE[context->bufferLength];
		memcpy(context->buffer, MysterySong_Asset.Buffer, context->bufferLength);
	}
	else {
		std::wcout << "Transcoding mystery song for audio device " << context->deviceName << std::endl;
		context->format = EzAudioGetDeviceFormat(context->client);
		context->buffer = EzAudioTranscode(assetFormat, context->format, MysterySong_Asset.Buffer, MysterySong_BufferLength, &context->bufferLength);
		delete[] assetFormat;
	}
	context->position = 0;

	EzAudioInitClient(context->client, context->format, TRUE);
	context->renderer = EzAudioGetRenderer(context->client);
	EzAudioFillBuffer(context->client, context->renderer, context->format, context->buffer, context->bufferLength, &context->position, TRUE);
	EzAudioStartClient(context->client);

	contexts.InsertHead(context);
	std::wcout << L"Added new audio player for device " << context->deviceName << std::endl;
}
static void DisableContext(Context* context) {
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

	context->position = 0;
}
static void ReinitContext(Context* context) {
	context->volumeController = EzAudioGetVolumeController(context->device);
	context->client = EzAudioGetClient(context->device);

	context->position = 0;

	EzAudioInitClient(context->client, context->format, TRUE);
	context->renderer = EzAudioGetRenderer(context->client);
	EzAudioFillBuffer(context->client, context->renderer, context->format, context->buffer, context->bufferLength, &context->position, TRUE);
	EzAudioStartClient(context->client);

	context->disabled = FALSE;
	context->disabledTime = 0;

	std::wcout << L"Reinitialized audio player for device " << context->deviceName << std::endl;
}
static void RemoveContext(Context* context) {
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

	contexts.Remove(context);

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
		LPWSTR deviceID = EzAudioGetDeviceID(devices[i]);
		Context* context = GetContextFromDeviceID(deviceID);
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
	delete[] devices;

	UINT32 contextCount = contexts.Count();
	contexts.EnumHead();
	for (UINT32 i = 0; i < contextCount; i++)
	{
		Context* context = contexts.EnumGet();
		contexts.EnumNext();
		if (!context->disabled) {
			try {
				EzAudioFillBuffer(context->client, context->renderer, context->format, context->buffer, context->bufferLength, &context->position, TRUE);

				EzAudioSetVolume(context->volumeController, 0.15f);
				EzAudioSetMute(context->volumeController, FALSE);
			}
			catch (EzError error) {
				HRESULT errorHR = error.GetHR();
				if (errorHR == 0x80004005 /* EzAudioFillBuffer failed because device is disconnected. */
					|| errorHR == 0x8007001F /* Generic device disconnected error. */) {
					DisableContext(context);
				}
				else {
					throw;
				}
			}
		}
	}
}
void FreeMysteryAudio() {
	while (!contexts.IsEmpty())
	{
		Context* context = contexts.GetHead();
		RemoveContext(context);
	}
	deviceEnumerator->Release();
}