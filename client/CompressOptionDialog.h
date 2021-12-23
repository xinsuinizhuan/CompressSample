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

class CompressOptionDialog : public FramelessWindow<QDialog> {
    Q_OBJECT
   public:
    CompressOptionDialog(QWidget* parent = Q_NULLPTR);
    ~CompressOptionDialog();

    void setDefaultPath(const QString& s);
    QString selectedPath();

   protected:
    void setupUi();

   private:
    QWidget* centralWidget_ = Q_NULLPTR;
    QWidget* widgetTitle_ = Q_NULLPTR;
    QWidget* widgetFolder_ = Q_NULLPTR;
    QPushButton* pushButtonClose_ = Q_NULLPTR;
    QPushButton* pushButtonCompress_ = Q_NULLPTR;
    QLineEdit* lineEditPath_ = Q_NULLPTR;
    QPushButton* pushButtonBrowser_ = Q_NULLPTR;

    QRadioButton* radioButtonSpeed_ = Q_NULLPTR;
    QRadioButton* radioButtonSize_ = Q_NULLPTR;

    QButtonGroup* btnGroup_ = Q_NULLPTR;
};
