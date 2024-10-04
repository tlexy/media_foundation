#pragma once

#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mferror.h>

HRESULT GetGUIDName(const GUID& guid, WCHAR** ppwsz);

HRESULT LogMediaType(IMFMediaType* pType);