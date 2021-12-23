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