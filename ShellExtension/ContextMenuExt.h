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
