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
#include "Win32Path.h"
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <Shlwapi.h>
#include <strsafe.h>

bool Win32Path::IsPathExist(const std::wstring& strPath) {
    if (strPath.length() == 0)
        return false;

    return (_waccess_s(strPath.c_str(), 0) == 0);
}

bool Win32Path::IsDirectoryPath(const std::wstring& strPath) {
    if (strPath.length() == 0)
        return false;

    wchar_t lastChar = strPath[strPath.length() - 1];
    if (lastChar == L'\\')
        return true;

    DWORD attribs = ::GetFileAttributesW(strPath.c_str());
    if (attribs & FILE_ATTRIBUTE_DIRECTORY)
        return true;

    HANDLE h = CreateFileW(strPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE) {
        CloseHandle(h);
        h = INVALID_HANDLE_VALUE;
        return false;
    }

    return false;
}

bool Win32Path::IsRootDirectory(const std::wstring& strPath) {
    if (strPath.length() == 0)
        return false;

    if (strPath.length() != 2 && strPath.length() != 3) {
        return false;
    }
    
    wchar_t firstChar = strPath[0];
    bool valid = ((firstChar >= L'A' && firstChar <= L'Z') || (firstChar >= L'a' && firstChar <= L'z'));

    if (!valid)
        return false;

    if (strPath[1] != L':')
        return false;
    
    return true;
}

std::wstring Win32Path::GetDirectoryName(const std::wstring& strPath) {
    if (IsRootDirectory(strPath))
        return L"";
    std::wstring strResult = RemoveBackslashTerminated(strPath);
    size_t pos = strPath.find_last_of(L'\\');
    if (pos != std::wstring::npos) {
        return strResult.substr(pos + 1);
    }
    return strResult;
}

bool Win32Path::IsBackslashTerminated(const std::wstring& strPath) {
    if (strPath.length() == 0)
        return false;

    wchar_t lastChar = strPath[strPath.length() - 1];
    if (lastChar == L'\\')
        return true;
    return false;
}

std::wstring Win32Path::RemoveBackslashTerminated(const std::wstring& strPath) {
    if (IsBackslashTerminated(strPath)) {
        return strPath.substr(0, strPath.length() - 1);
    }
    return strPath;
}

std::wstring Win32Path::AddBackslashTerminated(const std::wstring& strPath) {
    if (IsBackslashTerminated(strPath)) {
        return strPath;
    }
    return strPath + L"\\";
}

std::wstring Win32Path::GetBelongDirectoryName(const std::wstring& strPath) {
    std::wstring strBelongDirPath = GetBelongDirectoryPath(strPath);
    if (strBelongDirPath.length() == 0)
        return L"";

    if (IsRootDirectory(strBelongDirPath))
        return strBelongDirPath;

    strBelongDirPath = RemoveBackslashTerminated(strBelongDirPath);
    size_t lastBackslashPos = strBelongDirPath.find_last_of(L'\\');
    if (lastBackslashPos == std::wstring::npos)
        return L"";
    return strBelongDirPath.substr(lastBackslashPos + 1);
}

std::wstring Win32Path::GetBelongDirectoryPath(const std::wstring& strPath) {
    std::wstring strResult = RemoveBackslashTerminated(strPath);
    size_t lastBackslashPos = strResult.find_last_of(L'\\');
    if (lastBackslashPos == std::wstring::npos)
        return L"";

    return strResult.substr(0, lastBackslashPos + 1);
}

std::wstring Win32Path::GetFullFileName(const std::wstring& strFilePath) {
    size_t lastBackslashPos = strFilePath.find_last_of(L'\\');
    if (lastBackslashPos == std::wstring::npos)
        return strFilePath;
    return strFilePath.substr(lastBackslashPos + 1);
}

std::wstring Win32Path::GetFileNameWithoutSuffix(const std::wstring& strFilePath) {
    std::wstring strResult = GetFullFileName(strFilePath);
    size_t lastPotPos = strResult.find_last_of(L'.');
    if (lastPotPos == std::wstring::npos)
        return strResult;
    return strResult.substr(0, lastPotPos);
}

std::wstring Win32Path::GetFileSuffixName(const std::wstring& strFilePath) {
    std::wstring strResult = GetFullFileName(strFilePath);
    size_t lastPotPos = strResult.find_last_of(L'.');
    if (lastPotPos == std::wstring::npos)
        return L"";
    return strResult.substr(lastPotPos + 1);
}
