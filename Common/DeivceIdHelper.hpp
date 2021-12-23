/*
***************************************************************************************************
* ���������
* ���������Դ�롢�����ƣ�Ϊ��Դ�������ѭMIT��Դ���Э�飨������������MIT���Э�鷢�ͳ�ͻ���Ա�����Ϊ׼����
* �κθ��˻���֯�����Բ������Ƶ�ʹ�á��޸ĸ������
* �κθ��˻���֯��������Դ����Ͷ����Ƶ���ʽ���·ַ���ʹ�ø������
* �κθ��˻���֯�����Բ������Ƶؽ�����������޸ĺ������汾��������ҵ������ҵ����;��
* 
* ����������
* ���߲���ʹ�á��޸ġ��ַ����Լ������κ���ʽ��ʹ�ã���������������޸ĺ������汾���������ĺ�������κη������Ρ�
* ����Ҳ���Ը�����İ�ȫ�ԡ��ȶ��������κα�֤��
* 
* ʹ�á��޸ġ��ַ������ʱ��Ҫ����������������Ĭ���Ѿ�ͬ������������
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
