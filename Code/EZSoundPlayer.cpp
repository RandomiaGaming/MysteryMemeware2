#include <windows.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <iostream>
#include <fstream>
#include <vector>

HRESULT PlayWAVExclusive(const std::wstring& filename)
{
    HRESULT hr;
    IMMDeviceEnumerator* pEnumerator = NULL;
    IMMDevice* pDevice = NULL;
    IAudioClient* pAudioClient = NULL;
    IAudioRenderClient* pRenderClient = NULL;
    WAVEFORMATEX* pwfx = NULL;
    UINT32 bufferFrameCount;
    BYTE* pData;
    DWORD flags = 0;

    CoInitialize(NULL);

    // Get the default audio device
    hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL,
        CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
        (void**)&pEnumerator);
    EXIT_ON_ERROR(hr);

    hr = pEnumerator->GetDefaultAudioEndpoint(
        eRender, eConsole, &pDevice);
    EXIT_ON_ERROR(hr);

    // Activate an audio client
    hr = pDevice->Activate(
        __uuidof(IAudioClient), CLSCTX_ALL,
        NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr);

    // Get the device format
    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr);

    // Initialize the audio client
    hr = pAudioClient->Initialize(
        AUDCLNT_SHAREMODE_EXCLUSIVE,
        0,
        10000000,  // 1 second buffer
        0,
        pwfx,
        NULL);
    EXIT_ON_ERROR(hr);

    // Get the render client
    hr = pAudioClient->GetService(
        __uuidof(IAudioRenderClient), (void**)&pRenderClient);
    EXIT_ON_ERROR(hr);

    // Get the buffer size
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr);

    // Load the WAV file
    std::ifstream file(filename, std::ios::binary);
    std::vector<BYTE> wavData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Play the audio
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    BYTE* pWaveData = wavData.data();
    size_t waveDataSize = wavData.size();
    size_t offset = 0;

    hr = pAudioClient->Start();  // Start playing.
    EXIT_ON_ERROR(hr);

    while (offset < waveDataSize)
    {
        hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
        EXIT_ON_ERROR(hr);

        numFramesAvailable = bufferFrameCount - numFramesPadding;

        hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
        EXIT_ON_ERROR(hr);

        size_t bytesToWrite = numFramesAvailable * pwfx->nBlockAlign;
        if (bytesToWrite > (waveDataSize - offset))
        {
            bytesToWrite = waveDataSize - offset;
        }

        memcpy(pData, pWaveData + offset, bytesToWrite);
        offset += bytesToWrite;

        hr = pRenderClient->ReleaseBuffer(bytesToWrite / pwfx->nBlockAlign, flags);
        EXIT_ON_ERROR(hr);

        if (offset >= waveDataSize)
        {
            flags |= AUDCLNT_BUFFERFLAGS_SILENT;
        }
    }

    hr = pAudioClient->Stop();  // Stop playing.
    EXIT_ON_ERROR(hr);

Exit:
    SAFE_RELEASE(pEnumerator);
    SAFE_RELEASE(pDevice);
    SAFE_RELEASE(pAudioClient);
    SAFE_RELEASE(pRenderClient);
    CoTaskMemFree(pwfx);
    CoUninitialize();

    return hr;
}

int main2()
{
    std::wstring filename = L"D:\Coding\C++\MysteryMemeware2\Song.wav";
    HRESULT hr = PlayWAVExclusive(filename);
    if (FAILED(hr))
    {
        std::cerr << "Failed to play WAV file." << std::endl;
    }
    return 0;
}