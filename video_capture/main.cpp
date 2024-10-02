#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <Mfreadwrite.h>
#include <mferror.h>
#include <iostream>
#include <string>

#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "Mf")

IMFAttributes* pConfig = NULL;

void DebugShowDeviceNames(IMFActivate** ppDevices, UINT count)
{
    for (DWORD i = 0; i < count; i++)
    {
        HRESULT hr = S_OK;
        WCHAR* szFriendlyName = NULL;

        // Try to get the display name.
        UINT32 cchName;
        hr = ppDevices[i]->GetAllocatedString(
            MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME,
            &szFriendlyName, &cchName);

        if (SUCCEEDED(hr))
        {
            std::wstring str(szFriendlyName, cchName);
            std::wcout << str << std::endl;
            OutputDebugString(szFriendlyName);
            OutputDebugString(L"\n");
        }
        CoTaskMemFree(szFriendlyName);
    }
}

HRESULT CreateVideoCaptureDevice(IMFMediaSource** ppSource)
{
    *ppSource = NULL;

    UINT32 count = 0;

    IMFActivate** ppDevices = NULL;

    // Create an attribute store to hold the search criteria.
    HRESULT hr = MFCreateAttributes(&pConfig, 1);

    // Request video capture devices.
    if (SUCCEEDED(hr))
    {
        hr = pConfig->SetGUID(
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
        );
    }

    // Enumerate the devices,
    if (SUCCEEDED(hr))
    {
        hr = MFEnumDeviceSources(pConfig, &ppDevices, &count);
    }

    DebugShowDeviceNames(ppDevices, count);

    // Create a media source for the first device in the list.
    if (SUCCEEDED(hr))
    {
        if (count > 0)
        {
            hr = ppDevices[0]->ActivateObject(IID_PPV_ARGS(ppSource));
        }
        else
        {
            hr = MF_E_NOT_FOUND;
        }
    }

    for (DWORD i = 0; i < count; i++)
    {
        ppDevices[i]->Release();
    }
    CoTaskMemFree(ppDevices);
    return hr;
}

HRESULT CreateVideoSourceReader(IMFMediaSource** ppSource, IMFSourceReader** reader)
{
    *reader = NULL;
    HRESULT hr = MFCreateSourceReaderFromMediaSource(
        *ppSource,
        pConfig,
        reader);
    if (!SUCCEEDED(hr))
    {
        return hr;
    }
    //设置 Media Type
    IMFMediaType* mediaType = NULL;
    MFCreateMediaType(&mediaType);
    //设置媒体为视频
    mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    //YUV格式为 I420
    //https://learn.microsoft.com/zh-cn/windows/win32/medfound/video-subtype-guids
    mediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_I420);
    //每个视频帧的大小为 640 * 480
    MFSetAttributeSize(mediaType, MF_MT_FRAME_SIZE, 640, 480);
    (*reader)->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM,
        NULL,
        mediaType);

    //读取数据
    IMFSample* sample = NULL;
    DWORD index, flags;
    LONGLONG llVideoTs;
    bool running = true;
    while (running) {
        HRESULT ret = (*reader)->ReadSample(
            MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            MF_SOURCE_READER_CONTROLF_DRAIN,
            &index, //实际流的index
            &flags, //staus flags
            &llVideoTs, //时间戳
            &sample); //存放采集到的视频数据
        //std::cout << sample->Release();
        int a = 1;
        if (sample != NULL)
        {
            std::cout << sample->Release();
        }
    }

    return hr;
}

void main()
{
    // Initialize the COM runtime.
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (SUCCEEDED(hr))
    {
        // Initialize the Media Foundation platform.
        hr = MFStartup(MF_VERSION);
        if (SUCCEEDED(hr))
        {
            IMFMediaSource* source;
            CreateVideoCaptureDevice(&source);
            IMFSourceReader* videoReader;
            CreateVideoSourceReader(&source, &videoReader);
            MFShutdown();
        }
        CoUninitialize();
    }
}