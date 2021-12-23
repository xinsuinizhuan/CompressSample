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
