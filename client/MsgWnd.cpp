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
#include "MsgWnd.h"

MsgWnd::MsgWnd(QWidget* parent) :
    FramelessWindow<QDialog>(true, parent) {
    setAttribute(Qt::WA_DeleteOnClose, true);
    setupUi();
    setBtn2Prefer(true);

    connect(pushButtonClose_, &QPushButton::clicked, this, [this]() {
        done(ClickedBtn::CloseBtn);
    });

    connect(pushButton1_, &QPushButton::clicked, this, [this]() {
        done(ClickedBtn::Btn1);
    });

    connect(pushButton2_, &QPushButton::clicked, this, [this]() {
        done(ClickedBtn::Btn2);
    });
}

MsgWnd::~MsgWnd() {
}

void MsgWnd::setTitle(const QString& s) {
    labelTitle_->setText(s);
}

void MsgWnd::setMsg(const QString& s) {
    if (s.length() >= 50)  // a simple way
        labelMsg_->setWordWrap(true);
    labelMsg_->setText(s);
    this->adjustSize();
}

void MsgWnd::setBtn1Text(const QString& s) {
    pushButton1_->setText(s);
}

void MsgWnd::setBtn2Text(const QString& s) {
    pushButton2_->setText(s);
}

void MsgWnd::setBtn2Visible(bool visible) {
    pushButton2_->setVisible(visible);
}

void MsgWnd::setBtn2Prefer(bool prefer) {
    pushButton1_->setProperty("clr", prefer ? "blue" : "red");
    pushButton2_->setProperty("clr", prefer ? "red" : "blue");

    REFRESH_STYLE(pushButton1_);
    REFRESH_STYLE(pushButton2_);
}

void MsgWnd::setupUi() {
    setObjectName("MsgWnd");
    this->setWindowFlags(this->windowFlags() | Qt::Tool | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    this->setMinimumWidth(470);
    this->setMaximumWidth(1000);

    centralWidget_ = new QWidget();
    centralWidget_->setObjectName("MsgWnd_centralWidget");

    pushButtonClose_ = new QPushButton();
    pushButtonClose_->setObjectName("MsgWnd_pushButtonClose");
    pushButtonClose_->setCursor(QCursor(Qt::PointingHandCursor));
    pushButtonClose_->setFixedSize(22, 22);

    labelTitle_ = new QLabel();
    labelTitle_->setObjectName("MsgWnd_labelTitle");
    labelTitle_->setWordWrap(true);
    labelTitle_->setFixedHeight(22);

    labelMsg_ = new QLabel();
    labelMsg_->setObjectName("MsgWnd_labelMsg");
    labelMsg_->setWordWrap(false);
    labelMsg_->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    labelMsg_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    pushButton1_ = new QPushButton("PushButton1");
    pushButton1_->setObjectName("MsgWnd_pushButton1");
    pushButton1_->setCursor(QCursor(Qt::PointingHandCursor));
    pushButton1_->setFixedSize(112, 42);

    pushButton2_ = new QPushButton("PushButton2");
    pushButton2_->setObjectName("MsgWnd_pushButton2");
    pushButton2_->setCursor(QCursor(Qt::PointingHandCursor));
    pushButton2_->setFixedSize(112, 42);

    QHBoxLayout* hl1 = new QHBoxLayout();
    hl1->setSpacing(0);
    hl1->setContentsMargins(0, 0, 0, 0);
    hl1->addSpacing(20);
    hl1->addWidget(labelTitle_);
    hl1->addStretch();
    hl1->addWidget(pushButtonClose_);
    hl1->addSpacing(20);

    QHBoxLayout* hl2 = new QHBoxLayout();
    hl2->setSpacing(0);
    hl2->setContentsMargins(0, 0, 0, 0);
    hl2->addSpacing(20);
    hl2->addWidget(labelMsg_);
    hl2->addSpacing(20);

    QHBoxLayout* hl3 = new QHBoxLayout();
    hl3->setSpacing(0);
    hl3->setContentsMargins(0, 0, 0, 0);
    hl3->addStretch();
    hl3->addWidget(pushButton1_);
    hl3->addSpacing(30);
    hl3->addWidget(pushButton2_);
    hl3->addStretch();

    QVBoxLayout* vl = new QVBoxLayout();
    vl->setSpacing(0);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->addSpacing(10);
    vl->addLayout(hl1);
    vl->addSpacing(30);
    vl->addLayout(hl2);
    vl->addSpacing(30);
    vl->addLayout(hl3);
    vl->addSpacing(20);

    centralWidget_->setLayout(vl);

    QVBoxLayout* vl2 = new QVBoxLayout();
    vl2->setSpacing(0);
    vl2->setContentsMargins(12, 12, 12, 12);
    vl2->addWidget(centralWidget_);

    this->setLayout(vl2);
    this->setTitlebar({centralWidget_, labelTitle_, labelMsg_});

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setOffset(0, 0);
    shadow->setColor(QColor("#F11D1D"));
    shadow->setBlurRadius(12);
    centralWidget_->setGraphicsEffect(shadow);
}
