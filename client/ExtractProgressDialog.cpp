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
#include "ExtractProgressDialog.h"
#include "MsgWnd.h"

ExtractProgressDialog::ExtractProgressDialog(QWidget* parent /*= Q_NULLPTR*/) :
    FramelessWindow<QDialog>(true, parent) {
    setupUi();

    connect(pushButtonClose_, &QPushButton::clicked, this, [this]() {
        diskExtractor_->cancel();
        done(1);
    });

    connect(pushButtonCancel_, &QPushButton::clicked, this, [this]() {
        diskExtractor_->cancel();
        done(1);
    });

    connect(pushButtonOK_, &QPushButton::clicked, this, [this]() {
        done(0);
    });

    diskExtractor_ = new DiskExtractor();
    connect(diskExtractor_, &DiskExtractor::error, this, [this](short err) {
        pushButtonOK_->setVisible(true);
        pushButtonCancel_->setVisible(false);

        MsgWnd* pMsgWnd = new MsgWnd(this);
        connect(pMsgWnd, &MsgWnd::finished, this, [pMsgWnd]() { pMsgWnd->deleteLater(); });
        pMsgWnd->setBtn1Text("ȷ��");
        pMsgWnd->setBtn2Visible(false);
        pMsgWnd->setMsg(QString("��ѹʧ�ܣ������룺%1").arg(err));
        pMsgWnd->setTitle(PRODUCT_NAME_CH_A);
        pMsgWnd->open();
    });

    connect(diskExtractor_, &DiskExtractor::progress, this,
            [this](QString curEntry, int processedEntry, int totalEntry, qint64 processedBytes, qint64 totalBytes) {
                int progress = (double)processedBytes * 100.f / (double)totalBytes;
                setProgress(QDir::toNativeSeparators(curEntry), progress);
            });

    connect(diskExtractor_, &DiskExtractor::finished, this, [this]() {
        setProgress(QString("��ѹ����%1").arg(QDir::toNativeSeparators(targetFolder_)), 100);
        pushButtonOK_->setVisible(true);
        pushButtonCancel_->setVisible(false);

        if (autoCloseAfterFinished_)
            done(0);
    });
}

ExtractProgressDialog::~ExtractProgressDialog() {
}

void ExtractProgressDialog::extract(const QString& compressFile, const QString& folder) {
    QDir dir;
    if (!dir.mkpath(folder)) {
        pushButtonOK_->setVisible(true);
        pushButtonCancel_->setVisible(false);

        MsgWnd* pMsgWnd = new MsgWnd(this);
        connect(pMsgWnd, &MsgWnd::finished, this, [pMsgWnd]() { pMsgWnd->deleteLater(); });
        pMsgWnd->setBtn1Text("ȷ��");
        pMsgWnd->setBtn2Visible(false);
        pMsgWnd->setMsg(QString("����Ŀ¼%1ʧ��").arg(folder));
        pMsgWnd->setTitle(PRODUCT_NAME_CH_A);
        pMsgWnd->open();
        return;
    }

    targetFolder_ = folder;
    diskExtractor_->setArchive(compressFile);
    diskExtractor_->setOutputDirectory(folder);
    diskExtractor_->setCalculateProgress(true);
    diskExtractor_->start();
}

void ExtractProgressDialog::setProgress(const QString& curFile, int progress) {
    if (labelCurrentFile_)
        labelCurrentFile_->setText(curFile);

    progressBar_->setValue(progress);
}

void ExtractProgressDialog::setupUi() {
    setObjectName("extractProgressDialog");
    setWindowTitle("��ѹ��");
    setFixedSize(480, 198);

    widgetTitle_ = new QWidget();
    widgetTitle_->setObjectName("extractProgressDialog_widgetTitle");
    widgetTitle_->setFixedHeight(28);
    {
        pushButtonClose_ = new QPushButton();
        pushButtonClose_->setObjectName("extractProgressDialog_pushButtonClose");
        pushButtonClose_->setFixedSize(18, 18);

        QLabel* lblTitle = new QLabel("��ѹ�ļ�");
        lblTitle->setObjectName("extractProgressDialog_lblTitle");

        QLabel* lblLogo = new QLabel();
        lblLogo->setObjectName("extractProgressDialog_lblLogo");
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
    labelCurrentFile_->setObjectName("extractProgressDialog_labelCurrentFile");

    progressBar_ = new QProgressBar();
    progressBar_->setObjectName("extractProgressDialog_progressBar");
    progressBar_->setFixedHeight(20);
    progressBar_->setAlignment(Qt::AlignCenter);
    progressBar_->setRange(0, 100);

    pushButtonOK_ = new QPushButton("ȷ��");
    pushButtonOK_->setObjectName("extractProgressDialog_pushButtonOK");
    pushButtonOK_->setFixedSize(78, 38);
    pushButtonOK_->setVisible(false);

    pushButtonCancel_ = new QPushButton("ȡ��");
    pushButtonCancel_->setObjectName("extractProgressDialog_pushButtonCancel");
    pushButtonCancel_->setFixedSize(78, 38);

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
    centralWidget_->setObjectName("extractProgressDialog_centralWidget");
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
