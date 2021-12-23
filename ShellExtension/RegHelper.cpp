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
#include "RegHelper.h"
#include <strsafe.h>

HRESULT SetHKCRRegistryKeyAndValue(PCWSTR pszSubKey, PCWSTR pszValueName, PCWSTR pszData) {
    HRESULT hr;
    HKEY hKey = NULL;

    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, pszSubKey, 0,
                                           NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));

    if (SUCCEEDED(hr)) {
        if (pszData != NULL) {
            DWORD cbData = lstrlen(pszData) * sizeof(*pszData);
            hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, pszValueName, 0,
                                                  REG_SZ, reinterpret_cast<const BYTE*>(pszData), cbData));
        }

        RegCloseKey(hKey);
    }

    return hr;
}

HRESULT GetHKCRRegistryKeyAndValue(PCWSTR pszSubKey, PCWSTR pszValueName, PWSTR pszData, DWORD cbData) {
    HRESULT hr;
    HKEY hKey = NULL;

    // Try to open the specified registry key.
    hr = HRESULT_FROM_WIN32(RegOpenKeyEx(HKEY_CLASSES_ROOT, pszSubKey, 0,
                                         KEY_READ, &hKey));

    if (SUCCEEDED(hr)) {
        // Get the data for the specified value name.
        hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, pszValueName, NULL,
                                                NULL, reinterpret_cast<LPBYTE>(pszData), &cbData));

        RegCloseKey(hKey);
    }

    return hr;
}

HRESULT RegisterInprocServer(PCWSTR pszModule, const CLSID& clsid, PCWSTR pszFriendlyName, PCWSTR pszThreadModel) {
    if (pszModule == NULL || pszThreadModel == NULL) {
        return E_INVALIDARG;
    }

    HRESULT hr;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
    if (SUCCEEDED(hr)) {
        hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszFriendlyName);

        if (SUCCEEDED(hr)) {
            hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey),
                                 L"CLSID\\%s\\InprocServer32", szCLSID);
            if (SUCCEEDED(hr)) {
                hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszModule);
                if (SUCCEEDED(hr)) {
                    hr = SetHKCRRegistryKeyAndValue(szSubkey,
                                                    L"ThreadingModel", pszThreadModel);
                }
            }
        }
    }

    return hr;
}

HRESULT UnregisterInprocServer(const CLSID& clsid) {
    HRESULT hr = S_OK;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
    if (SUCCEEDED(hr)) {
        hr = HRESULT_FROM_WIN32(RegDeleteTree(HKEY_CLASSES_ROOT, szSubkey));
    }

    return hr;
}

HRESULT RegisterShellExtContextMenuHandler(
    PCWSTR pszFileType,
    const CLSID& clsid,
    PCWSTR pszFriendlyName) {
    if (pszFileType == NULL) {
        return E_INVALIDARG;
    }

    HRESULT hr;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

    if (*pszFileType == L'.') {
        wchar_t szDefaultVal[260];
        hr = GetHKCRRegistryKeyAndValue(pszFileType, NULL, szDefaultVal,
                                        sizeof(szDefaultVal));

        if (SUCCEEDED(hr) && szDefaultVal[0] != L'\0') {
            pszFileType = szDefaultVal;
        }
    }

    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey),
                         L"%s\\shellex\\ContextMenuHandlers\\%s", pszFileType, szCLSID);
    if (SUCCEEDED(hr)) {
        hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszFriendlyName);
    }

    return hr;
}

HRESULT UnregisterShellExtContextMenuHandler(
    PCWSTR pszFileType,
    const CLSID& clsid) {
    if (pszFileType == NULL) {
        return E_INVALIDARG;
    }

    HRESULT hr;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

    if (*pszFileType == L'.') {
        wchar_t szDefaultVal[260];
        hr = GetHKCRRegistryKeyAndValue(pszFileType, NULL, szDefaultVal,
                                        sizeof(szDefaultVal));

        if (SUCCEEDED(hr) && szDefaultVal[0] != L'\0') {
            pszFileType = szDefaultVal;
        }
    }

    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey),
                         L"%s\\shellex\\ContextMenuHandlers\\%s", pszFileType, szCLSID);
    if (SUCCEEDED(hr)) {
        hr = HRESULT_FROM_WIN32(RegDeleteTree(HKEY_CLASSES_ROOT, szSubkey));
    }

    return hr;
}