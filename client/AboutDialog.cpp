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
#include "AboutDialog.h"

AboutDialog::AboutDialog(QWidget* parent /*= Q_NULLPTR*/) :
    FramelessWindow<QDialog>(true, parent) {
    setupUi();

    connect(pushButtonClose_, &QPushButton::clicked, this, [this]() {
        done(1);
    });

    connect(pushButtonOK_, &QPushButton::clicked, this, [this]() {
        done(0);
    });

    connect(pushButtonLink_, &QPushButton::clicked, this, [this]() {
        std::wstring url = pushButtonLink_->text().toStdWString();
        std::thread t = std::thread([url]() {
            ::ShellExecuteW(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOW);
        });
        t.detach();
    });
}

AboutDialog::~AboutDialog() {
}

void AboutDialog::setupUi() {
    setObjectName("compressAboutDialog");
    setWindowTitle(PRODUCT_NAME_CH_A);
    setFixedSize(428, 174);

    widgetTitle_ = new QWidget();
    widgetTitle_->setObjectName("compressAboutDialog_widgetTitle");
    widgetTitle_->setFixedHeight(28);
    {
        pushButtonClose_ = new QPushButton();
        pushButtonClose_->setObjectName("compressAboutDialog_pushButtonClose");
        pushButtonClose_->setFixedSize(18, 18);
        pushButtonClose_->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

        QLabel* lblTitle = new QLabel("压缩文件-" PRODUCT_NAME_CH_A);
        lblTitle->setObjectName("compressAboutDialog_lblTitle");

        QLabel* lblLogo = new QLabel();
        lblLogo->setObjectName("compressAboutDialog_lblLogo");
        lblLogo->setFixedSize(18, 16);

        QHBoxLayout* hl = new QHBoxLayout();
        hl->setContentsMargins(10, 0, 10, 0);
        hl->addWidget(lblLogo);
        hl->addWidget(lblTitle);
        hl->addStretch();
        hl->addWidget(pushButtonClose_);

        widgetTitle_->setLayout(hl);
    }

    QHBoxLayout* hl1 = new QHBoxLayout();
    {
        QLabel* lblLogoBig = new QLabel();
        lblLogoBig->setObjectName("compressAboutDialog_lblLogoBig");
        lblLogoBig->setFixedSize(48, 42);

        QVBoxLayout* vl = new QVBoxLayout();
        {
            vl->setContentsMargins(0, 0, 0, 0);
            vl->setSpacing(0);
            QLabel* lblMsg = new QLabel(PRODUCT_NAME_CH_A "，给您带来不一样的压缩服务");
            lblMsg->setObjectName("compressAboutDialog_lblMsg");

            pushButtonLink_ = new QPushButton("www.xxxxxxxx.com");
            pushButtonLink_->setObjectName("compressAboutDialog_pushButtonLink");
            pushButtonLink_->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));

            QHBoxLayout* hl22 = new QHBoxLayout();
            hl22->addWidget(pushButtonLink_);
            hl22->addStretch();

            vl->addWidget(lblMsg);
            vl->addLayout(hl22);
        }

        hl1->addStretch();
        hl1->addWidget(lblLogoBig);
        hl1->addLayout(vl);
        hl1->addStretch();
    }

    pushButtonOK_ = new QPushButton("确定");
    pushButtonOK_->setObjectName("compressAboutDialog_pushButtonOK");
    pushButtonOK_->setFixedSize(78, 30);

    QHBoxLayout* hlButton = new QHBoxLayout();
    hlButton->addStretch();
    hlButton->addWidget(pushButtonOK_);
    hlButton->addStretch();

    QVBoxLayout* vl = new QVBoxLayout();
    vl->setContentsMargins(0, 0, 0, 0);
    vl->addSpacing(34);
    vl->addLayout(hl1);
    vl->addSpacing(18);
    vl->addLayout(hlButton);
    vl->addStretch();

    QVBoxLayout* vlMain = new QVBoxLayout();
    vlMain->setContentsMargins(0, 0, 0, 0);
    vlMain->addWidget(widgetTitle_);
    vlMain->addLayout(vl);

    centralWidget_ = new QWidget();
    centralWidget_->setObjectName("compressAboutDialog_centralWidget");
    centralWidget_->setLayout(vlMain);

    QHBoxLayout* hlMain = new QHBoxLayout();
    hlMain->setContentsMargins(0, 0, 0, 0);
    hlMain->addWidget(centralWidget_);

    this->setLayout(hlMain);
    this->setTitlebar({widgetTitle_});
}
