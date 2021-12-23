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
                                                    "ѹ���ļ�",
                                                    folder,
                                                    "ѹ���ļ� (*.zip *.7z *.rar);;Zip (*.zip);;7Zip (*.7z);;RAR (*.rar)");
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
    setWindowTitle("ѹ����");
    setFixedSize(480, 198);

    widgetTitle_ = new QWidget();
    widgetTitle_->setObjectName("compressOptionDialog_widgetTitle");
    widgetTitle_->setFixedHeight(28);
    {
        pushButtonClose_ = new QPushButton();
        pushButtonClose_->setObjectName("compressOptionDialog_pushButtonClose");
        pushButtonClose_->setFixedSize(18, 18);
        pushButtonClose_->setCursor(QCursor(Qt::PointingHandCursor));

        QLabel* lblTitle = new QLabel("ѹ���ļ�");
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
        radioButtonSpeed_->setText("�ٶ����");
        radioButtonSpeed_->setChecked(true);
        radioButtonSpeed_->setCursor(QCursor(Qt::PointingHandCursor));

        radioButtonSize_ = new QRadioButton();
        radioButtonSize_->setObjectName("compressOptionDialog_radioButtonSize");
        radioButtonSize_->setText("�����С");
        radioButtonSize_->setCursor(QCursor(Qt::PointingHandCursor));

        btnGroup_ = new QButtonGroup();
        btnGroup_->addButton(radioButtonSpeed_, 0);
        btnGroup_->addButton(radioButtonSize_, 1);

        QLabel* lbl = new QLabel("ѹ������: ");
        lbl->setObjectName("compressOptionDialog_lblConfig");

        hlFormat->addWidget(lbl);
        hlFormat->addWidget(radioButtonSpeed_);
        hlFormat->addWidget(radioButtonSize_);
        hlFormat->addStretch();
    }

    pushButtonCompress_ = new QPushButton("����ѹ��");
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
