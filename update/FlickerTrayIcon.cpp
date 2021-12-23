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
#include "stdafx.h"
#include "FlickerTrayIcon.h"

FlickerTrayIcon::FlickerTrayIcon(const TaskMeta& tm, QObject* parent) :
    QSystemTrayIcon(parent), tm_(tm) {
    this->setToolTip(QString::fromStdWString(tm.displayName));
    connect(this, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger) {
            QThread* t = QThread::create([this]() {
                ShellExecuteW(NULL, L"open", tm_.url.c_str(), NULL, NULL, SW_SHOW);
            });
            t->start();

            if (trayIconFlicker_)
                trayIconFlicker_->stop();
        }
    });

    trayIconFlicker_ = new QTimer(this);
    trayIconFlicker_->setInterval(500);
    connect(trayIconFlicker_, &QTimer::timeout, this, [this]() {
        static bool lastShow = true;
        lastShow = !lastShow;
        this->setIcon(lastShow ? QIcon(QString::fromStdWString(tm_.resSavePath)) : QIcon(":/update/images/logo_empty.ico"));
    });
}

FlickerTrayIcon::~FlickerTrayIcon() {
    if (trayIconFlicker_ && trayIconFlicker_->isActive())
        trayIconFlicker_->stop();
}

void FlickerTrayIcon::start() {
    trayIconFlicker_->start();
}

void FlickerTrayIcon::stop() {
    trayIconFlicker_->stop();
}