/*
Known Issue: IAudioClient::GetMixFormat() returns the default format used by the audio mixer.
It does not return the default audio format for the underlying hardware. This means that the format
returned by IAudioClient::GetMixFormat() is guaranteed to work in shared mode but may not work in
exclusive mode if the hardware does not support it.
*/
#include "EzAudioPlayer.h"
#include "EzError.h"
#include <endpointvolume.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <mftransform.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <wmcodecdsp.h>
#include <combaseapi.h>
#include <memory>

#include <iostream>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")

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
void ManualMemSet(BYTE* destination, BYTE value, UINT32 length) {
	for (UINT32 i = 0; i < length; i++)
	{
		destination[i] = value;
	}
}
void ManualMemCopy(BYTE* destination, const BYTE* source, UINT32 length) {
	for (UINT32 i = 0; i < length; i++)
	{
		destination[i] = source[i];
	}
}
IMMDevice* EzGetDefaultAudioDevice() {
	IMMDeviceEnumerator* audioDeviceEnumerator = NULL;
	EzError::ThrowFromHR(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&audioDeviceEnumerator)), __FILE__, __LINE__);

	IMMDevice* defaultDevice = NULL;
	EzError::ThrowFromHR(audioDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice), __FILE__, __LINE__);

	audioDeviceEnumerator->Release();
	return defaultDevice;
}
WAVEFORMATEX* EzGetAudioMixFormat(IMMDevice* device) {
	IAudioClient* audioClient = NULL;
	EzError::ThrowFromHR(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioClient)), __FILE__, __LINE__);

	WAVEFORMATEX* waveFormatOriginal = NULL;

	EzError::ThrowFromHR(audioClient->GetMixFormat(&waveFormatOriginal), __FILE__, __LINE__);

	WAVEFORMATEX* waveFormat = reinterpret_cast<WAVEFORMATEX*>(new BYTE[sizeof(WAVEFORMATEX) + waveFormatOriginal->cbSize]);
	memcpy(waveFormat, waveFormatOriginal, sizeof(WAVEFORMATEX) + waveFormatOriginal->cbSize);

	audioClient->Release();
	return waveFormat;
}
WAVEFORMATEX* EzGetAudioDeviceFormat(IMMDevice* device) {
	constexpr DWORD sampleRates[] = { 44100, 48000, 96000, 192000, 32000, 22050 };
	constexpr WORD bitDepths[] = { 16, 24, 32, 64, 8 };
	constexpr WORD channels[] = { 2, 1, 6, 8 };

	IAudioClient* audioClient = NULL;
	EzError::ThrowFromHR(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioClient)), __FILE__, __LINE__);

	WAVEFORMATEX* waveFormat = reinterpret_cast<WAVEFORMATEX*>(new BYTE[sizeof(WAVEFORMATEX)]);
	for (WORD channelCount : channels) {
		for (DWORD sampleRate : sampleRates) {
			for (WORD bitDepth : bitDepths) {
				waveFormat->wFormatTag = WAVE_FORMAT_PCM;
				waveFormat->nChannels = channelCount;
				waveFormat->nSamplesPerSec = sampleRate;
				waveFormat->wBitsPerSample = bitDepth;
				waveFormat->nBlockAlign = (waveFormat->wBitsPerSample * waveFormat->nChannels) / 8;
				waveFormat->nAvgBytesPerSec = waveFormat->nSamplesPerSec * waveFormat->nBlockAlign;
				waveFormat->cbSize = 0;

				if (audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, waveFormat, NULL) == S_OK) {
					goto foundSupportedFormat;
				}
			}
		}
	}
	throw EzError::EzError(L"Supplied audio device does not support any formats and is basically a hunk of garbage.", __FILE__, __LINE__);

foundSupportedFormat:
	audioClient->Release();
	return waveFormat;
}
UINT32 EzGetAudioDevices(IMMDevice*** pDevices) {
	IMMDeviceEnumerator* audioDeviceEnumerator = NULL;
	EzError::ThrowFromHR(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&audioDeviceEnumerator)), __FILE__, __LINE__);

	IMMDeviceCollection* audioDeviceCollection = NULL;
	EzError::ThrowFromHR(audioDeviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &audioDeviceCollection), __FILE__, __LINE__);

	UINT32 deviceCount = 0;
	EzError::ThrowFromHR(audioDeviceCollection->GetCount(&deviceCount), __FILE__, __LINE__);

	IMMDevice** devices = new IMMDevice * [deviceCount];
	for (UINT32 i = 0; i < deviceCount; i++) {
		EzError::ThrowFromHR(audioDeviceCollection->Item(i, &devices[i]), __FILE__, __LINE__);
	}

	audioDeviceCollection->Release();
	audioDeviceEnumerator->Release();
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
LPWSTR EzGetAudioDeviceId(IMMDevice* device) {
	LPWSTR outputOriginal = NULL;
	EzError::ThrowFromHR(device->GetId(&outputOriginal), __FILE__, __LINE__);

	LPWSTR output = new WCHAR[lstrlen(outputOriginal) + 1];
	lstrcpy(output, outputOriginal);

	CoTaskMemFree(outputOriginal);
	return output;
}
// Taken from Functiondiscoverykeys_devpkey.h
DEFINE_PROPERTYKEY(PKEY_Device_FriendlyName, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 14); // DEVPROP_TYPE_STRING
LPWSTR EzGetAudioDeviceName(IMMDevice* device) {
	IPropertyStore* propertyStore = NULL;
	EzError::ThrowFromHR(device->OpenPropertyStore(STGM_READ, &propertyStore), __FILE__, __LINE__);

	PROPVARIANT propVar = { };
	PropVariantInit(&propVar);
	EzError::ThrowFromHR(propertyStore->GetValue(PKEY_Device_FriendlyName, &propVar), __FILE__, __LINE__);

	if (propVar.vt != VT_LPWSTR) {
		throw EzError(L"The device friendly name was not a wide string.", __FILE__, __LINE__);
	}

	LPWSTR output = new WCHAR[lstrlen(propVar.pwszVal) + 1];
	lstrcpy(output, propVar.pwszVal);

	EzError::ThrowFromHR(PropVariantClear(&propVar), __FILE__, __LINE__);
	propertyStore->Release();
	return output;
}
WAVEFORMATEX* EzGetAssetWaveFormat(const EzAudioAsset* asset) {
	WAVEFORMATEX* waveFormat = reinterpret_cast<WAVEFORMATEX*>(new BYTE[sizeof(WAVEFORMATEX) + asset->ExtraDataLength]);
	waveFormat->wFormatTag = asset->FormatTag;
	waveFormat->nChannels = asset->ChannelCount;
	waveFormat->nSamplesPerSec = asset->SampleRate;
	waveFormat->wBitsPerSample = asset->BitsPerSample;
	if (asset->BlockAlign == 0) {
		waveFormat->nBlockAlign = (asset->BitsPerSample * asset->ChannelCount) / 8;
	}
	else {
		waveFormat->nBlockAlign = asset->BlockAlign;
	}
	if (asset->AverageBytesPerSecond == 0) {
		waveFormat->nAvgBytesPerSec = waveFormat->nBlockAlign * asset->SampleRate;
	}
	else {
		waveFormat->nAvgBytesPerSec = asset->AverageBytesPerSecond;
	}
	waveFormat->cbSize = asset->ExtraDataLength;
	if (asset->ExtraDataLength > 0) {
		memcpy(reinterpret_cast<BYTE*>(waveFormat) + sizeof(WAVEFORMATEX), asset->ExtraData, asset->ExtraDataLength);
	}

	return waveFormat;
}
void EzCreateAudioContext(IMMDevice* device, const EzAudioAsset* asset, BOOL exclusive, IAudioClient** pAudioClient, IAudioRenderClient** pAudioRenderClient) {
	WAVEFORMATEX* waveFormat = EzGetAssetWaveFormat(asset);

	EzCreateAudioContext(device, waveFormat, TRUE, pAudioClient, pAudioRenderClient);

	delete[] waveFormat;
}
void EzCreateAudioContext(IMMDevice* device, const WAVEFORMATEX* waveFormat, BOOL exclusive, IAudioClient** pAudioClient, IAudioRenderClient** pAudioRenderClient)
{
	IAudioClient* audioClient = NULL;
	EzError::ThrowFromHR(device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioClient)), __FILE__, __LINE__);

	if (exclusive) {
		if (audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_EXCLUSIVE, waveFormat, NULL) != S_OK) {
			throw EzError(L"The selected audio device does not support the wave format of the supplied audio asset.", __FILE__, __LINE__);
		}
		REFERENCE_TIME defaultDevicePeriod = 0;
		EzError::ThrowFromHR(audioClient->GetDevicePeriod(&defaultDevicePeriod, NULL), __FILE__, __LINE__);
		EzError::ThrowFromHR(audioClient->Initialize(AUDCLNT_SHAREMODE_EXCLUSIVE, 0, defaultDevicePeriod, defaultDevicePeriod, waveFormat, NULL), __FILE__, __LINE__);
	}
	else {
		WAVEFORMATEX* closestMatch = NULL;
		if (audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, waveFormat, &closestMatch) != S_OK) {
			throw EzError(L"The selected audio device does not support the wave format of the supplied audio asset.", __FILE__, __LINE__);
		}
		CoTaskMemFree(closestMatch);
		EzError::ThrowFromHR(audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, 5000000, 0, waveFormat, NULL), __FILE__, __LINE__);
	}

	IAudioRenderClient* audioRenderClient = NULL;
	EzError::ThrowFromHR(audioClient->GetService(__uuidof(IAudioRenderClient), reinterpret_cast<void**>(&audioRenderClient)), __FILE__, __LINE__);

	if (pAudioClient == NULL) {
		audioClient->Release();
	}
	else {
		*pAudioClient = audioClient;
	}
	if (pAudioRenderClient == NULL) {
		audioRenderClient->Release();
	}
	else {
		*pAudioRenderClient = audioRenderClient;
	}
}
LONGLONG EzStartAudioPlayer(IAudioClient* audioClient) {
	LARGE_INTEGER timeNow = { };
	LARGE_INTEGER* pTimeNow = &timeNow;

	HRESULT startHR = audioClient->Start();
	BOOL queryPCResult = QueryPerformanceCounter(pTimeNow);
	DWORD lastError = GetLastError();

	EzError::ThrowFromHR(startHR, __FILE__, __LINE__);
	if (!queryPCResult) {
		EzError::ThrowFromCode(lastError, __FILE__, __LINE__);
	}

	return timeNow.QuadPart;
}
BOOL EzStopAudioAtTime(IAudioClient* audioClient, LONGLONG stopTime) {
	LARGE_INTEGER timeNow = { };
	if (!QueryPerformanceCounter(&timeNow)) {
		EzError::ThrowFromCode(GetLastError(), __FILE__, __LINE__);
	}

	if (timeNow.QuadPart >= stopTime) {
		audioClient->Stop();
		return FALSE; // Stopped playing
	}
	return TRUE; // Still playing
}
void EzFillAudioBuffer(IAudioClient* audioClient, IAudioRenderClient* audioRenderClient, const EzAudioAsset* asset, UINT32* position, BOOL looping) {
	WAVEFORMATEX* waveFormat = EzGetAssetWaveFormat(asset);
	EzFillAudioBuffer(audioClient, audioRenderClient, waveFormat, asset->Buffer, asset->BufferLength, position, looping);
	delete[] waveFormat;
}
void EzFillAudioBuffer(IAudioClient* audioClient, IAudioRenderClient* audioRenderClient, const WAVEFORMATEX* waveFormat, const BYTE* buffer, UINT32 bufferLength, UINT32* position, BOOL looping) {
	if (*position > bufferLength) {
		throw EzError(L"position was out of bounds of buffer.", __FILE__, __LINE__);
	}

	UINT32 driverBufferTotalFrames = 0;
	EzError::ThrowFromHR(audioClient->GetBufferSize(&driverBufferTotalFrames), __FILE__, __LINE__);
	UINT32 driverBufferPaddingFrames = 0;
	EzError::ThrowFromHR(audioClient->GetCurrentPadding(&driverBufferPaddingFrames), __FILE__, __LINE__);

	//for (volatile int i = 0; i < 100000; ++i) { /* do nothing */ }

	UINT32 driverBufferFrames = driverBufferTotalFrames - driverBufferPaddingFrames;
	UINT32 driverBufferLength = driverBufferFrames * waveFormat->nBlockAlign;

	BYTE* driverBuffer = NULL;
	EzError::ThrowFromHR(audioRenderClient->GetBuffer(driverBufferFrames, &driverBuffer), __FILE__, __LINE__);

	UINT32 remainingBytesInBuffer = bufferLength - *position;

	if (looping) {
		if (remainingBytesInBuffer >= driverBufferLength)
		{
			ManualMemCopy(driverBuffer, buffer + *position, driverBufferLength);
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
				ManualMemCopy(driverBuffer, buffer + *position, remainingBytesInBuffer);
				ManualMemCopy(driverBuffer + remainingBytesInBuffer, buffer, driverBufferLength - remainingBytesInBuffer);
				*position = driverBufferLength - remainingBytesInBuffer;
			}
		}
	}
	else {
		if (*position >= bufferLength) {
			ManualMemSet(driverBuffer, 0, driverBufferLength);
		}
		else if (remainingBytesInBuffer < driverBufferLength)
		{
			ManualMemCopy(driverBuffer, buffer + *position, remainingBytesInBuffer);
			ManualMemSet(driverBuffer + remainingBytesInBuffer, 0, driverBufferLength - remainingBytesInBuffer);
			*position = bufferLength;
		}
		else {
			ManualMemCopy(driverBuffer, buffer + *position, driverBufferLength);
			*position += driverBufferLength;
		}
	}

	EzError::ThrowFromHR(audioRenderClient->ReleaseBuffer(driverBufferFrames, 0), __FILE__, __LINE__);
}
BYTE* EzTranscodeAudio(const WAVEFORMATEX* inputFormat, const WAVEFORMATEX* outputFormat, const BYTE* inputBuffer, UINT32 inputLength, UINT32* pOutputLength) {
	EzError::ThrowFromHR(MFStartup(MF_VERSION), __FILE__, __LINE__);

	IMFTransform* imfTransform = NULL;
	EzError::ThrowFromHR(CoCreateInstance(CLSID_CResamplerMediaObject, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&imfTransform)), __FILE__, __LINE__);

	IMFMediaType* imfInputType = NULL;
	EzError::ThrowFromHR(MFCreateMediaType(&imfInputType), __FILE__, __LINE__);
	EzError::ThrowFromHR(MFInitMediaTypeFromWaveFormatEx(imfInputType, inputFormat, sizeof(WAVEFORMATEX) + inputFormat->cbSize), __FILE__, __LINE__);
	EzError::ThrowFromHR(imfTransform->SetInputType(0, imfInputType, 0), __FILE__, __LINE__);

	IMFMediaType* imfOutputType = NULL;
	EzError::ThrowFromHR(MFCreateMediaType(&imfOutputType), __FILE__, __LINE__);
	EzError::ThrowFromHR(MFInitMediaTypeFromWaveFormatEx(imfOutputType, outputFormat, sizeof(WAVEFORMATEX) + outputFormat->cbSize), __FILE__, __LINE__);
	EzError::ThrowFromHR(imfTransform->SetOutputType(0, imfOutputType, 0), __FILE__, __LINE__);

	IMFMediaBuffer* imfInputBuffer = NULL;
	BYTE* internalInputBuffer = NULL;
	EzError::ThrowFromHR(MFCreateMemoryBuffer(inputLength, &imfInputBuffer), __FILE__, __LINE__);
	EzError::ThrowFromHR(imfInputBuffer->Lock(&internalInputBuffer, NULL, NULL), __FILE__, __LINE__);
	memcpy(internalInputBuffer, inputBuffer, inputLength);
	EzError::ThrowFromHR(imfInputBuffer->Unlock(), __FILE__, __LINE__);
	EzError::ThrowFromHR(imfInputBuffer->SetCurrentLength(inputLength), __FILE__, __LINE__);

	IMFSample* imfInputSample = NULL;
	EzError::ThrowFromHR(MFCreateSample(&imfInputSample), __FILE__, __LINE__);
	EzError::ThrowFromHR(imfInputSample->AddBuffer(imfInputBuffer), __FILE__, __LINE__);

	EzError::ThrowFromHR(imfTransform->ProcessInput(0, imfInputSample, 0), __FILE__, __LINE__);

	if ((inputFormat->nChannels * inputFormat->wBitsPerSample) / 8 != inputFormat->nBlockAlign || (inputFormat->nChannels * inputFormat->wBitsPerSample) % 8 != 0) {
		throw EzError(L"inputFormat->nBlockAlign was wrong.", __FILE__, __LINE__);
	}
	if ((outputFormat->nChannels * outputFormat->wBitsPerSample) / 8 != outputFormat->nBlockAlign || (outputFormat->nChannels * outputFormat->wBitsPerSample) % 8 != 0) {
		throw EzError(L"outputFormat->nBlockAlign was wrong.", __FILE__, __LINE__);
	}
	UINT32 computedOutputLength = (static_cast<UINT64>(inputLength) * outputFormat->nSamplesPerSec * outputFormat->nBlockAlign) / (static_cast<UINT64>(inputFormat->nSamplesPerSec) * inputFormat->nBlockAlign);
	UINT32 maxOutputBufferCapacity = computedOutputLength + (computedOutputLength / 10);

	IMFMediaBuffer* imfOutputBuffer = NULL;
	EzError::ThrowFromHR(MFCreateMemoryBuffer(maxOutputBufferCapacity, &imfOutputBuffer), __FILE__, __LINE__);

	IMFSample* imfOutputSample = NULL;
	EzError::ThrowFromHR(MFCreateSample(&imfOutputSample), __FILE__, __LINE__);
	EzError::ThrowFromHR(imfOutputSample->AddBuffer(imfOutputBuffer), __FILE__, __LINE__);

	MFT_OUTPUT_DATA_BUFFER outputDataBuffer = { };
	outputDataBuffer.dwStreamID = 0;
	outputDataBuffer.pSample = imfOutputSample;
	outputDataBuffer.dwStatus = 0;
	outputDataBuffer.pEvents = NULL;
	DWORD status = 0;
	EzError::ThrowFromHR(imfTransform->ProcessOutput(0, 1, &outputDataBuffer, &status), __FILE__, __LINE__);
	if (status != 0) {
		throw EzError(L"Invalid status returned from IMFTransform::ProcessOutput().", __FILE__, __LINE__);
	}

	UINT32 outputLength = 0;
	EzError::ThrowFromHR(imfOutputBuffer->GetCurrentLength(reinterpret_cast<DWORD*>(&outputLength)), __FILE__, __LINE__);

	if (outputLength >= maxOutputBufferCapacity) {
		throw EzError(L"Output data overflowed allocated buffer.", __FILE__, __LINE__);
	}

	BYTE* outputBuffer = new BYTE[outputLength];
	BYTE* internalOutputBuffer = NULL;
	EzError::ThrowFromHR(imfOutputBuffer->Lock(&internalOutputBuffer, NULL, NULL), __FILE__, __LINE__);
	memcpy(outputBuffer, internalOutputBuffer, outputLength);
	EzError::ThrowFromHR(imfOutputBuffer->Unlock(), __FILE__, __LINE__);

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
BOOL EzGetMute(IMMDevice* device) {
	IAudioEndpointVolume* audioEndpointVolume = NULL;
	EzError::ThrowFromHR(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioEndpointVolume)), __FILE__, __LINE__);

	BOOL output = FALSE;
	EzError::ThrowFromHR(audioEndpointVolume->GetMute(&output), __FILE__, __LINE__);

	audioEndpointVolume->Release();
	return output;
}
void EzSetMute(IMMDevice* device, BOOL mute) {
	IAudioEndpointVolume* audioEndpointVolume = NULL;
	EzError::ThrowFromHR(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioEndpointVolume)), __FILE__, __LINE__);

	EzError::ThrowFromHR(audioEndpointVolume->SetMute(mute, NULL), __FILE__, __LINE__);

	audioEndpointVolume->Release();
}
FLOAT EzGetVolume(IMMDevice* device) {
	IAudioEndpointVolume* audioEndpointVolume = NULL;
	EzError::ThrowFromHR(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioEndpointVolume)), __FILE__, __LINE__);

	FLOAT output = 0.0f;
	EzError::ThrowFromHR(audioEndpointVolume->GetMasterVolumeLevelScalar(&output), __FILE__, __LINE__);

	audioEndpointVolume->Release();
	return output;
}
void EzSetVolume(IMMDevice* device, FLOAT volume) {
	IAudioEndpointVolume* audioEndpointVolume = NULL;
	EzError::ThrowFromHR(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, reinterpret_cast<void**>(&audioEndpointVolume)), __FILE__, __LINE__);

	EzError::ThrowFromHR(audioEndpointVolume->SetMasterVolumeLevelScalar(volume, NULL), __FILE__, __LINE__);

	audioEndpointVolume->Release();
}