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
