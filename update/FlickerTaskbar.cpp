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
#include "FlickerTaskbar.h"

FlickerTaskbar::FlickerTaskbar(const TaskMeta& tm, QWidget* parent) :
    QWidget(parent), tm_(tm) {
    setAttribute(Qt::WA_TranslucentBackground);
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::FramelessWindowHint);

    setWindowIcon(QIcon(QString::fromStdWString(tm_.resSavePath)));
    setWindowTitle(QString::fromStdWString(tm_.displayName));
    setFixedSize(10, 10);
}

FlickerTaskbar::~FlickerTaskbar() {
}

void FlickerTaskbar::start() {
    if (!this->isMinimized())
        this->showMinimized();

    FLASHWINFO fw = {sizeof(fw)};
    fw.dwFlags = FLASHW_TIMERNOFG | FLASHW_TRAY;
    fw.dwTimeout = 0;
    fw.uCount = 0;
    fw.hwnd = (HWND)this->winId();
    ::FlashWindowEx(&fw);
}

void FlickerTaskbar::stop() {
    FLASHWINFO fw = {sizeof(fw)};
    fw.dwFlags = FLASHW_STOP;
    fw.dwTimeout = 0;
    fw.uCount = 0;
    fw.hwnd = (HWND)this->winId();
    ::FlashWindowEx(&fw);
}

bool FlickerTaskbar::nativeEvent(const QByteArray& eventType, void* message, long* result) {
    if (eventType == "windows_generic_MSG") {
        MSG* pmsg = (MSG*)message;
        if (pmsg->message == WM_ACTIVATE) {
            if (pmsg->wParam == WA_ACTIVE || pmsg->wParam == WA_CLICKACTIVE) {
                std::wstring url = tm_.url;
                if (url.length() > 0) {
                    std::thread t = std::thread([url]() {
                        ::ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOW);
                    });
                    t.detach();
                }
                stop();
                close();
            }
            return true;
        }
    }
    return false;
}
