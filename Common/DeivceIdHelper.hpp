/*
***************************************************************************************************
* 许可声明：
* 本软件（含源码、二进制）为开源软件，遵循MIT开源许可协议（若以下声明与MIT许可协议发送冲突，以本声明为准）。
* 任何个人或组织都可以不受限制的使用、修改该软件。
* 任何个人或组织都可以以源代码和二进制的形式重新分发、使用该软件。
* 任何个人或组织都可以不受限制地将该软件（或修改后的任意版本）用于商业、非商业的用途。
* 
* 免责声明：
* 作者不对使用、修改、分发（以及其他任何形式的使用）该软件（及其他修改后的任意版本）所产生的后果负有任何法律责任。
* 作者也不对该软件的安全性、稳定性做出任何保证。
* 
* 使用、修改、分发该软件时需要保留上述声明，且默认已经同意上述声明。
***************************************************************************************************
*/
#ifndef DEVICE_HELPER_H_
#define DEVICE_HELPER_H_
#pragma once

#include <string>
#include <assert.h>
#include <Windows.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#include <strsafe.h>
#include "akali/registry.h"

std::wstring GenerateClientIdentifier() {
    std::wstring strResult;
    do {
        akali::RegKey reg(HKEY_CURRENT_USER, L"441B86F29FE74474A6FD445E159C0EC9");
        if (S_OK == reg.Open(KEY_ALL_ACCESS | KEY_WOW64_32KEY, false)) {
            reg.GetSZValue(L"Value", strResult);
            reg.Close();
        }
    } while (false);

    if (strResult.length() > 0) {
        return strResult;
    }

    union {
        long long ns100;
        FILETIME ft;
    } fileTime;
    GetSystemTimeAsFileTime(&fileTime.ft);
    long long lNowMicroMS = (long long)((fileTime.ns100 - 116444736000000000LL) / 10LL);
    std::wstring strNowMicroMS = std::to_wstring(lNowMicroMS);
    size_t len = strNowMicroMS.length();
    if (len < 20) {
        for (int i = 0; i < 20 - len; i++) {
            strNowMicroMS += L"0";
        }
    }
    assert(strNowMicroMS.length() == 20);

    wchar_t szGuid[100] = {0};
    GUID guid;
    if (S_OK == ::CoCreateGuid(&guid)) {
        StringCchPrintfW(szGuid, 100,
                         L"%08x%04x%04x%02x%02x%02x%02x%02x%02x%02x%02x",
                         guid.Data1, guid.Data2, guid.Data3,
                         guid.Data4[0], guid.Data4[1], guid.Data4[2],
                         guid.Data4[3], guid.Data4[4], guid.Data4[5],
                         guid.Data4[6], guid.Data4[7]);
    }
    else {
        for (int i = 0; i < 32; i++)
            szGuid[i] = L'0';
    }
    std::wstring strGuid = szGuid;
    assert(strGuid.length() == 32);

    strResult = strGuid + strNowMicroMS;

    do {
        akali::RegKey reg(HKEY_CURRENT_USER, L"441B86F29FE74474A6FD445E159C0EC9");
        if (S_OK == reg.Open(KEY_ALL_ACCESS | KEY_WOW64_32KEY, true)) {
            HRESULT hr = reg.SetSZValue(L"Value", strResult);
            assert(hr == S_OK);
            reg.Close();
        }
    } while (false);

    return strResult;
}

#endif
