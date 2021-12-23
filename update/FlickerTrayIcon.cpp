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