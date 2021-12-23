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
