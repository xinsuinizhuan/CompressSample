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
#pragma once

#include <Windows.h>
#include <ShlObj.h>
#include <string>
#include <vector>
#include "IconBitmapUtils.h"

typedef struct _MenuItem {
    int id;

    std::wstring menuTextW;

    std::string verbA;
    std::wstring verbW;

    std::string verbCanonicalNameA;
    std::wstring verbCanonicalNameW;

    std::string verbHelpTextA;
    std::wstring verbHelpTextW;
} MenuItem;

class ContextMenuExt : public IShellExtInit, public IContextMenu {
   public:
    //IUnknown methods
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv);
    IFACEMETHODIMP_(ULONG)
    AddRef();
    IFACEMETHODIMP_(ULONG)
    Release();

    //IShellExtInit method
    IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgId);

    //IContextMenu methods
    IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici);
    IFACEMETHODIMP GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax);

    ContextMenuExt(void);
    ~ContextMenuExt(void);

   private:
    long long GetTimeStamp();
    bool IsSupportCompressFormat(const std::wstring& strPath);
    void IsMenuShown(bool& bCompressMenuShown, bool& bExtractMenuShown);
    void GetRecommandCompressFileName(std::wstring& onlyFileName, std::wstring& fullFilePath);
    void GetRecommandExtractFolderName(std::wstring& onlyFolderName, std::wstring& fullFolderPath);

    void OnAddToCompressFile(HWND hWnd);
    void OnAddToCompressFileWithName(HWND hWnd);
    void OnExtractTo(HWND hWnd);
    void OnExtractToCurrentFolder(HWND hWnd);
    void OnExtractToWithFolderName(HWND hWnd);

    void startProcess(const std::wstring& param);

   private:
    long m_cRef;
    std::vector<std::wstring> selectedFiles_;
    IconBitmapUtils iconBitmapUtils_;

   private:
    std::wstring recommandCompressFilePath_;
    std::wstring recommandExtractFolderPath_;
};
