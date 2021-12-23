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
#include "UpdateWnd.h"
#include "HttpManager.h"
#include "akali/registry.h"
#include "akali/stringencode.h"
#include "akali/md5.h"
#include "akali/trace.h"

using namespace akali;

ListItem::ListItem(const TaskMeta& tm, QWidget* parent) :
    QWidget(parent), taskMeta_(tm) {
    setupUi();

    if (lbl_)
        lbl_->setText(QString::fromStdWString(tm.displayName));

    if (tm.id == -1) {
        chk_->setChecked(true);
        chk_->setEnabled(false);
    }
    else {
        chk_->setChecked(tm.check);
    }
}

bool ListItem::isSelected() {
    if (taskMeta_.id == -1)
        return false;
    return (chk_->isChecked());
}

void ListItem::setupUi() {
    chk_ = new QCheckBox();
    chk_->setObjectName("listItem_chk");
    chk_->setFixedSize(16, 16);
    chk_->setChecked(true);

    lbl_ = new QLabel();
    lbl_->setObjectName("listItem_lbl");

    QHBoxLayout* hl = new QHBoxLayout();
    hl->setContentsMargins(0, 5, 0, 5);
    hl->setSpacing(0);
    hl->addWidget(chk_);
    hl->addSpacing(6);
    hl->addWidget(lbl_);

    this->setLayout(hl);
    this->setFixedHeight(30);
}

UpdateWnd::UpdateWnd(QWidget* parent) :
    FramelessWindow<QWidget>(true, parent) {
    setupUi();
    setWindowTitle(PRODUCT_NAME_CH_A "-升级");

    connect(btnClose_, &QPushButton::clicked, this, [this]() { exitApp(); });
    connect(btnMin_, &QPushButton::clicked, this, [this]() { showMinimized(); });
    connect(btnUpdate_, &QPushButton::clicked, this, &UpdateWnd::onUpdateButtonClicked);
    connect(chkNotShow_, &QCheckBox::stateChanged, this, [this](int state) {
        std::vector<NoRecentShowHistory> norecents = Util::ParseNoRecentHistory(Util::GetNoRecentHistoryJSON());
        bool needAdd = true;
        for (std::vector<NoRecentShowHistory>::iterator it = norecents.begin(); it != norecents.end(); it++) {
            if (it->id == 2) {  // TODO: 确定id
                if (state == Qt::Checked) {
                    it->stamp = std::to_wstring((long long)time(nullptr));
                }
                else {
                    norecents.erase(it);
                }
                needAdd = false;
                break;
            }
        }

        if (needAdd) {
            NoRecentShowHistory nr;
            nr.id = 2;  // TODO: 确定id
            nr.stamp = std::to_wstring((long long)time(nullptr));
            norecents.push_back(nr);
        }

        Util::SetNoRecentHistory(norecents);
    });

    taskCheckTimer_ = new QTimer(this);
    taskCheckTimer_->setInterval(3600000);  // 1h
    connect(taskCheckTimer_, &QTimer::timeout, this, &UpdateWnd::onTaskCheckTimer);
    taskCheckTimer_->start();

    if (taskCheckTimer_->interval() > 600000) {  // 10min
        onTaskCheckTimer();
    }
}

UpdateWnd::~UpdateWnd() {
}

void UpdateWnd::onTaskCheckTimer() {
    HttpManager::Instance()->asyncGetEnvCheckInfo(
        Util::GetOSMajorVersion(),
        Util::GetOSMinorVersion(),
        Util::GetAppVersion(),
        Util::GetChannel(),
        L"",
        [this](const EnvCheckResult& envChkRet) {
            if (!envChkRet.result)
                return;
            if (envChkRet.followAction == FollowAction::EXIT_APP) {
                followAction_ = envChkRet.followAction;
                QMetaObject::invokeMethod(this, [this]() {
                    runTask();
                });
                return;
            }

            HttpManager::Instance()->asyncGetTaskList(
                Util::GetOSMajorVersion(),
                Util::GetOSMinorVersion(),
                Util::GetAppVersion(),
                Util::GetChannel(),
                L"",
                (int)envChkRet.followAction,
                envChkRet,
                [envChkRet, this](FullTask fullTask) {
                    if (envChkRet.followAction == FollowAction::DO_UPDATE && fullTask.taskType != TaskType::INSTALL_SOFT) {
                        return;
                    }

                    if (fullTask_.taskType == TaskType::FLICK_TRAY_ICON || fullTask_.taskType == TaskType::FLICK_TASKBAR) {
                        for (int i = 0; i < fullTask.tms.size(); i++) {
                            std::wstring resUrlMd5 = akali::Utf8ToUnicode(akali::GetStringMd5(fullTask.tms[i].resUrl.c_str(), fullTask.tms[i].resUrl.length() * sizeof(wchar_t)));
                            std::wstring resSavePath = appDataRootFolder().toStdWString() + resUrlMd5 + L".ico";

                            if (_waccess_s(resSavePath.c_str(), 0) == 0) {
                                fullTask.tms[i].resSavePath = resSavePath;
                            }
                            else {
                                std::shared_ptr<teemo::Teemo> tmo = std::make_shared<teemo::Teemo>();
                                teemos_.push_back(tmo);

                                tmo->setThreadNum(2);
                                tmo->setRedirectedUrlCheckEnabled(true);
                                teemo::Result ret = tmo->start(akali::UnicodeToUtf8(fullTask.tms[i].resUrl), akali::UnicodeToUtf8(resSavePath), nullptr, nullptr, nullptr).get();
                                if (ret == teemo::Result::SUCCESSED) {
                                    fullTask.tms[i].resSavePath = resSavePath;
                                }
                                else if (ret == teemo::Result::CANCELED) {
                                    return;
                                }
                                else {
                                    continue;
                                }
                            }
                        }
                    }

                    fullTask_ = fullTask;
                    followAction_ = envChkRet.followAction;

                    QMetaObject::invokeMethod(this, [this]() {
                        runTask();
                    });
                    return;
                });
        });
}

void UpdateWnd::setupUi() {
    setObjectName("update");
    setFixedSize(720, 460);

    btnClose_ = new QPushButton();
    btnClose_->setObjectName("update_btnClose");
    btnClose_->setFixedSize(18, 18);
    btnClose_->setCursor(QCursor(Qt::PointingHandCursor));

    btnMin_ = new QPushButton();
    btnMin_->setObjectName("update_btnMin");
    btnMin_->setFixedSize(18, 18);
    btnMin_->setCursor(QCursor(Qt::PointingHandCursor));

    list_ = new QListWidget();
    list_->setObjectName("update_list");
    list_->setSelectionMode(QAbstractItemView::NoSelection);

    btnUpdate_ = new QPushButton("一键升级");
    btnUpdate_->setObjectName("client_btnUpdate");
    btnUpdate_->setFixedSize(200, 54);
    btnUpdate_->setCursor(QCursor(Qt::PointingHandCursor));

    chkNotShow_ = new QCheckBox("近期不再提示");
    chkNotShow_->setObjectName("client_chkNotShow");

    widgetTop_ = new QWidget();
    widgetTop_->setObjectName("client_widgetTop");
    widgetTop_->setFixedSize(720, 364);
    {
        QHBoxLayout* hl1 = new QHBoxLayout();
        hl1->addStretch();
        hl1->addWidget(btnMin_);
        hl1->addWidget(btnClose_);

        QLabel* lblMsg = new QLabel("升级功能");
        lblMsg->setObjectName("client_lblMsg");

        QVBoxLayout* vl1 = new QVBoxLayout();
        vl1->addWidget(lblMsg);
        vl1->addSpacing(10);
        vl1->addWidget(list_);
        vl1->addStretch();

        QHBoxLayout* hl2 = new QHBoxLayout();
        hl2->addSpacing(420);
        hl2->addLayout(vl1);
        hl2->addStretch();

        QVBoxLayout* vl2 = new QVBoxLayout();
        vl2->setContentsMargins(20, 20, 20, 20);
        vl2->addLayout(hl1);
        vl2->addSpacing(50);
        vl2->addLayout(hl2);
        vl2->addStretch();

        widgetTop_->setLayout(vl2);
    }

    widgetBottom_ = new QWidget();
    widgetBottom_->setObjectName("client_widgetBottom");
    widgetBottom_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    {
        QVBoxLayout* vl1 = new QVBoxLayout();
        vl1->addStretch();
        vl1->addWidget(chkNotShow_);

        QHBoxLayout* hl1 = new QHBoxLayout();
        hl1->setContentsMargins(0, 20, 20, 20);
        hl1->addSpacing(261);
        hl1->addWidget(btnUpdate_);
        hl1->addSpacing(130);
        hl1->addLayout(vl1);
        hl1->addStretch();

        widgetBottom_->setLayout(hl1);
    }

    QVBoxLayout* vl1 = new QVBoxLayout();
    vl1->setSpacing(0);
    vl1->setContentsMargins(0, 0, 0, 0);
    vl1->addWidget(widgetTop_);
    vl1->addWidget(widgetBottom_);

    centalWidget_ = new QWidget();
    centalWidget_->setObjectName("client_centralWidget");
    centalWidget_->setLayout(vl1);

    QHBoxLayout* hl1 = new QHBoxLayout();
    hl1->setSpacing(0);
    hl1->setContentsMargins(0, 0, 0, 0);
    hl1->addWidget(centalWidget_);

    this->setLayout(hl1);
    this->setTitlebar({widgetTop_});
}

void UpdateWnd::addItem(const TaskMeta& tm) {
    ListItem* item = new ListItem(tm);
    if (list_) {
        QListWidgetItem* w = new QListWidgetItem();
        w->setSizeHint(QSize(120, 30));
        list_->addItem(w);
        list_->setItemWidget(w, item);
    }
}

void UpdateWnd::exitApp() {
    this->hide();
    for (int i = 0; i < teemos_.size(); i++) {
        if (teemos_[i]) {
            teemos_[i]->stop();
        }
    }
    teemos_.clear();

    if (softInstallThread_ && softInstallThread_->isRunning())
        softInstallThread_->wait();

    for (int i = 0; i < trayIcons_.size(); i++) {
        if (trayIcons_[i]) {
            trayIcons_[i]->hide();
        }
    }
    trayIcons_.clear();

    for (int i = 0; i < taskbars_.size(); i++) {
        if (taskbars_[i]) {
            taskbars_[i]->close();
        }
    }
    taskbars_.clear();

    qApp->quit();
}

void UpdateWnd::runTask() {
    if (followAction_ == FollowAction::EXIT_APP) {
        exitApp();
    }
    else if (followAction_ == FollowAction::DO_UPDATE) {
        for (int i = 0; i < fullTask_.tms.size(); i++) {
            addItem(fullTask_.tms[i]);
        }
        this->show();
    }
    else if (followAction_ == FollowAction::RUN_TASK) {
        if (fullTask_.taskType == 3) {
            for (int i = 0; i < trayIcons_.size(); i++) {
                if (trayIcons_[i])
                    trayIcons_[i]->hide();
            }
            trayIcons_.clear();

            for (int i = 0; i < fullTask_.tms.size(); i++) {
                FlickerTrayIcon* pTray = new FlickerTrayIcon(fullTask_.tms[i], this);
                trayIcons_.push_back(pTray);

                pTray->show();
            }
        }
    }

    followAction_ = FollowAction::FA_UNKNOWN;
}

void UpdateWnd::onUpdateButtonClicked() {
    std::vector<TaskMeta> tms;
    for (int i = 0; i < list_->count(); i++) {
        ListItem* item = (ListItem*)(list_->itemWidget(list_->item(i)));
        if (item) {
            if (item->isSelected()) {
                tms.push_back(item->taskMeta());
            }
        }
    }

    if (tms.size() == 0)
        return;

    this->hide();
    progressWnd_ = new UpdateProgressWnd(this);
    connect(progressWnd_, &UpdateProgressWnd::finished, this, [this]() {
        progressWnd_->deleteLater();
        exitApp();
    });
    progressWnd_->open();

    softInstallThread_ = QThread::create([tms, this]() {
        for (int i = 0; i < tms.size(); i++) {
            std::wstring resSavePath = appDataRootFolder().toStdWString() + tms[i].fileName;
            std::shared_ptr<teemo::Teemo> tmo = std::make_shared<teemo::Teemo>();
            teemos_.push_back(tmo);

            tmo->setThreadNum(2);
            tmo->setRedirectedUrlCheckEnabled(true);
            if (tms[i].md5.length() > 0) {
                tmo->setHashVerifyPolicy(teemo::HashVerifyPolicy::ALWAYS, teemo::HashType::MD5, akali::UnicodeToUtf8(tms[i].md5));
            }
            teemo::Result ret = tmo->start(akali::UnicodeToUtf8(tms[i].resUrl), akali::UnicodeToUtf8(resSavePath), nullptr, nullptr, nullptr).get();
            if (ret == teemo::Result::SUCCESSED) {
                if (!Util::CreateProcessWarpper(resSavePath, tms[i].cmd)) {
                    akali::TraceMsgW(L"[update] %s %s failed, %d", resSavePath.c_str(), tms[i].cmd.c_str(), GetLastError());
                }
            }
            else if (ret == teemo::Result::CANCELED) {
                return;
            }
            else {
                continue;
            }
        }

        QMetaObject::invokeMethod(this, [this]() {
            onUpdateFinished();
        });
    });
    softInstallThread_->start();
}

void UpdateWnd::onUpdateFinished() {
    int appver = Util::GetAppVersion();
    int newAppVer = appver + (1 >> 3);
    Util::SetAppVersion(newAppVer);

    if (progressWnd_)
        progressWnd_->setFinished();
}
