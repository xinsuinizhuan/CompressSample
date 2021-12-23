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
#include "CompressProgressDialog.h"
#include "MsgWnd.h"

CompressProgressDialog::CompressProgressDialog(QWidget* parent /*= Q_NULLPTR*/) :
    FramelessWindow<QDialog>(true, parent) {
    setupUi();

    connect(pushButtonClose_, &QPushButton::clicked, this, [this]() {
        diskCompressor_->cancel();
        done(1);
    });

    connect(pushButtonCancel_, &QPushButton::clicked, this, [this]() {
        diskCompressor_->cancel();
        done(1);
    });

    connect(pushButtonOK_, &QPushButton::clicked, this, [this]() {
        done(0);
    });

    diskCompressor_ = new DiskCompressor();
    connect(diskCompressor_, &DiskCompressor::error, this, [this](short err) {
        pushButtonOK_->setVisible(true);
        pushButtonCancel_->setVisible(false);

        MsgWnd* pMsgWnd = new MsgWnd(this);
        connect(pMsgWnd, &MsgWnd::finished, this, [pMsgWnd, this]() {
            pMsgWnd->deleteLater();
            done(1);
        });
        pMsgWnd->setBtn1Text("确定");
        pMsgWnd->setBtn2Visible(false);
        pMsgWnd->setMsg(QString("压缩失败！错误码：%1").arg(err));
        pMsgWnd->setTitle(PRODUCT_NAME_CH_A);
        pMsgWnd->open();
    });

    connect(diskCompressor_, &DiskCompressor::progress, this,
            [this](QString curEntry, int processedEntry, int totalEntry, qint64 processedBytes, qint64 totalBytes) {
                int progress = (double)processedBytes * 100.f / (double)totalBytes;
                setProgress(QDir::toNativeSeparators(curEntry), progress);
            });

    connect(diskCompressor_, &DiskCompressor::finished, this, [this]() {
        setProgress(QString("压缩到：%1").arg(QDir::toNativeSeparators(targetPath_)), 100);
        pushButtonOK_->setVisible(true);
        pushButtonCancel_->setVisible(false);

        if (autoCloseAfterFinished_)
            done(0);
    });
}

CompressProgressDialog::~CompressProgressDialog() {
}

void CompressProgressDialog::compress(const QStringList& files, const QString& compressTarget, short compressFormat) {
    targetPath_ = compressTarget;

    diskCompressor_->clear();
    diskCompressor_->setFileName(compressTarget);
    diskCompressor_->setArchiveFormat(compressFormat);
    diskCompressor_->addFiles(files);
    diskCompressor_->start();
}

void CompressProgressDialog::setProgress(const QString& curFile, int progress) {
    if (labelCurrentFile_)
        labelCurrentFile_->setText(curFile);

    progressBar_->setValue(progress);
}

void CompressProgressDialog::setupUi() {
    setObjectName("compressProgressDialog");
    setWindowTitle("压缩到");
    setFixedSize(480, 198);

    widgetTitle_ = new QWidget();
    widgetTitle_->setObjectName("compressProgressDialog_widgetTitle");
    widgetTitle_->setFixedHeight(28);
    {
        pushButtonClose_ = new QPushButton();
        pushButtonClose_->setObjectName("compressProgressDialog_pushButtonClose");
        pushButtonClose_->setFixedSize(18, 18);
        pushButtonClose_->setCursor(QCursor(Qt::PointingHandCursor));

        QLabel* lblTitle = new QLabel("压缩文件");
        lblTitle->setObjectName("compressProgressDialog_lblTitle");

        QLabel* lblLogo = new QLabel();
        lblLogo->setObjectName("compressProgressDialog_lblLogo");
        lblLogo->setFixedSize(18, 16);

        QHBoxLayout* hl = new QHBoxLayout();
        hl->setContentsMargins(10, 0, 10, 0);
        hl->addWidget(lblLogo);
        hl->addWidget(lblTitle);
        hl->addStretch();
        hl->addWidget(pushButtonClose_);

        widgetTitle_->setLayout(hl);
    }

    labelCurrentFile_ = new QLabel();
    labelCurrentFile_->setObjectName("compressProgressDialog_labelCurrentFile");

    progressBar_ = new QProgressBar();
    progressBar_->setObjectName("compressProgressDialog_progressBar");
    progressBar_->setFixedHeight(20);
    progressBar_->setAlignment(Qt::AlignCenter);
    progressBar_->setRange(0, 100);

    pushButtonOK_ = new QPushButton("确定");
    pushButtonOK_->setObjectName("compressProgressDialog_pushButtonOK");
    pushButtonOK_->setFixedSize(78, 38);
    pushButtonOK_->setVisible(false);
    pushButtonOK_->setCursor(QCursor(Qt::PointingHandCursor));

    pushButtonCancel_ = new QPushButton("取消");
    pushButtonCancel_->setObjectName("compressProgressDialog_pushButtonCancel");
    pushButtonCancel_->setFixedSize(78, 38);
    pushButtonCancel_->setCursor(QCursor(Qt::PointingHandCursor));

    QHBoxLayout* hlButton = new QHBoxLayout();
    hlButton->addStretch();
    hlButton->addWidget(pushButtonOK_);
    hlButton->addWidget(pushButtonCancel_);

    QVBoxLayout* vl = new QVBoxLayout();
    vl->setContentsMargins(10, 10, 10, 10);
    vl->addStretch();
    vl->addWidget(labelCurrentFile_);
    vl->addWidget(progressBar_);
    vl->addStretch();
    vl->addLayout(hlButton);

    QVBoxLayout* vlMain = new QVBoxLayout();
    vlMain->setContentsMargins(0, 0, 0, 0);
    vlMain->addWidget(widgetTitle_);
    vlMain->addLayout(vl);

    centralWidget_ = new QWidget();
    centralWidget_->setObjectName("compressProgressDialog_centralWidget");
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
