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

class ExtractOptionDialog : public FramelessWindow<QDialog> {
    Q_OBJECT
   public:
    ExtractOptionDialog(QWidget* parent = Q_NULLPTR);
    ~ExtractOptionDialog();

    void setDefaultFolder(const QString& s);
    QString selectedFolder();

   protected:
    void setupUi();

   private:
    QWidget* centralWidget_ = Q_NULLPTR;
    QWidget* widgetTitle_ = Q_NULLPTR;
    QWidget* widgetFolder_ = Q_NULLPTR;
    QPushButton* pushButtonClose_ = Q_NULLPTR;
    QPushButton* pushButtonExtract_ = Q_NULLPTR;
    QLineEdit* lineEditFolder_ = Q_NULLPTR;
    QPushButton* pushButtonBrowser_ = Q_NULLPTR;
};
