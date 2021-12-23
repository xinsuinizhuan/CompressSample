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
