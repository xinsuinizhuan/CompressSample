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
#include <ShlObj.h>
#include "ExtractOptionDialog.h"

ExtractOptionDialog::ExtractOptionDialog(QWidget* parent /*= Q_NULLPTR*/) :
    FramelessWindow<QDialog>(true, parent) {
    setupUi();

    connect(pushButtonClose_, &QPushButton::clicked, this, [this]() {
        done(1);
    });

    connect(pushButtonExtract_, &QPushButton::clicked, this, [this]() {
        done(0);
    });

    connect(pushButtonBrowser_, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this,
                                                        "解压目录",
                                                        lineEditFolder_->text(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!dir.isEmpty())
            lineEditFolder_->setText(QDir::toNativeSeparators(dir));
    });
}

ExtractOptionDialog::~ExtractOptionDialog() {
}

void ExtractOptionDialog::setDefaultFolder(const QString& s) {
    if (lineEditFolder_)
        lineEditFolder_->setText(QDir::toNativeSeparators(s));
}

QString ExtractOptionDialog::selectedFolder() {
    return lineEditFolder_ ? lineEditFolder_->text() : "";
}

void ExtractOptionDialog::setupUi() {
    setObjectName("extractOptionDialog");
    setWindowTitle("解压到");
    setFixedSize(480, 198);

    widgetTitle_ = new QWidget();
    widgetTitle_->setObjectName("extractOptionDialog_widgetTitle");
    widgetTitle_->setFixedHeight(28);
    {
        pushButtonClose_ = new QPushButton();
        pushButtonClose_->setObjectName("extractOptionDialog_pushButtonClose");
        pushButtonClose_->setFixedSize(18, 18);
        pushButtonClose_->setCursor(QCursor(Qt::PointingHandCursor));

        QLabel* lblTitle = new QLabel("解压文件");
        lblTitle->setObjectName("extractOptionDialog_lblTitle");

        QLabel* lblLogo = new QLabel();
        lblLogo->setObjectName("extractOptionDialog_lblLogo");
        lblLogo->setFixedSize(18, 16);

        QHBoxLayout* hl = new QHBoxLayout();
        hl->setContentsMargins(10, 0, 10, 0);
        hl->addWidget(lblLogo);
        hl->addWidget(lblTitle);
        hl->addStretch();
        hl->addWidget(pushButtonClose_);

        widgetTitle_->setLayout(hl);
    }

    QLabel* lblTips = new QLabel("目标路径（如果不存在将被创建）:");
    lblTips->setObjectName("extractOptionDialog_lblTips");

    widgetFolder_ = new QWidget();
    widgetFolder_->setObjectName("extractOptionDialog_widgetFolder");
    {
        lineEditFolder_ = new QLineEdit();
        lineEditFolder_->setObjectName("extractOptionDialog_lineEditFolder");
        lineEditFolder_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        pushButtonBrowser_ = new QPushButton();
        pushButtonBrowser_->setObjectName("extractOptionDialog_pushButtonBrowser");
        pushButtonBrowser_->setFixedSize(20, 20);
        pushButtonBrowser_->setCursor(QCursor(Qt::PointingHandCursor));

        QHBoxLayout* hl = new QHBoxLayout();
        hl->setContentsMargins(10, 2, 10, 2);
        hl->addWidget(lineEditFolder_);
        hl->addWidget(pushButtonBrowser_);

        widgetFolder_->setLayout(hl);
    }

    pushButtonExtract_ = new QPushButton("立即解压");
    pushButtonExtract_->setObjectName("extractOptionDialog_pushButtonExtract");
    pushButtonExtract_->setFixedSize(78, 38);
    pushButtonExtract_->setCursor(QCursor(Qt::PointingHandCursor));

    QHBoxLayout* hlButton = new QHBoxLayout();
    hlButton->addStretch();
    hlButton->addWidget(pushButtonExtract_);

    QVBoxLayout* vl = new QVBoxLayout();
    vl->setContentsMargins(10, 10, 10, 10);
    vl->addWidget(lblTips);
    vl->addWidget(widgetFolder_);
    vl->addLayout(hlButton);

    QVBoxLayout* vlMain = new QVBoxLayout();
    vlMain->setContentsMargins(0, 0, 0, 0);
    vlMain->addWidget(widgetTitle_);
    vlMain->addLayout(vl);

    centralWidget_ = new QWidget();
    centralWidget_->setObjectName("extractOptionDialog_centralWidget");
    centralWidget_->setLayout(vlMain);

    QHBoxLayout* hlMain = new QHBoxLayout();
    hlMain->setContentsMargins(12, 12, 12, 12);
    hlMain->addWidget(centralWidget_);

    this->setLayout(hlMain);
    this->setTitlebar({widgetTitle_});

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setOffset(0, 2);
    shadow->setColor(QColor("#AAAAAA"));
    shadow->setBlurRadius(12);
    centralWidget_->setGraphicsEffect(shadow);
}
