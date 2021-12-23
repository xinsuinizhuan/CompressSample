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
#pragma once
#include "FramelessWindow.hpp"
#include "QArchive"

using namespace QArchive;

class ExtractProgressDialog : public FramelessWindow<QDialog> {
    Q_OBJECT
   public:
    ExtractProgressDialog(QWidget* parent = Q_NULLPTR);
    ~ExtractProgressDialog();

    void setAutoCloseAfterFinished(bool b) {
        autoCloseAfterFinished_ = b;
    }
   public slots:
    void extract(const QString& compressFile, const QString& folder);
    void setProgress(const QString& curFile, int progress);

   protected:
    void setupUi();

   private:
    QWidget* centralWidget_ = Q_NULLPTR;
    QWidget* widgetTitle_ = Q_NULLPTR;
    QLabel* labelCurrentFile_ = Q_NULLPTR;
    QPushButton* pushButtonClose_ = Q_NULLPTR;
    QPushButton* pushButtonOK_ = Q_NULLPTR;
    QPushButton* pushButtonCancel_ = Q_NULLPTR;
    QProgressBar* progressBar_ = Q_NULLPTR;

   private:
    DiskExtractor* diskExtractor_ = Q_NULLPTR;
    bool autoCloseAfterFinished_ = true;
    QString targetFolder_;
};
