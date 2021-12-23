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
                                                        "��ѹĿ¼",
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
    setWindowTitle("��ѹ��");
    setFixedSize(480, 198);

    widgetTitle_ = new QWidget();
    widgetTitle_->setObjectName("extractOptionDialog_widgetTitle");
    widgetTitle_->setFixedHeight(28);
    {
        pushButtonClose_ = new QPushButton();
        pushButtonClose_->setObjectName("extractOptionDialog_pushButtonClose");
        pushButtonClose_->setFixedSize(18, 18);
        pushButtonClose_->setCursor(QCursor(Qt::PointingHandCursor));

        QLabel* lblTitle = new QLabel("��ѹ�ļ�");
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

    QLabel* lblTips = new QLabel("Ŀ��·������������ڽ���������:");
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

    pushButtonExtract_ = new QPushButton("������ѹ");
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
