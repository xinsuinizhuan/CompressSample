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

#include <QtWidgets>
#include "FramelessWindow.hpp"
#include "HttpManager.h"
#include "FlickerTrayIcon.h"
#include "FlickerTaskbar.h"
#include "teemo/teemo.h"
#include "UpdateProgressWnd.h"

class ListItem : public QWidget {
    Q_OBJECT
   public:
    ListItem(const TaskMeta& tm, QWidget* parent = Q_NULLPTR);

    bool isSelected();
    TaskMeta taskMeta() { return taskMeta_; }

   private:
    void setupUi();

   private:
    TaskMeta taskMeta_;
    QCheckBox* chk_ = Q_NULLPTR;
    QLabel* lbl_ = Q_NULLPTR;
};

class UpdateWnd : public FramelessWindow<QWidget> {
    Q_OBJECT
   public:
    UpdateWnd(QWidget* parent = Q_NULLPTR);
    ~UpdateWnd();

   private:
    void setupUi();
    void addItem(const TaskMeta& tm);
   private slots:
    void onTaskCheckTimer();
    void exitApp();
    void runTask();
    void onUpdateButtonClicked();
    void onUpdateFinished();
   private:
    QPushButton* btnClose_ = Q_NULLPTR;
    QPushButton* btnMin_ = Q_NULLPTR;
    QPushButton* btnUpdate_ = Q_NULLPTR;
    QListWidget* list_ = Q_NULLPTR;
    QCheckBox* chkNotShow_ = Q_NULLPTR;

    QWidget* widgetTop_ = Q_NULLPTR;
    QWidget* widgetBottom_ = Q_NULLPTR;
    QWidget* centalWidget_ = Q_NULLPTR;

    QVector<QPointer<FlickerTrayIcon>> trayIcons_;
    QVector<QPointer<FlickerTaskbar>> taskbars_;

    QTimer* taskCheckTimer_ = Q_NULLPTR;
    UpdateProgressWnd* progressWnd_ = Q_NULLPTR;

   private:
    FollowAction followAction_ = FollowAction::FA_UNKNOWN;
    FullTask fullTask_;
    std::vector<std::shared_ptr<teemo::Teemo>> teemos_;
    QThread* softInstallThread_ = Q_NULLPTR;
};
