#include "debug_info.h"
#include <Mfreadwrite.h>
#include <iostream>

#pragma comment(lib, "mfreadwrite")
#pragma comment(lib, "mfplat")
#pragma comment(lib, "mfuuid")
#pragma comment(lib, "Mf")

template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

IMFAttributes* pAttri = NULL;

HRESULT CreateVideoCaptureDevice(IMFMediaSource** ppSource)
{
    *ppSource = NULL;

    UINT32 count = 0;

    IMFActivate** ppDevices = NULL;

    // Create an attribute store to hold the search criteria.
    HRESULT hr = MFCreateAttributes(&pAttri, 1);

    // Request video capture devices.
    if (SUCCEEDED(hr))
    {
        hr = pAttri->SetGUID(
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
            MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID
        );
    }

    // Enumerate the devices,
    if (SUCCEEDED(hr))
    {
        hr = MFEnumDeviceSources(pAttri, &ppDevices, &count);
    }

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

HRESULT EnumerateCaptureFormats(IMFMediaSource* pSource)
{
    IMFPresentationDescriptor* pPD = NULL;
    IMFStreamDescriptor* pSD = NULL;
    IMFMediaTypeHandler* pHandler = NULL;
    IMFMediaType* pType = NULL;

    HRESULT hr = pSource->CreatePresentationDescriptor(&pPD);
    if (FAILED(hr))
    {
        goto done;
    }

    BOOL fSelected;
    hr = pPD->GetStreamDescriptorByIndex(0, &fSelected, &pSD);
    if (FAILED(hr))
    {
        goto done;
    }

    hr = pSD->GetMediaTypeHandler(&pHandler);
    if (FAILED(hr))
    {
        goto done;
    }

    DWORD cTypes = 0;
    hr = pHandler->GetMediaTypeCount(&cTypes);
    if (FAILED(hr))
    {
        goto done;
    }

    for (DWORD i = 0; i < cTypes; i++)
    {
        hr = pHandler->GetMediaTypeByIndex(i, &pType);
        if (FAILED(hr))
        {
            goto done;
        }

        LogMediaType(pType);
        OutputDebugString(L"\n");

        SafeRelease(&pType);
    }

done:
    SafeRelease(&pPD);
    SafeRelease(&pSD);
    SafeRelease(&pHandler);
    SafeRelease(&pType);
    return hr;
}

int main()
{
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (!SUCCEEDED(hr))
    {
        return 1;
    }
    hr = MFStartup(MF_VERSION);
    if (!SUCCEEDED(hr))
    {
        return 1;
    }

    IMFMediaSource* source;
    hr = CreateVideoCaptureDevice(&source);
    if (!SUCCEEDED(hr))
    {
        return 1;
    }
    //EnumerateCaptureFormats(source);
    IMFSourceReader* videoReader;
    videoReader = NULL;
    hr = MFCreateSourceReaderFromMediaSource(
        source,
        pAttri,
        &videoReader);
    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    DWORD dwMediaTypeIndex = 0;
    while (true) {
        IMFMediaType* nativeTypes = NULL;
        hr = videoReader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            dwMediaTypeIndex, &nativeTypes);//MF_SOURCE_READER_CURRENT_TYPE_INDEX
        if (hr == MF_E_NO_MORE_TYPES)
        {
            int a = 1;
        }
        if (!SUCCEEDED(hr))
        {
            break;
        }
        GUID subType;
        UINT32 width = 0, height = 0, fpsNum = 0, fpsDen = 0;

        WCHAR* pGuidName = NULL;
        nativeTypes->GetGUID(MF_MT_SUBTYPE, &subType);
        GetGUIDName(subType, &pGuidName);
        std::wcout << "guid name: " << pGuidName << std::endl;
        //subTypeÊÇÊ²Ã´£¿
        MFGetAttributeSize(nativeTypes, MF_MT_FRAME_SIZE, &width, &height);
        MFGetAttributeRatio(nativeTypes, MF_MT_FRAME_RATE, &fpsNum, &fpsDen);
        //MFGetAttributeString(nativeTypes, MF_MT_YUV_MATRIX, NULL);//
        std::cout << width << "\t" << height << "\t" << fpsNum << "\t" << fpsDen << std::endl;
        ++dwMediaTypeIndex;

        /*PROPVARIANT pro;
        hr = videoReader->GetPresentationAttribute(MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            MF_PD_MIME_TYPE, &pro);
        int a = 1;*/

    }
    //LogMediaType(nativeTypes[0]);

    MFShutdown();
    CoUninitialize();
	return 0;
}