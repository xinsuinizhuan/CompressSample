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

    btnFinish_ = new QPushButton("���");
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

        lblMsg_ = new QLabel(PRODUCT_NAME_CH_A "������...");
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
    btnFinish_->setText("���");
    btnFinish_->setEnabled(true);
    btnClose_->setVisible(true);
    lblMsg_->setText("��~" PRODUCT_NAME_CH_A "���������ɣ�");
}
