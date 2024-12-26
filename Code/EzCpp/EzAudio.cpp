// Approved 11/14/2024

#include "EzAudio.h"
#include "EzError.h"

#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <mftransform.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <wmcodecdsp.h>
#include <combaseapi.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")

// Better version of IsFormatSupported
BOOL EzAudioClientSupportsFormat(IAudioClient* client, const WAVEFORMATEX* format, BOOL exclusive) {
	HRESULT hr = 0;

	if (exclusive) {
		hr = client->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, format, NULL);
	}
	else {
		WAVEFORMATEX* closestMatch = NULL;
		hr = client->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, format, &closestMatch);
		if (closestMatch != NULL) {
			CoTaskMemFree(closestMatch);
		}
	}

	if (hr == S_OK) {
		return TRUE;
	}
	else if (hr == S_FALSE || hr == AUDCLNT_E_UNSUPPORTED_FORMAT) {
		return FALSE;
	}
	else {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
}

// Devices and DeviceEnumerators
IMMDeviceEnumerator* EzAudioGetDeviceEnumerator() {
	HRESULT hr = 0;

	IMMDeviceEnumerator* deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&deviceEnumerator));
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	return deviceEnumerator;
}
IMMDevice* EzAudioGetDefaultDevice(IMMDeviceEnumerator* deviceEnumerator) {
	HRESULT hr = 0;

	IMMDevice* defaultDevice = NULL;
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	return defaultDevice;
}
UINT32 EzAudioGetDevices(IMMDeviceEnumerator* deviceEnumerator, IMMDevice*** pDevices) {
	HRESULT hr = 0;

	IMMDeviceCollection* deviceCollection = NULL;
	hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &deviceCollection);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	UINT32 deviceCount = 0;
	hr = deviceCollection->GetCount(&deviceCount);
	if (FAILED(hr)) {
		deviceCollection->Release();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	IMMDevice** devices = new IMMDevice * [deviceCount];
	for (UINT32 i = 0; i < deviceCount; i++) {
		hr = deviceCollection->Item(i, &devices[i]);
		if (FAILED(hr)) {
			delete[] devices;
			deviceCollection->Release();
			EzError::ThrowFromHR(hr, __FILE__, __LINE__);
		}
	}

	deviceCollection->Release();
	if (pDevices == NULL) {
		for (UINT32 i = 0; i < deviceCount; i++) {
			devices[i]->Release();
		}
		delete[] devices;
	}
	else {
		*pDevices = devices;
	}
	return deviceCount;
}
LPWSTR EzAudioGetDeviceID(IMMDevice* device) {
	HRESULT hr = 0;

	LPWSTR deviceIDOriginal = NULL;
	hr = device->GetId(&deviceIDOriginal);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	LPWSTR deviceID = new WCHAR[lstrlen(deviceIDOriginal) + 1];
	lstrcpy(deviceID, deviceIDOriginal);

	CoTaskMemFree(deviceIDOriginal);
	return deviceID;
}
DEFINE_PROPERTYKEY(PKEY_Device_FriendlyName, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 14); // Taken from Functiondiscoverykeys_devpkey.h
LPWSTR EzAudioGetDeviceName(IMMDevice* device) {
	HRESULT hr = 0;

	IPropertyStore* propertyStore = NULL;
	hr = device->OpenPropertyStore(STGM_READ, &propertyStore);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	PROPVARIANT propVarient = { };
	PropVariantInit(&propVarient);
	hr = propertyStore->GetValue(PKEY_Device_FriendlyName, &propVarient);
	if (FAILED(hr)) {
		propertyStore->Release();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	propertyStore->Release();

	if (propVarient.vt != VT_LPWSTR) {
		PropVariantClear(&propVarient);
		throw EzError("The device friendly name was not a wide string.", __FILE__, __LINE__);
	}

	LPWSTR deviceName = new WCHAR[lstrlen(propVarient.pwszVal) + 1];
	lstrcpy(deviceName, propVarient.pwszVal);

	hr = PropVariantClear(&propVarient);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	return deviceName;
}

// Clients and WaveFormats
IAudioClient* EzAudioGetClient(IMMDevice* device) {
	HRESULT hr = 0;

	IAudioClient* client = NULL;
	hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&client));
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	return client;
}
WAVEFORMATEX* EzAudioGetClientMixFormat(IAudioClient* client) {
	HRESULT hr = 0;

	WAVEFORMATEX* formatOriginal = NULL;
	hr = client->GetMixFormat(&formatOriginal);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	WAVEFORMATEX* format = reinterpret_cast<WAVEFORMATEX*>(new BYTE[sizeof(WAVEFORMATEX) + formatOriginal->cbSize]);
	memcpy(format, formatOriginal, sizeof(WAVEFORMATEX) + formatOriginal->cbSize);

	CoTaskMemFree(formatOriginal);
	return format;
}
WAVEFORMATEX* EzAudioGetDeviceFormat(IAudioClient* client) {
	HRESULT hr = 0;

	constexpr DWORD sampleRates[] = { 44100, 48000, 96000, 192000, 32000, 22050 };
	constexpr WORD bitDepths[] = { 16, 24, 32, 64, 8 };
	constexpr WORD channels[] = { 2, 1, 6, 8 };

	WAVEFORMATEX* format = reinterpret_cast<WAVEFORMATEX*>(new BYTE[sizeof(WAVEFORMATEX)]);
	memset(format, 0, sizeof(WAVEFORMATEX));
	BOOL supported = FALSE;
	for (WORD channelCount : channels) {
		for (DWORD sampleRate : sampleRates) {
			for (WORD bitDepth : bitDepths) {
				format->wFormatTag = WAVE_FORMAT_PCM;
				format->nChannels = channelCount;
				format->nSamplesPerSec = sampleRate;
				format->wBitsPerSample = bitDepth;
				format->nBlockAlign = (format->wBitsPerSample * format->nChannels) / 8;
				format->nAvgBytesPerSec = format->nSamplesPerSec * format->nBlockAlign;

				if (EzAudioClientSupportsFormat(client, format, TRUE)) {
					goto foundSupportedFormat;
				}
			}
		}
	}
	delete[] format;
	throw EzError("Supplied audio client does not support any formats and is basically a hunk of garbage.", __FILE__, __LINE__);

foundSupportedFormat:
	return format;
}
WAVEFORMATEX* EzAudioGetAssetFormat(const EzAudioAsset* asset) {
	WAVEFORMATEX* format = reinterpret_cast<WAVEFORMATEX*>(new BYTE[sizeof(WAVEFORMATEX) + asset->ExtraDataLength]);
	format->wFormatTag = asset->FormatTag;
	format->nChannels = asset->ChannelCount;
	format->nSamplesPerSec = asset->SampleRate;
	format->wBitsPerSample = asset->BitsPerSample;
	if (asset->BlockAlign == 0) {
		format->nBlockAlign = (asset->BitsPerSample * asset->ChannelCount) / 8;
	}
	else {
		format->nBlockAlign = asset->BlockAlign;
	}
	if (asset->AverageBytesPerSecond == 0) {
		format->nAvgBytesPerSec = format->nBlockAlign * asset->SampleRate;
	}
	else {
		format->nAvgBytesPerSec = asset->AverageBytesPerSecond;
	}
	format->cbSize = asset->ExtraDataLength;
	if (asset->ExtraDataLength > 0) {
		memcpy(reinterpret_cast<BYTE*>(format) + sizeof(WAVEFORMATEX), asset->ExtraData, asset->ExtraDataLength);
	}
	return format;
}

// Contexts
void EzAudioInitClient(IAudioClient* client, const WAVEFORMATEX* format, BOOL exclusive)
{
	HRESULT hr = 0;

	if (exclusive) {
		REFERENCE_TIME defaultDevicePeriod = 0;
		hr = client->GetDevicePeriod(&defaultDevicePeriod, NULL);
		if (FAILED(hr)) {
			EzError::ThrowFromHR(hr, __FILE__, __LINE__);
		}
		hr = client->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE, 0, defaultDevicePeriod, defaultDevicePeriod, format, NULL);
		if (FAILED(hr)) {
			EzError::ThrowFromHR(hr, __FILE__, __LINE__);
		}
	}
	else {
		hr = client->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 5000000, 0, format, NULL);
		if (FAILED(hr)) {
			EzError::ThrowFromHR(hr, __FILE__, __LINE__);
		}
	}
}
IAudioRenderClient* EzAudioGetRenderer(IAudioClient* client) {
	HRESULT hr = 0;

	IAudioRenderClient* renderer = NULL;
	hr = client->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&renderer));
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	return renderer;
}
LONGLONG EzAudioStartClient(IAudioClient* client) {
	HRESULT hr = NULL;
	DWORD lastError = GetLastError();

	LONGLONG timeNow = { };
	LARGE_INTEGER* pTimeNow = reinterpret_cast<LARGE_INTEGER*>(&timeNow);
	BOOL success = FALSE;

	hr = client->Start();
	success = QueryPerformanceCounter(pTimeNow);

	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	if (!success) {
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	return timeNow;
}
BOOL EzAudioStopClientAtTime(IAudioClient* client, LONGLONG stopTime) {
	HRESULT hr = 0;
	DWORD lastError = 0;

	LONGLONG timeNow = 0;
	if (!QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&timeNow))) {
		lastError = GetLastError();
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	if (timeNow >= stopTime) {
		hr = client->Stop();
		if (FAILED(hr)) {
			EzError::ThrowFromHR(hr, __FILE__, __LINE__);
		}
		return FALSE; // Stopped playing
	}
	return TRUE; // Still playing
}
void EzAudioFillBuffer(IAudioClient* client, IAudioRenderClient* renderer, const WAVEFORMATEX* format, const BYTE* buffer, UINT32 bufferLength, UINT32* position, BOOL looping) {
	HRESULT hr = 0;

	if (*position > bufferLength) {
		throw EzError("position was out of bounds of buffer.", __FILE__, __LINE__);
	}

	UINT32 driverBufferTotalFrames = 0;
	hr = client->GetBufferSize(&driverBufferTotalFrames);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	UINT32 driverBufferPaddingFrames = 0;
	hr = client->GetCurrentPadding(&driverBufferPaddingFrames);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	UINT32 driverBufferFrames = driverBufferTotalFrames - driverBufferPaddingFrames;
	if (driverBufferFrames < 50) {
		return;
	}
	else {
		driverBufferFrames -= 50;
	}
	UINT32 driverBufferLength = driverBufferFrames * format->nBlockAlign;

	BYTE* driverBuffer = NULL;
	hr = renderer->GetBuffer(driverBufferFrames, &driverBuffer);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	UINT32 remainingBytesInBuffer = bufferLength - *position;

	if (looping) {
		if (remainingBytesInBuffer >= driverBufferLength)
		{
			memcpy(driverBuffer, buffer + *position, driverBufferLength);
			*position += driverBufferLength;
		}
		else {
			if (bufferLength < driverBufferLength) {
				for (UINT32 i = 0; i < driverBufferLength; i++) {
					driverBuffer[i] = buffer[(*position + i) % bufferLength];
				}
				*position = (*position + driverBufferLength) % bufferLength;
			}
			else {
				memcpy(driverBuffer, buffer + *position, remainingBytesInBuffer);
				memcpy(driverBuffer + remainingBytesInBuffer, buffer, driverBufferLength - remainingBytesInBuffer);
				*position = driverBufferLength - remainingBytesInBuffer;
			}
		}
	}
	else {
		if (*position >= bufferLength) {
			memset(driverBuffer, 0, driverBufferLength);
		}
		else if (remainingBytesInBuffer < driverBufferLength)
		{
			memcpy(driverBuffer, buffer + *position, remainingBytesInBuffer);
			memset(driverBuffer + remainingBytesInBuffer, 0, driverBufferLength - remainingBytesInBuffer);
			*position = bufferLength;
		}
		else {
			memcpy(driverBuffer, buffer + *position, driverBufferLength);
			*position += driverBufferLength;
		}
	}

	hr = renderer->ReleaseBuffer(driverBufferFrames, 0);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
}

// Transcoding
BYTE* EzAudioTranscode(const WAVEFORMATEX* inputFormat, const WAVEFORMATEX* outputFormat, const BYTE* inputBuffer, UINT32 inputLength, UINT32* pOutputLength) {
	HRESULT hr = 0;

	hr = MFStartup(MF_VERSION);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	IMFTransform* imfTransform = NULL;
	hr = CoCreateInstance(CLSID_CResamplerMediaObject, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&imfTransform));
	if (FAILED(hr)) {
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	IMFMediaType* imfInputType = NULL;
	hr = MFCreateMediaType(&imfInputType);
	if (FAILED(hr)) {
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	hr = MFInitMediaTypeFromWaveFormatEx(imfInputType, inputFormat, sizeof(WAVEFORMATEX) + inputFormat->cbSize);
	if (FAILED(hr)) {
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	hr = imfTransform->SetInputType(0, imfInputType, 0);
	if (FAILED(hr)) {
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	IMFMediaType* imfOutputType = NULL;
	hr = MFCreateMediaType(&imfOutputType);
	if (FAILED(hr)) {
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	hr = MFInitMediaTypeFromWaveFormatEx(imfOutputType, outputFormat, sizeof(WAVEFORMATEX) + outputFormat->cbSize);
	if (FAILED(hr)) {
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	hr = imfTransform->SetOutputType(0, imfOutputType, 0);
	if (FAILED(hr)) {
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	IMFMediaBuffer* imfInputBuffer = NULL;
	BYTE* internalInputBuffer = NULL;
	hr = MFCreateMemoryBuffer(inputLength, &imfInputBuffer);
	if (FAILED(hr)) {
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	hr = imfInputBuffer->Lock(&internalInputBuffer, NULL, NULL);
	if (FAILED(hr)) {
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	memcpy(internalInputBuffer, inputBuffer, inputLength);
	hr = imfInputBuffer->Unlock();
	if (FAILED(hr)) {
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	hr = imfInputBuffer->SetCurrentLength(inputLength);
	if (FAILED(hr)) {
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	IMFSample* imfInputSample = NULL;
	hr = MFCreateSample(&imfInputSample);
	if (FAILED(hr)) {
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	hr = imfInputSample->AddBuffer(imfInputBuffer);
	if (FAILED(hr)) {
		imfInputSample->Release();
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	hr = imfTransform->ProcessInput(0, imfInputSample, 0);
	if (FAILED(hr)) {
		imfInputSample->Release();
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	if ((inputFormat->nChannels * inputFormat->wBitsPerSample) / 8 != inputFormat->nBlockAlign || (inputFormat->nChannels * inputFormat->wBitsPerSample) % 8 != 0) {
		imfInputSample->Release();
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		throw EzError("inputFormat->nBlockAlign was wrong.", __FILE__, __LINE__);
	}
	if ((outputFormat->nChannels * outputFormat->wBitsPerSample) / 8 != outputFormat->nBlockAlign || (outputFormat->nChannels * outputFormat->wBitsPerSample) % 8 != 0) {
		imfInputSample->Release();
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		throw EzError("outputFormat->nBlockAlign was wrong.", __FILE__, __LINE__);
	}
	UINT32 computedOutputLength = (static_cast<UINT64>(inputLength) * outputFormat->nSamplesPerSec * outputFormat->nBlockAlign) / (static_cast<UINT64>(inputFormat->nSamplesPerSec) * inputFormat->nBlockAlign);
	UINT32 maxOutputBufferCapacity = computedOutputLength + (computedOutputLength / 10);

	IMFMediaBuffer* imfOutputBuffer = NULL;
	hr = MFCreateMemoryBuffer(maxOutputBufferCapacity, &imfOutputBuffer);
	if (FAILED(hr)) {
		imfInputSample->Release();
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	IMFSample* imfOutputSample = NULL;
	hr = MFCreateSample(&imfOutputSample);
	if (FAILED(hr)) {
		imfOutputBuffer->Release();
		imfInputSample->Release();
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	hr = imfOutputSample->AddBuffer(imfOutputBuffer);
	if (FAILED(hr)) {
		imfOutputSample->Release();
		imfOutputBuffer->Release();
		imfInputSample->Release();
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	MFT_OUTPUT_DATA_BUFFER outputDataBuffer = { };
	outputDataBuffer.dwStreamID = 0;
	outputDataBuffer.pSample = imfOutputSample;
	outputDataBuffer.dwStatus = 0;
	outputDataBuffer.pEvents = NULL;
	DWORD status = 0;
	hr = imfTransform->ProcessOutput(0, 1, &outputDataBuffer, &status);
	if (FAILED(hr)) {
		imfOutputSample->Release();
		imfOutputBuffer->Release();
		imfInputSample->Release();
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	if (status != 0) {
		imfOutputSample->Release();
		imfOutputBuffer->Release();
		imfInputSample->Release();
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		throw EzError("Invalid status returned from IMFTransform::ProcessOutput().", __FILE__, __LINE__);
	}

	UINT32 outputLength = 0;
	hr = imfOutputBuffer->GetCurrentLength(reinterpret_cast<DWORD*>(&outputLength));
	if (FAILED(hr)) {
		imfOutputSample->Release();
		imfOutputBuffer->Release();
		imfInputSample->Release();
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	if (outputLength >= maxOutputBufferCapacity) {
		imfOutputSample->Release();
		imfOutputBuffer->Release();
		imfInputSample->Release();
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		throw EzError("Output data overflowed allocated buffer.", __FILE__, __LINE__);
	}

	BYTE* outputBuffer = new BYTE[outputLength];
	BYTE* internalOutputBuffer = NULL;
	hr = imfOutputBuffer->Lock(&internalOutputBuffer, NULL, NULL);
	if (FAILED(hr)) {
		imfOutputSample->Release();
		imfOutputBuffer->Release();
		imfInputSample->Release();
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
	memcpy(outputBuffer, internalOutputBuffer, outputLength);
	hr = imfOutputBuffer->Unlock();
	if (FAILED(hr)) {
		imfOutputSample->Release();
		imfOutputBuffer->Release();
		imfInputSample->Release();
		imfInputBuffer->Release();
		imfOutputType->Release();
		imfInputType->Release();
		imfTransform->Release();
		MFShutdown();
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	imfOutputSample->Release();
	imfOutputBuffer->Release();
	imfInputSample->Release();
	imfInputBuffer->Release();
	imfOutputType->Release();
	imfInputType->Release();
	imfTransform->Release();
	MFShutdown();
	if (pOutputLength != NULL) {
		*pOutputLength = outputLength;
	}
	return outputBuffer;
}

// Volume and Mute
IAudioEndpointVolume* EzAudioGetVolumeController(IMMDevice* device) {
	HRESULT hr = 0;

	IAudioEndpointVolume* volumeController = NULL;
	hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&volumeController));
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	return volumeController;
}
FLOAT EzAudioGetVolume(IAudioEndpointVolume* volumeController) {
	HRESULT hr = 0;

	FLOAT volume = 0.0f;
	hr = volumeController->GetMasterVolumeLevelScalar(&volume);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	return volume;
}
void EzAudioSetVolume(IAudioEndpointVolume* volumeController, FLOAT volume) {
	HRESULT hr = 0;

	hr = volumeController->SetMasterVolumeLevelScalar(volume, NULL);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
}
BOOL EzAudioGetMute(IAudioEndpointVolume* volumeController) {
	HRESULT hr = 0;

	BOOL mute = FALSE;
	hr = volumeController->GetMute(&mute);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}

	return mute;
}
void EzAudioSetMute(IAudioEndpointVolume* volumeController, BOOL mute) {
	HRESULT hr = 0;

	hr = volumeController->SetMute(mute, NULL);
	if (FAILED(hr)) {
		EzError::ThrowFromHR(hr, __FILE__, __LINE__);
	}
}