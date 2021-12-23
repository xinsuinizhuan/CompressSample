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
#include "CompressOptionDialog.h"

CompressOptionDialog::CompressOptionDialog(QWidget* parent /*= Q_NULLPTR*/) :
    FramelessWindow<QDialog>(true, parent) {
    setupUi();

    connect(pushButtonClose_, &QPushButton::clicked, this, [this]() {
        done(1);
    });

    connect(pushButtonCompress_, &QPushButton::clicked, this, [this]() {
        done(0);
    });

    connect(pushButtonBrowser_, &QPushButton::clicked, this, [this]() {
        QString folder = QFileInfo(lineEditPath_->text()).absoluteDir().absolutePath();
        QString path = QFileDialog::getSaveFileName(this,
                                                    "压缩文件",
                                                    folder,
                                                    "压缩文件 (*.zip *.7z *.rar);;Zip (*.zip);;7Zip (*.7z);;RAR (*.rar)");
        if (!path.isEmpty())
            lineEditPath_->setText(QDir::toNativeSeparators(path));
    });

    connect(btnGroup_, &QButtonGroup::idToggled, this, [this](int id, bool checked) {
        if (checked) {
            QString path = lineEditPath_->text();
            QFileInfo fi(path);
            QString strFolder = fi.absoluteDir().absolutePath();
            QString strFileNameWithoutSuffix = fi.fileName();
            int pos = strFileNameWithoutSuffix.indexOf(".");
            if (pos != -1) {
                strFileNameWithoutSuffix = strFileNameWithoutSuffix.mid(0, pos);
            }

            if (id == 0) {
                QString newPath = QDir(strFolder).absoluteFilePath(strFileNameWithoutSuffix + ".zip");
                lineEditPath_->setText(QDir::toNativeSeparators(newPath));
            }
            else if (id == 1) {
                QString newPath = QDir(strFolder).absoluteFilePath(strFileNameWithoutSuffix + ".7z");
                lineEditPath_->setText(QDir::toNativeSeparators(newPath));
            }
        }
    });
}

CompressOptionDialog::~CompressOptionDialog() {
}

void CompressOptionDialog::setDefaultPath(const QString& s) {
    if (lineEditPath_)
        lineEditPath_->setText(s);
}

QString CompressOptionDialog::selectedPath() {
    return lineEditPath_ ? lineEditPath_->text() : "";
}

void CompressOptionDialog::setupUi() {
    setObjectName("compressOptionDialog");
    setWindowTitle("压缩到");
    setFixedSize(480, 198);

    widgetTitle_ = new QWidget();
    widgetTitle_->setObjectName("compressOptionDialog_widgetTitle");
    widgetTitle_->setFixedHeight(28);
    {
        pushButtonClose_ = new QPushButton();
        pushButtonClose_->setObjectName("compressOptionDialog_pushButtonClose");
        pushButtonClose_->setFixedSize(18, 18);
        pushButtonClose_->setCursor(QCursor(Qt::PointingHandCursor));

        QLabel* lblTitle = new QLabel("压缩文件");
        lblTitle->setObjectName("compressOptionDialog_lblTitle");

        QLabel* lblLogo = new QLabel();
        lblLogo->setObjectName("compressOptionDialog_lblLogo");
        lblLogo->setFixedSize(18, 16);

        QHBoxLayout* hl = new QHBoxLayout();
        hl->setContentsMargins(10, 0, 10, 0);
        hl->addWidget(lblLogo);
        hl->addWidget(lblTitle);
        hl->addStretch();
        hl->addWidget(pushButtonClose_);

        widgetTitle_->setLayout(hl);
    }

    widgetFolder_ = new QWidget();
    widgetFolder_->setObjectName("compressOptionDialog_widgetPath");
    widgetFolder_->setFixedHeight(36);
    {
        lineEditPath_ = new QLineEdit();
        lineEditPath_->setObjectName("compressOptionDialog_lineEditPath");
        lineEditPath_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        pushButtonBrowser_ = new QPushButton();
        pushButtonBrowser_->setObjectName("compressOptionDialog_pushButtonBrowser");
        pushButtonBrowser_->setFixedSize(20, 20);
        pushButtonBrowser_->setCursor(QCursor(Qt::PointingHandCursor));

        QHBoxLayout* hl = new QHBoxLayout();
        hl->setContentsMargins(10, 2, 10, 2);
        hl->addWidget(lineEditPath_);
        hl->addWidget(pushButtonBrowser_);

        widgetFolder_->setLayout(hl);
    }

    QHBoxLayout* hlFormat = new QHBoxLayout();
    {
        radioButtonSpeed_ = new QRadioButton();
        radioButtonSpeed_->setObjectName("compressOptionDialog_radioButtonSpeed");
        radioButtonSpeed_->setText("速度最快");
        radioButtonSpeed_->setChecked(true);
        radioButtonSpeed_->setCursor(QCursor(Qt::PointingHandCursor));

        radioButtonSize_ = new QRadioButton();
        radioButtonSize_->setObjectName("compressOptionDialog_radioButtonSize");
        radioButtonSize_->setText("体积最小");
        radioButtonSize_->setCursor(QCursor(Qt::PointingHandCursor));

        btnGroup_ = new QButtonGroup();
        btnGroup_->addButton(radioButtonSpeed_, 0);
        btnGroup_->addButton(radioButtonSize_, 1);

        QLabel* lbl = new QLabel("压缩配置: ");
        lbl->setObjectName("compressOptionDialog_lblConfig");

        hlFormat->addWidget(lbl);
        hlFormat->addWidget(radioButtonSpeed_);
        hlFormat->addWidget(radioButtonSize_);
        hlFormat->addStretch();
    }

    pushButtonCompress_ = new QPushButton("立即压缩");
    pushButtonCompress_->setObjectName("compressOptionDialog_pushButtonCompress");
    pushButtonCompress_->setFixedSize(78, 38);
    pushButtonCompress_->setCursor(QCursor(Qt::PointingHandCursor));

    QHBoxLayout* hlButton = new QHBoxLayout();
    hlButton->addStretch();
    hlButton->addWidget(pushButtonCompress_);

    QVBoxLayout* vl = new QVBoxLayout();
    vl->setContentsMargins(10, 10, 10, 10);
    vl->addStretch();
    vl->addWidget(widgetFolder_);
    vl->addSpacing(6);
    vl->addLayout(hlFormat);
    vl->addLayout(hlButton);
    vl->addStretch();

    QVBoxLayout* vlMain = new QVBoxLayout();
    vlMain->setContentsMargins(0, 0, 0, 0);
    vlMain->addWidget(widgetTitle_);
    vlMain->addLayout(vl);

    centralWidget_ = new QWidget();
    centralWidget_->setObjectName("compressOptionDialog_centralWidget");
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
