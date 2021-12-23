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
#include "Util.h"
#ifdef WIN32
#include <ShlObj.h>
#endif
#include <Windows.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include "akali/trace.h"
#include "akali/registry.h"
#include "akali/stringencode.h"
#include "json/json.h"
using namespace akali;

std::wstring Util::GetChannel() {
    std::wstring strChannel;
    RegKey regKey(HKEY_CURRENT_USER, L"SOFTWARE\\" PRODUCT_NAME_EN_W);
    if (regKey.Open(KEY_READ, false) == S_OK) {
        regKey.GetSZValue(L"Channel", strChannel);
        regKey.Close();
    }

    if (strChannel.length() == 0)
        strChannel = L"master";
    return strChannel;
}

int Util::GetAppVersion() {
    std::wstring strVer;
    RegKey regKey(HKEY_CURRENT_USER, L"SOFTWARE\\" PRODUCT_NAME_EN_W);
    if (regKey.Open(KEY_READ, false) == S_OK) {
        regKey.GetSZValue(L"Version", strVer);
        regKey.Close();
    }

    if (strVer.length() == 0)
        strVer = L"1.0.0.0";

    int ver[4] = {0};
    swscanf(strVer.c_str(), L"%d.%d.%d.%d", &ver[0], &ver[1], &ver[2], &ver[3]);

    int result = (ver[0] << 3) + (ver[1] << 2) + (ver[2] << 1) + ver[3];

    return result;
}

void Util::SetAppVersion(int appver) {
    wchar_t szVer[100];
    StringCchPrintfW(szVer, 100, L"%d.%d.%d.%d", (BYTE)appver >> 3, (BYTE)appver >> 2, (BYTE)appver >> 1, (BYTE)appver);
    RegKey regKey(HKEY_CURRENT_USER, L"SOFTWARE\\" PRODUCT_NAME_EN_W);
    if (regKey.Open(KEY_WRITE | KEY_READ, true) == S_OK) {
        regKey.SetSZValue(L"Version", szVer);
        regKey.Close();
    }
}

int Util::GetOSMajorVersion() {
    LONG(WINAPI * RtlGetVersion)
    (LPOSVERSIONINFOEX);
    *(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

    OSVERSIONINFOEX osversion;
    osversion.dwOSVersionInfoSize = sizeof(osversion);
    osversion.szCSDVersion[0] = TEXT('\0');

    if (RtlGetVersion != NULL) {
        // RtlGetVersion uses 0 (STATUS_SUCCESS) as return value when succeeding
        if (RtlGetVersion(&osversion) != 0)
            return false;
    }
    else {
        // GetVersionEx was deprecated in Windows 10, only use it as fallback
#pragma warning(suppress : 4996)
        OSVERSIONINFOEX osversion;
        osversion.dwOSVersionInfoSize = sizeof(osversion);
        osversion.szCSDVersion[0] = TEXT('\0');
        if (!GetVersionEx((LPOSVERSIONINFO)&osversion))
            return false;
    }

    return (int)osversion.dwMajorVersion;
}

int Util::GetOSMinorVersion() {
    LONG(WINAPI * RtlGetVersion)
    (LPOSVERSIONINFOEX);
    *(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

    OSVERSIONINFOEX osversion;
    osversion.dwOSVersionInfoSize = sizeof(osversion);
    osversion.szCSDVersion[0] = TEXT('\0');

    if (RtlGetVersion != NULL) {
        // RtlGetVersion uses 0 (STATUS_SUCCESS) as return value when succeeding
        if (RtlGetVersion(&osversion) != 0)
            return false;
    }
    else {
        // GetVersionEx was deprecated in Windows 10, only use it as fallback
#pragma warning(suppress : 4996)
        OSVERSIONINFOEX osversion;
        osversion.dwOSVersionInfoSize = sizeof(osversion);
        osversion.szCSDVersion[0] = TEXT('\0');
        if (!GetVersionEx((LPOSVERSIONINFO)&osversion))
            return false;
    }

    return (int)osversion.dwMinorVersion;
}

std::wstring Util::GetNoRecentHistoryJSON() {
    std::wstring strnrecent;
    RegKey regKey(HKEY_CURRENT_USER, L"SOFTWARE\\" PRODUCT_NAME_EN_W);
    if (regKey.Open(KEY_READ, false) == S_OK) {
        regKey.GetSZValue(L"nrecent", strnrecent);
        regKey.Close();
    }
    return strnrecent;
}

std::wstring Util::GetSoftHistoryJSON() {
    std::wstring strSoftHis;
    RegKey regKey3(HKEY_CURRENT_USER, L"SOFTWARE\\" PRODUCT_NAME_EN_W);
    if (regKey3.Open(KEY_READ, false) == S_OK) {
        regKey3.GetSZValue(L"SoftHistory", strSoftHis);
        regKey3.Close();
    }
    return strSoftHis;
}

std::wstring Util::GetMoneyHistoryJSON() {
    std::wstring strMoneyHis;
    RegKey regKey2(HKEY_CURRENT_USER, L"SOFTWARE\\" PRODUCT_NAME_EN_W);
    if (regKey2.Open(KEY_READ, false) == S_OK) {
        regKey2.GetSZValue(L"MoneyHistory", strMoneyHis);
        regKey2.Close();
    }
    return strMoneyHis;
}

std::vector<NoRecentShowHistory> Util::ParseNoRecentHistory(const std::wstring& strJSON) {
    std::vector<NoRecentShowHistory> ret;
    try {
        Json::CharReaderBuilder readerBuilder;
        Json::CharReader* reader = readerBuilder.newCharReader();
        Json::Value root;
        Json::String err;
        std::string strJSON8 = akali::UnicodeToUtf8(strJSON);
        if (!reader->parse((const char*)(strJSON8.c_str()),
                           ((const char*)strJSON8.c_str() + strJSON8.size()), &root, &err)) {
            return ret;
        }

        if (root.isArray()) {
            for (int i = 0; i < root.size(); i++) {
                NoRecentShowHistory nrsh;
                nrsh.fromJsonObj(root[i]);
                ret.push_back(nrsh);
            }
        }
    } catch (std::exception& e) {
        akali::TraceMsgA("%s\n", e.what() ? e.what() : "");
    }

    return ret;
}

std::vector<SoftInstallHistory> Util::ParseSoftHistory(const std::wstring& strJSON) {
    std::vector<SoftInstallHistory> ret;
    try {
        Json::CharReaderBuilder readerBuilder;
        Json::CharReader* reader = readerBuilder.newCharReader();
        Json::Value root;
        Json::String err;
        std::string strJSON8 = akali::UnicodeToUtf8(strJSON);
        if (!reader->parse((const char*)(strJSON8.c_str()),
                           ((const char*)strJSON8.c_str() + strJSON8.size()), &root, &err)) {
            return ret;
        }

        if (root.isArray()) {
            for (int i = 0; i < root.size(); i++) {
                SoftInstallHistory nr;
                nr.fromJsonObj(root[i]);
                ret.push_back(nr);
            }
        }
    } catch (std::exception& e) {
        akali::TraceMsgA("%s\n", e.what() ? e.what() : "");
    }

    return ret;
}

std::vector<ProfitHistory> Util::ParseMoneyHistory(const std::wstring& strJSON) {
    std::vector<ProfitHistory> ret;
    try {
        Json::CharReaderBuilder readerBuilder;
        Json::CharReader* reader = readerBuilder.newCharReader();
        Json::Value root;
        Json::String err;
        std::string strJSON8 = akali::UnicodeToUtf8(strJSON);
        if (!reader->parse((const char*)(strJSON8.c_str()),
                           ((const char*)strJSON8.c_str() + strJSON8.size()), &root, &err)) {
            return ret;
        }

        if (root.isArray()) {
            for (int i = 0; i < root.size(); i++) {
                ProfitHistory pr;
                pr.fromJsonObj(root[i]);
                ret.push_back(pr);
            }
        }
    } catch (std::exception& e) {
        akali::TraceMsgA("%s\n", e.what() ? e.what() : "");
    }

    return ret;
}

void Util::SetNoRecentHistory(const std::vector<NoRecentShowHistory>& norecent) {
    Json::Value root;
    for (int i = 0; i < norecent.size(); i++) {
        Json::Value tmp;
        tmp["id"] = norecent[i].id;
        tmp["stamp"] = akali::UnicodeToUtf8(norecent[i].stamp);

        root.append(tmp);
    }

    std::wstring strJSON = akali::Utf8ToUnicode(root.toStyledString());
    RegKey regKey(HKEY_CURRENT_USER, L"SOFTWARE\\" PRODUCT_NAME_EN_W);
    if (regKey.Open(KEY_READ | KEY_WRITE, true) == S_OK) {
        regKey.SetSZValue(L"nrecent", strJSON);
        regKey.Close();
    }
}

void Util::SetSoftHistory(const std::vector<SoftInstallHistory>& his) {
    Json::Value root;
    for (int i = 0; i < his.size(); i++) {
        Json::Value tmp;
        tmp["id"] = his[i].id;
        tmp["stamp"] = akali::UnicodeToUtf8(his[i].stamp);

        root.append(tmp);
    }

    std::wstring strJSON = akali::Utf8ToUnicode(root.toStyledString());
    RegKey regKey(HKEY_CURRENT_USER, L"SOFTWARE\\" PRODUCT_NAME_EN_W);
    if (regKey.Open(KEY_READ | KEY_WRITE, true) == S_OK) {
        regKey.SetSZValue(L"SoftHistory", strJSON);
        regKey.Close();
    }
}

void Util::SetProfitHistory(const std::vector<ProfitHistory>& moneyHis) {
    Json::Value root;
    for (int i = 0; i < moneyHis.size(); i++) {
        Json::Value tmp;
        tmp["id"] = moneyHis[i].id;
        tmp["stamp"] = akali::UnicodeToUtf8(moneyHis[i].stamp);

        root.append(tmp);
    }

    std::wstring strJSON = akali::Utf8ToUnicode(root.toStyledString());
    RegKey regKey(HKEY_CURRENT_USER, L"SOFTWARE\\" PRODUCT_NAME_EN_W);
    if (regKey.Open(KEY_READ | KEY_WRITE, true) == S_OK) {
        regKey.SetSZValue(L"MoneyHistory", strJSON);
        regKey.Close();
    }
}

bool Util::CreateProcessWarpper(const std::wstring& path, const std::wstring& cmd) {
    WCHAR szDir[MAX_PATH] = {0};
    StringCchPrintfW(szDir, MAX_PATH, L"%s", path.c_str());
    PathRemoveFileSpecW(szDir);

    WCHAR szCMD[1024];
    StringCchPrintfW(szCMD, 1024, L"\"%s\" %s", path.c_str(), cmd.c_str());

    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi = {0};
    if (!CreateProcessW(NULL, szCMD, NULL, NULL, FALSE, 0, NULL, szDir, &si, &pi))
        return false;

    if (pi.hThread)
        CloseHandle(pi.hThread);

    if (pi.hProcess)
        CloseHandle(pi.hProcess);
    return true;
}
