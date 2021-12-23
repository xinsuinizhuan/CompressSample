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
#include "UpdateProgressWnd.h"

UpdateProgressWnd::UpdateProgressWnd(QWidget* parent) :
    FramelessWindow<QDialog>(true, parent) {
    setupUi();
    btnFinish_->setEnabled(false);
    btnClose_->setVisible(false);

    connect(btnClose_, &QPushButton::clicked, this, [this]() { done(0); });
    connect(btnMin_, &QPushButton::clicked, this, [this]() { showMinimized(); });
    connect(btnFinish_, &QPushButton::clicked, this, [this]() { done(0); });
}

UpdateProgressWnd::~UpdateProgressWnd() {
}

void UpdateProgressWnd::setupUi() {
    setObjectName("update_finished");
    setFixedSize(720, 460);

    btnClose_ = new QPushButton();
    btnClose_->setObjectName("update_btnClose");
    btnClose_->setFixedSize(18, 18);
    btnClose_->setCursor(QCursor(Qt::PointingHandCursor));

    btnMin_ = new QPushButton();
    btnMin_->setObjectName("update_btnMin");
    btnMin_->setFixedSize(18, 18);
    btnMin_->setCursor(QCursor(Qt::PointingHandCursor));

    btnFinish_ = new QPushButton("完成");
    btnFinish_->setObjectName("client_btnFinish");
    btnFinish_->setFixedSize(200, 54);
    btnFinish_->setCursor(QCursor(Qt::PointingHandCursor));

    widgetTop_ = new QWidget();
    widgetTop_->setObjectName("client_widgetTop");
    widgetTop_->setFixedSize(720, 364);
    {
        QHBoxLayout* hl1 = new QHBoxLayout();
        hl1->addStretch();
        hl1->addWidget(btnMin_);
        hl1->addWidget(btnClose_);

        lblMsg_ = new QLabel(PRODUCT_NAME_CH_A "升级中...");
        lblMsg_->setObjectName("client_lblMsg");

        QHBoxLayout* hl2 = new QHBoxLayout();
        hl2->addStretch();
        hl2->addWidget(lblMsg_);
        hl2->addStretch();

        QVBoxLayout* vl2 = new QVBoxLayout();
        vl2->setContentsMargins(20, 20, 20, 20);
        vl2->addSpacing(10);
        vl2->addLayout(hl2);
        vl2->addStretch();

        widgetTop_->setLayout(vl2);
    }

    widgetBottom_ = new QWidget();
    widgetBottom_->setObjectName("client_widgetBottom");
    widgetBottom_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    {
        QHBoxLayout* hl1 = new QHBoxLayout();
        hl1->setContentsMargins(0, 20, 20, 20);
        hl1->addStretch();
        hl1->addWidget(btnFinish_);
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

void UpdateProgressWnd::setFinished() {
    btnFinish_->setText("完成");
    btnFinish_->setEnabled(true);
    btnClose_->setVisible(true);
    lblMsg_->setText("亲~" PRODUCT_NAME_CH_A "软件升级完成！");
}
