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
#include <QtWidgets>
#include "FramelessWindow.hpp"

class UpdateProgressWnd : public FramelessWindow<QDialog> {
    Q_OBJECT
   public:
    UpdateProgressWnd(QWidget* parent = Q_NULLPTR);
    ~UpdateProgressWnd();

   public slots:
    void setFinished();

   private:
    void setupUi();

   private:
    QLabel* lblMsg_ = Q_NULLPTR;
    QPushButton* btnClose_ = Q_NULLPTR;
    QPushButton* btnMin_ = Q_NULLPTR;
    QPushButton* btnFinish_ = Q_NULLPTR;

    QWidget* widgetTop_ = Q_NULLPTR;
    QWidget* widgetBottom_ = Q_NULLPTR;
    QWidget* centalWidget_ = Q_NULLPTR;
};
