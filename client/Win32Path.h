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
#include <string>

class Win32Path {
public:
    static bool IsPathExist(const std::wstring& strPath);
    static bool IsDirectoryPath(const std::wstring& strPath);
    static bool IsRootDirectory(const std::wstring& strPath);

    static std::wstring GetDirectoryName(const std::wstring& strPath);

    static bool IsBackslashTerminated(const std::wstring& strPath);
    static std::wstring RemoveBackslashTerminated(const std::wstring& strPath);
    static std::wstring AddBackslashTerminated(const std::wstring& strPath);

    static std::wstring GetBelongDirectoryName(const std::wstring& strPath);
    static std::wstring GetBelongDirectoryPath(const std::wstring& strPath);

    static std::wstring GetFullFileName(const std::wstring& strFilePath);
    static std::wstring GetFileNameWithoutSuffix(const std::wstring& strFilePath);
    static std::wstring GetFileSuffixName(const std::wstring& strFilePath);
};
