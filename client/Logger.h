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
#include <QString>
#include "spdlog/spdlog.h"

enum LogScope {
    LS_APP = 0,
};

void SetupLog(const QString& folder, const QString& suffix);
std::shared_ptr<spdlog::logger> GetLogger(LogScope ls);

template <typename... Args>
inline void Info(LogScope ls, const char* format, const Args&... args) {
    std::shared_ptr<spdlog::logger> logger = GetLogger(ls);
    if (logger)
        logger->info(format, args...);
}

template <typename... Args>
inline void Warn(LogScope ls, const char* format, const Args&... args) {
    std::shared_ptr<spdlog::logger> logger = GetLogger(ls);
    if (logger)
        logger->warn(format, args...);
}

template <typename... Args>
inline void Error(LogScope ls, const char* format, const Args&... args) {
    std::shared_ptr<spdlog::logger> logger = GetLogger(ls);
    if (logger)
        logger->error(format, args...);
}