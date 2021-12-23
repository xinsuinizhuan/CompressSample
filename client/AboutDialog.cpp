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

        QLabel* lblTitle = new QLabel("ѹ���ļ�-" PRODUCT_NAME_CH_A);
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
            QLabel* lblMsg = new QLabel(PRODUCT_NAME_CH_A "������������һ����ѹ������");
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

    pushButtonOK_ = new QPushButton("ȷ��");
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
