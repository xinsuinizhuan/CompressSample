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
#include "Logger.h"
#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
#include <QDir>
#include <QCoreApplication>
#include "spdlog/sinks/msvc_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"

void SetupLog(const QString& folder, const QString& suffix) {
    QDir dir(folder);
    dir.mkpath(folder);

    QString path;
    if (suffix.length() == 0)
        path = dir.absoluteFilePath(QCoreApplication::applicationName() + ".log");
    else
        path += dir.absoluteFilePath(QCoreApplication::applicationName() + suffix + ".log");

    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_every(std::chrono::seconds(1));
    spdlog::flush_on(spdlog::level::warn);

    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        path.toLocal8Bit().constData(), 1048576, 7);
    file_sink->set_pattern("%Y-%m-%d %H:%M:%S.%e %z [%L] [%05P.%05t] [%n] %v");

    auto msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
    msvc_sink->set_pattern("%Y-%m-%d %H:%M:%S.%e %z [%L] [%05P.%05t] [%n] %v");

    std::vector<spdlog::sink_ptr> sinks = {file_sink, msvc_sink};

    auto appLog = std::make_shared<spdlog::logger>("App", std::begin(sinks), std::end(sinks));
    spdlog::register_logger(appLog);
}

std::shared_ptr<spdlog::logger> GetLogger(LogScope ls) {
    std::shared_ptr<spdlog::logger> logger = nullptr;
    switch (ls) {
        case LS_APP:
            logger = spdlog::get("App");
            break;
        default:
            logger = spdlog::get("App");
            break;
    }
    return logger;
}
