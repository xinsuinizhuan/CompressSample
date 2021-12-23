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
#include "ContextMenuExt.h"
#include <strsafe.h>
#include <Shlwapi.h>
#include "resource.h"
#include "Trace.h"
#include "Registry.h"
#include "Win32Path.h"
#include "StringEncode.h"
#include "../Common/ProductDef.h"

#define IDM_ADD_TO_COMPRESS_FILE 0
#define IDM_ADD_TO_COMPRESS_FILE_WITH_NAME 1
#define IDM_EXTRACT_TO 2
#define IDM_EXTRACT_TO_CURRENT_FOLDER 3
#define IDM_EXTRACT_TO_WITH_FOLDER_NAME 4

extern HINSTANCE g_hInst;
extern long g_cDllRef;

ContextMenuExt::ContextMenuExt(void) :
    m_cRef(1) {
    InterlockedIncrement(&g_cDllRef);
}

ContextMenuExt::~ContextMenuExt(void) {
    selectedFiles_.clear();
    InterlockedDecrement(&g_cDllRef);
}

long long ContextMenuExt::GetTimeStamp() {
    union {
        long long ns100;
        FILETIME ft;
    } fileTime;
    GetSystemTimeAsFileTime(&fileTime.ft);

    long long lNowMicroMS = (long long)((fileTime.ns100 - 116444736000000000LL) / 10LL);

    return lNowMicroMS;
}

bool ContextMenuExt::IsSupportCompressFormat(const std::wstring& strPath) {
    bool isSupport = false;
    size_t lastPot = strPath.find_last_of(L'.');
    if (lastPot != std::wstring::npos) {
        std::wstring strSuffix = strPath.substr(lastPot);
        if (_wcsicmp(strSuffix.c_str(), L".zip") == 0 ||
            _wcsicmp(strSuffix.c_str(), L".7z") == 0 ||
            _wcsicmp(strSuffix.c_str(), L".rar") == 0) {
            isSupport = true;
        }
    }

    return isSupport;
}

void ContextMenuExt::IsMenuShown(bool& bCompressMenuShown, bool& bExtractMenuShown) {
    bCompressMenuShown = false;
    bExtractMenuShown = false;

    if (selectedFiles_.size() == 1) {
        if (Win32Path::IsDirectoryPath(selectedFiles_[0])) {
            bCompressMenuShown = true;
        }
        else {
            if (IsSupportCompressFormat(selectedFiles_[0])) {
                bExtractMenuShown = true;
            }
            else {
                bCompressMenuShown = true;
            }
        }
    }
    else if (selectedFiles_.size() > 1) {
        bCompressMenuShown = true;
    }
}

void ContextMenuExt::GetRecommandCompressFileName(std::wstring& onlyFileName, std::wstring& fullFilePath) {
    onlyFileName.clear();
    fullFilePath.clear();

    if (selectedFiles_.size() >= 1) {
        std::wstring filePath = selectedFiles_[0];

        if (selectedFiles_.size() == 1) {
            if (Win32Path::IsDirectoryPath(filePath)) {
                onlyFileName = Win32Path::GetDirectoryName(filePath);
            }
            else {
                onlyFileName = Win32Path::GetFileNameWithoutSuffix(filePath);
            }

            if (onlyFileName.length() > 0)
                onlyFileName += L".zip";
        }
        else if (selectedFiles_.size() > 1) {
            onlyFileName = Win32Path::GetBelongDirectoryName(filePath);
            if (onlyFileName.length() > 0)
                onlyFileName += L".zip";
        }

        fullFilePath = Win32Path::AddBackslashTerminated(Win32Path::GetBelongDirectoryPath(filePath)) + onlyFileName;
    }
}

void ContextMenuExt::GetRecommandExtractFolderName(std::wstring& onlyFolderName, std::wstring& fullFolderPath) {
    onlyFolderName.clear();
    fullFolderPath.clear();

    if (selectedFiles_.size() == 1) {
        onlyFolderName = Win32Path::GetFileNameWithoutSuffix(selectedFiles_[0]);
        std::wstring dirPath = Win32Path::AddBackslashTerminated(Win32Path::GetBelongDirectoryPath(selectedFiles_[0]));
        fullFolderPath = Win32Path::AddBackslashTerminated(dirPath + onlyFolderName);
    }
}

void ContextMenuExt::OnAddToCompressFile(HWND hWnd) {
    TraceMsgW(L"[ContextMenuExt] OnAddToCompressFile");
    std::wstringstream ssParam;
    ssParam << L"-a ";
    ssParam << L"-files=\"";

    for (size_t i = 0; i < selectedFiles_.size(); i++) {
        ssParam << selectedFiles_[i] << "|";
    }
    ssParam << L"\"";

    startProcess(ssParam.str());
}

void ContextMenuExt::OnAddToCompressFileWithName(HWND hWnd) {
    TraceMsgW(L"[ContextMenuExt] OnAddToCompressFileWithName recommandCompressFilePath: %s", recommandCompressFilePath_.c_str());

    std::wstringstream ssParam;
    ssParam << L"-a ";
    ssParam << L"-files=\"";

    for (size_t i = 0; i < selectedFiles_.size(); i++) {
        ssParam << selectedFiles_[i] << "|";
    }
    ssParam << L"\"";

    if (recommandCompressFilePath_.length() > 0) {
        ssParam << L" -target=\"" << recommandCompressFilePath_ << L"\"";
    }

    startProcess(ssParam.str());
}

void ContextMenuExt::OnExtractTo(HWND hWnd) {
    TraceMsgW(L"[ContextMenuExt] OnExtractTo");

    std::wstringstream ssParam;
    ssParam << L"-e ";
    ssParam << L"-file=\"";

    if (selectedFiles_.size() > 0) {
        ssParam << selectedFiles_[0];
    }

    ssParam << L"\"";

    startProcess(ssParam.str());
}

void ContextMenuExt::OnExtractToCurrentFolder(HWND hWnd) {
    TraceMsgW(L"[ContextMenuExt] OnExtractToCurrentFolder");

    std::wstringstream ssParam;
    ssParam << L"-ec ";
    ssParam << L"-file=\"";
    if (selectedFiles_.size() > 0) {
        ssParam << selectedFiles_[0];
    }
    ssParam << L"\"";

    startProcess(ssParam.str());
}

void ContextMenuExt::OnExtractToWithFolderName(HWND hWnd) {
    TraceMsgW(L"[ContextMenuExt] OnExtractToWithFolderName recommandExtractFolderPath: %s", recommandExtractFolderPath_.c_str());

    std::wstringstream ssParam;
    ssParam << L"-e ";
    ssParam << L"-file=\"";
    if (selectedFiles_.size() > 0) {
        ssParam << selectedFiles_[0];
    }
    ssParam << L"\"";

    if (recommandExtractFolderPath_.length() > 0) {
        ssParam << L" -target=\"" << recommandExtractFolderPath_ << L"\"";
    }

    startProcess(ssParam.str());
}

void ContextMenuExt::startProcess(const std::wstring& param) {
    std::wstring installFolder;
    std::wstring strKey = std::to_wstring(GetTimeStamp());
    RegKey regKey(HKEY_CURRENT_USER, L"SOFTWARE\\" PRODUCT_NAME_EN_W);
    if (regKey.Open(KEY_WRITE | KEY_READ, true) != S_OK) {
        return;
    }
    regKey.GetSZValue(L"Path", installFolder);
    if (installFolder.length() == 0) {
        regKey.Close();
        return;
    }

    if (regKey.SetSZValue(strKey.c_str(), param) != S_OK) {
        regKey.Close();
        return;
    }
    regKey.Close();

    wchar_t szPath[MAX_PATH] = {0};
    wchar_t szDir[MAX_PATH] = {0};
    StringCchCopyW(szDir, MAX_PATH, installFolder.c_str());
    PathAddBackslashW(szDir);
    StringCchCopyW(szPath, MAX_PATH, szDir);
    PathCombineW(szPath, szPath, CLIENT_EXE_NAME_W);

    wchar_t szFullCMD[MAX_PATH];
    StringCchPrintfW(szFullCMD, MAX_PATH, L"\"%s\" -contextmenu=%s", szPath, strKey.c_str());
    TraceMsgW(L"[ContextMenuExt] Create process: %s", szFullCMD);

    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi = {0};
    if (::CreateProcessW(NULL, szFullCMD, NULL, NULL, FALSE, 0, NULL, szDir, &si, &pi)) {
        if (pi.hThread)
            CloseHandle(pi.hThread);
        if (pi.hProcess)
            CloseHandle(pi.hProcess);
    }
}

//IUnknown

IFACEMETHODIMP ContextMenuExt::QueryInterface(REFIID riid, void** ppv) {
    static const QITAB qit[] =
        {
            QITABENT(ContextMenuExt, IContextMenu),
            QITABENT(ContextMenuExt, IShellExtInit),
            {0},
        };
    return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG)
ContextMenuExt::AddRef() {
    return InterlockedIncrement(&m_cRef);
}

IFACEMETHODIMP_(ULONG)
ContextMenuExt::Release() {
    ULONG ref = InterlockedDecrement(&m_cRef);

    if (ref == 0) {
        delete this;
    }

    return ref;
}

//IShellExtInit

IFACEMETHODIMP ContextMenuExt::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID) {
    TraceMsgW(L"[ContextMenuExt] Initialize");

    if (NULL == pDataObj) {
        return E_INVALIDARG;
    }

    FORMATETC fe = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    STGMEDIUM stm = {TYMED_HGLOBAL};

    if (FAILED(pDataObj->GetData(&fe, &stm))) {
        TraceMsgW(L"[ContextMenuExt] Initialize GetData failed");
        return E_INVALIDARG;
    }

    HDROP hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
    if (!hDrop) {
        TraceMsgW(L"[ContextMenuExt] Initialize GlobalLock failed");
        ReleaseStgMedium(&stm);
        return E_INVALIDARG;
    }

    UINT nFiles = DragQueryFile(hDrop, UINT(-1), NULL, 0);
    if (nFiles > 0) {
        selectedFiles_.clear();

        for (UINT i = 0; i < nFiles; i++) {
            UINT len = DragQueryFile(hDrop, i, NULL, 0);
            if (len == 0)
                continue;

            auto szFilePath = std::make_unique<wchar_t[]>(len + 1);
            if (0 == DragQueryFile(hDrop, (UINT)i, szFilePath.get(), len + 1))
                continue;

            std::wstring strFilePath = szFilePath.get();
            selectedFiles_.push_back(strFilePath);
            TraceMsgW(L"[ContextMenuExt] selected: %s", strFilePath.c_str());
        }
    }

    GlobalUnlock(stm.hGlobal);
    ReleaseStgMedium(&stm);

    return S_OK;
}

//IContextMenu

IFACEMETHODIMP ContextMenuExt::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {
    TraceMsgW(L"[ContextMenuExt] QueryContextMenu indexMenu: %d, idCmdFirst: %d, idCmdLast: %d", indexMenu, idCmdFirst, idCmdLast);
    if (CMF_DEFAULTONLY & uFlags) {
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
    }

    USHORT menuCount = 0;
    bool bCompressMenuShown = false;
    bool bExtractMenuShown = false;
    IsMenuShown(bCompressMenuShown, bExtractMenuShown);

    if (bCompressMenuShown) {
        std::wstring strRecommandCompressFilename;
        GetRecommandCompressFileName(strRecommandCompressFilename, recommandCompressFilePath_);

        {
            MENUITEMINFOW mii = {sizeof(mii)};
            mii.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_STATE | MIIM_BITMAP;
            mii.wID = idCmdFirst + IDM_ADD_TO_COMPRESS_FILE;
            mii.fType = MFT_STRING;
            mii.dwTypeData = L"添加到压缩文件(&A)...";
            mii.fState = MFS_ENABLED;
            mii.hbmpItem = iconBitmapUtils_.IconToBitmapPARGB32(g_hInst, IDI_ICON1);

            if (!InsertMenuItemW(hMenu, indexMenu + menuCount, TRUE, &mii)) {
                return HRESULT_FROM_WIN32(GetLastError());
            }
            menuCount++;
        }

        if (strRecommandCompressFilename.length() > 0) {
            std::wstring strMenuText = L"添加到“" + strRecommandCompressFilename + L"“(&T)";
            MENUITEMINFOW mii = {sizeof(mii)};
            mii.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_STATE | MIIM_BITMAP;
            mii.wID = idCmdFirst + IDM_ADD_TO_COMPRESS_FILE_WITH_NAME;
            mii.fType = MFT_STRING;
            mii.dwTypeData = (LPWSTR)strMenuText.c_str();
            mii.fState = MFS_ENABLED;
            mii.hbmpItem = iconBitmapUtils_.IconToBitmapPARGB32(g_hInst, IDI_ICON1);

            if (!InsertMenuItemW(hMenu, indexMenu + menuCount, TRUE, &mii)) {
                return HRESULT_FROM_WIN32(GetLastError());
            }
            menuCount++;
        }
    }

    if (bExtractMenuShown) {
        std::wstring strRecommandExtractFolderName;
        GetRecommandExtractFolderName(strRecommandExtractFolderName, recommandExtractFolderPath_);

        {
            MENUITEMINFOW mii = {sizeof(mii)};
            mii.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_STATE | MIIM_BITMAP;
            mii.wID = idCmdFirst + IDM_EXTRACT_TO;
            mii.fType = MFT_STRING;
            mii.dwTypeData = L"解压到(&F)";
            mii.fState = MFS_ENABLED;
            mii.hbmpItem = iconBitmapUtils_.IconToBitmapPARGB32(g_hInst, IDI_ICON1);

            if (!InsertMenuItemW(hMenu, indexMenu + menuCount, TRUE, &mii)) {
                return HRESULT_FROM_WIN32(GetLastError());
            }
            menuCount++;
        }

        {
            MENUITEMINFOW mii = {sizeof(mii)};
            mii.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_STATE | MIIM_BITMAP;
            mii.wID = idCmdFirst + IDM_EXTRACT_TO_CURRENT_FOLDER;
            mii.fType = MFT_STRING;
            mii.dwTypeData = L"解压到当前目录(&X)";
            mii.fState = MFS_ENABLED;
            mii.hbmpItem = iconBitmapUtils_.IconToBitmapPARGB32(g_hInst, IDI_ICON1);

            if (!InsertMenuItemW(hMenu, indexMenu + menuCount, TRUE, &mii)) {
                return HRESULT_FROM_WIN32(GetLastError());
            }
            menuCount++;
        }

        if (strRecommandExtractFolderName.length() > 0) {
            std::wstring menuTxt = L"解压到 " + strRecommandExtractFolderName + L"\\ (&E)";
            MENUITEMINFOW mii = {sizeof(mii)};
            mii.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_STATE | MIIM_BITMAP;
            mii.wID = idCmdFirst + IDM_EXTRACT_TO_WITH_FOLDER_NAME;
            mii.fType = MFT_STRING;
            mii.dwTypeData = (LPWSTR)menuTxt.c_str();
            mii.fState = MFS_ENABLED;
            mii.hbmpItem = iconBitmapUtils_.IconToBitmapPARGB32(g_hInst, IDI_ICON1);

            if (!InsertMenuItemW(hMenu, indexMenu + menuCount, TRUE, &mii)) {
                return HRESULT_FROM_WIN32(GetLastError());
            }
            menuCount++;
        }
    }

    MENUITEMINFOW sep = {sizeof(sep)};
    sep.fMask = MIIM_TYPE;
    sep.fType = MFT_SEPARATOR;

    if (!InsertMenuItemW(hMenu, indexMenu + menuCount, TRUE, &sep)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    menuCount++;

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, menuCount + 1);
}

IFACEMETHODIMP ContextMenuExt::InvokeCommand(LPCMINVOKECOMMANDINFO pici) {
    BOOL fEx = FALSE;
    BOOL fUnicode = FALSE;

    if (pici->cbSize == sizeof(CMINVOKECOMMANDINFOEX)) {
        fEx = TRUE;
        if (pici->fMask & CMIC_MASK_UNICODE) {
            fUnicode = TRUE;
        }
    }

    TraceMsgW(L"[ContextMenuExt] Unicode: %d", fUnicode ? 1 : 0);

#if 0
    if (!fUnicode && HIWORD(pici->lpVerb)) {
        TraceMsgA("[ContextMenuExt] pici->lpVerb: %s", pici->lpVerb);
        if (StrCmpIA(pici->lpVerb, "") == 0) {
            OnAddToCompressFile(pici->hwnd);
        }
        else {
            return E_FAIL;
        }
    }
    else if (fUnicode && HIWORD(((CMINVOKECOMMANDINFOEX*)pici)->lpVerbW)) {
        CMINVOKECOMMANDINFOEX* piciEx = (CMINVOKECOMMANDINFOEX*)pici;
        TraceMsgW(L"[ContextMenuExt] piciEx->lpVerbW: %s", piciEx->lpVerbW);
        if (StrCmpIW(piciEx->lpVerbW, L"") == 0) {
            OnAddToCompressFile(piciEx->hwnd);
        }
        else {
            return E_FAIL;
        }
    }
    else {
        TraceMsgW(L"[ContextMenuExt] LOWORD(pici->lpVerb): %d", (int)(LOWORD(pici->lpVerb)));
        if (LOWORD(pici->lpVerb) == IDM_ADD_TO_COMPRESS_FILE) {
            OnAddToCompressFile(pici->hwnd);
        }
        else {
            return E_FAIL;
        }
    }
#else
    WORD id = LOWORD(pici->lpVerb);
    TraceMsgW(L"[ContextMenuExt] LOWORD(pici->lpVerb): %d", id);

    if (id == IDM_ADD_TO_COMPRESS_FILE) {
        OnAddToCompressFile(pici->hwnd);
    }
    else if (id == IDM_ADD_TO_COMPRESS_FILE_WITH_NAME) {
        OnAddToCompressFileWithName(pici->hwnd);
    }
    else if (id == IDM_EXTRACT_TO) {
        OnExtractTo(pici->hwnd);
    }
    else if (id == IDM_EXTRACT_TO_CURRENT_FOLDER) {
        OnExtractToCurrentFolder(pici->hwnd);
    }
    else if (id == IDM_EXTRACT_TO_WITH_FOLDER_NAME) {
        OnExtractToWithFolderName(pici->hwnd);
    }
    else {
        return E_FAIL;
    }
#endif

    return S_OK;
}

IFACEMETHODIMP ContextMenuExt::GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax) {
    return S_OK;
}