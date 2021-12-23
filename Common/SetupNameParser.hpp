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
#ifndef SETUP_NAME_PARSER_H_
#define SETUP_NAME_PARSER_H_
#pragma once

#include <string>
#include <windows.h>
#include <Shlwapi.h>
#include <strsafe.h>

class SetupNameParser {
   public:
    static void Parse(const std::wstring& setupName, std::wstring &strPrefix, std::wstring& strChannel, int& id) {
        try {
            wchar_t szPath[MAX_PATH] = {0};
            StringCchCopyW(szPath, MAX_PATH, setupName.c_str());

            wchar_t* first_ = NULL;
            wchar_t* last_ = NULL;

            first_ = wcsstr(szPath, L"_");

            if (first_)
                last_ = wcsstr(first_ + 1, L"_");

            if (first_ && last_ && last_ > first_ + 1) {
                wchar_t buf[100] = {0};
                StringCchCopyNW(buf, 100, first_ + 1, last_ - first_ - 1);
                strChannel = buf;
            }

            if (first_) {
                wchar_t buf2[100] = {0};
                StringCchCopyNW(buf2, 100, szPath, first_ - szPath);
                strPrefix = buf2;
            }

            wchar_t* last_pot = NULL;
            wchar_t* last_at = NULL;

            last_pot = wcsrchr(szPath, L'.');
            last_at = wcsrchr(szPath, L'@');
            if (last_at && last_pot && last_pot > last_at + 1) {
                wchar_t* space = wcsstr(last_at, L" ");
                wchar_t buf[100] = {0};
                if (space)
                    StringCchCopyNW(buf, 100, last_at + 1, space - last_at - 1);
                else
                    StringCchCopyNW(buf, 100, last_at + 1, last_pot - last_at - 1);

                if (wcslen(buf) > 0) {
                    id = _wtoi(buf);
                }
            }
        } catch (...) {
            strChannel.clear();
            id = 0;
        }
    }
};

#endif  // !SETUP_NAME_PARSER_H_
