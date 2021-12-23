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
#ifndef _REGISTRY_H__
#define _REGISTRY_H__
#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <vector>
#include <string>

class RegKey {
   public:
    RegKey(HKEY hkeyRoot, LPCWSTR pszSubKey);
    ~RegKey(void);

    HRESULT Open(REGSAM samDesired, bool bCreate);
    bool IsOpen(void) const;
    HKEY GetHandle(void) const;
    void Attach(HKEY hkey);
    void Detach(void);
    void Close(void);
    HRESULT WatchForChange(DWORD dwChangeFilter, bool bWatchSubtree);
    HRESULT WaitForChange(DWORD dwChangeFilter, bool bWatchSubtree);
    static bool DeleteKey(HKEY hKey, LPCWSTR pszSubKey, LPCWSTR pszValName, bool bPrefer64View);
    static bool DeleteSubKeys(HKEY hKeyRoot, LPCTSTR lpSubKey, bool bPrefer64View);
    HRESULT GetDWORDValue(LPCWSTR pszValueName, DWORD* pdwDataOut) const;
    HRESULT GetBINARYValue(LPCWSTR pszValueName, LPBYTE pbDataOut, int cbDataOut) const;
    HRESULT GetSZValue(LPCWSTR pszValueName, OUT std::wstring& strValue) const;
    HRESULT GetExpandSZValue(LPCWSTR pszValueName, bool bRetrieveExpandedString, OUT std::wstring& strValue) const;
    HRESULT GetMultiSZValue(LPCWSTR pszValueName, OUT std::vector<std::wstring>& vStrValues) const;
    DWORD GetValueBufferSize(LPCWSTR pszValueName) const;
    HRESULT SetDWORDValue(LPCWSTR pszValueName, DWORD dwData);
    HRESULT SetBINARYValue(LPCWSTR pszValueName, const LPBYTE pbData, int cbData);
    HRESULT SetSZValue(LPCWSTR pszValueName, const std::wstring& strData);
    HRESULT SetExpandSZValue(LPCWSTR pszValueName, const std::wstring& strData);
    HRESULT SetMultiSZValue(LPCWSTR pszValueName, const std::vector<std::wstring>& vStrValues);

    HRESULT GetSubKeys(std::vector<std::wstring>& subKeys);

    static bool IsWin64(); 
   protected:
    void OnChange(HKEY hkey);

   private:
    RegKey(const RegKey& rhs);
    RegKey& operator=(const RegKey& rhs);
    HKEY m_hkeyRoot;
    mutable HKEY m_hkey;
    HANDLE m_hChangeEvent;
    HANDLE m_hNotifyThr;
    DWORD m_dwSamDesired;
    DWORD m_dwChangeFilter;
    std::wstring m_strSubKey;
    bool m_bWatchSubtree;

    HRESULT GetValue(LPCWSTR pszValueName, DWORD dwTypeExpected, LPBYTE pbData, DWORD cbData) const;
    HRESULT SetValue(LPCWSTR pszValueName, DWORD dwValueType, const LPBYTE pbData, int cbData);
    LPWSTR CreateDoubleNulTermList(const std::vector<std::wstring>& vStrValues) const;
    static unsigned int _stdcall NotifyWaitThreadProc(LPVOID pvParam);
    static bool RegDeleteKey32_64(HKEY hKey, LPCWSTR pszSubKey, bool bPrefer64View);
    static bool RegDeleteSubKeys(HKEY hKey, bool bPrefer64View);
    static BOOL RegDelSubKeysRecurse(HKEY hKeyRoot, LPTSTR lpSubKey, bool bPrefer64View);
};
#endif  // ! REGISTRY_H__
