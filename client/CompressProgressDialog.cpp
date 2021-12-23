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
        pMsgWnd->setBtn1Text("ȷ��");
        pMsgWnd->setBtn2Visible(false);
        pMsgWnd->setMsg(QString("ѹ��ʧ�ܣ������룺%1").arg(err));
        pMsgWnd->setTitle(PRODUCT_NAME_CH_A);
        pMsgWnd->open();
    });

    connect(diskCompressor_, &DiskCompressor::progress, this,
            [this](QString curEntry, int processedEntry, int totalEntry, qint64 processedBytes, qint64 totalBytes) {
                int progress = (double)processedBytes * 100.f / (double)totalBytes;
                setProgress(QDir::toNativeSeparators(curEntry), progress);
            });

    connect(diskCompressor_, &DiskCompressor::finished, this, [this]() {
        setProgress(QString("ѹ������%1").arg(QDir::toNativeSeparators(targetPath_)), 100);
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
    setWindowTitle("ѹ����");
    setFixedSize(480, 198);

    widgetTitle_ = new QWidget();
    widgetTitle_->setObjectName("compressProgressDialog_widgetTitle");
    widgetTitle_->setFixedHeight(28);
    {
        pushButtonClose_ = new QPushButton();
        pushButtonClose_->setObjectName("compressProgressDialog_pushButtonClose");
        pushButtonClose_->setFixedSize(18, 18);
        pushButtonClose_->setCursor(QCursor(Qt::PointingHandCursor));

        QLabel* lblTitle = new QLabel("ѹ���ļ�");
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

    pushButtonOK_ = new QPushButton("ȷ��");
    pushButtonOK_->setObjectName("compressProgressDialog_pushButtonOK");
    pushButtonOK_->setFixedSize(78, 38);
    pushButtonOK_->setVisible(false);
    pushButtonOK_->setCursor(QCursor(Qt::PointingHandCursor));

    pushButtonCancel_ = new QPushButton("ȡ��");
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
