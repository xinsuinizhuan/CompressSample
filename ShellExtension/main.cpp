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
#include <Windows.h>
#include <Guiddef.h>
#include "ClassFactory.h"
#include "RegHelper.h"

const CLSID CLSID_ContextMenuExt =
    {0xBFD98515, 0xCD74, 0x48A4, {0x98, 0xE2, 0x13, 0xD2, 0x09, 0xE3, 0xEE, 0x4F}};

HINSTANCE g_hInst = NULL;
long g_cDllRef = 0;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            g_hInst = hModule;
            DisableThreadLibraryCalls(hModule);
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv) {
    HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;

    if (IsEqualCLSID(CLSID_ContextMenuExt, rclsid)) {
        hr = E_OUTOFMEMORY;

        ClassFactory* pClassFactory = new ClassFactory();
        if (pClassFactory) {
            hr = pClassFactory->QueryInterface(riid, ppv);
            pClassFactory->Release();
        }
    }

    return hr;
}

STDAPI DllCanUnloadNow(void) {
    return g_cDllRef > 0 ? S_FALSE : S_OK;
}

STDAPI DllRegisterServer(void) {
    HRESULT hr;

    wchar_t szModule[MAX_PATH];
    if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    hr = RegisterInprocServer(szModule, CLSID_ContextMenuExt,
                              L"ShellExtContextMenuHandler.ContextMenuExt Class",
                              L"Apartment");
    if (SUCCEEDED(hr)) {
        hr = RegisterShellExtContextMenuHandler(L"*",
                                                CLSID_ContextMenuExt,
                                                L"ShellExtContextMenuHandler.ContextMenuExt");

        hr = RegisterShellExtContextMenuHandler(L"Directory",
                                                CLSID_ContextMenuExt,
                                                L"ShellExtContextMenuHandler.ContextMenuExt");
    }

    return hr;
}

STDAPI DllUnregisterServer(void) {
    HRESULT hr = S_OK;

    wchar_t szModule[MAX_PATH];
    if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    hr = UnregisterInprocServer(CLSID_ContextMenuExt);
    if (SUCCEEDED(hr)) {
        hr = UnregisterShellExtContextMenuHandler(L"*",
                                                  CLSID_ContextMenuExt);

        hr = UnregisterShellExtContextMenuHandler(L"Directory",
                                                  CLSID_ContextMenuExt);
    }

    return hr;
}