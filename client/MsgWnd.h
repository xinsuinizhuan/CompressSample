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
#ifndef MSG_WND_H_
#define MSG_WND_H_
#pragma once
#include "FramelessWindow.hpp"

class MsgWnd : public FramelessWindow<QDialog> {
   public:
    enum ClickedBtn {
        CloseBtn = 0,
        Btn1 = 1,
        Btn2 = 2
    };
    MsgWnd(QWidget* parent = Q_NULLPTR);
    ~MsgWnd();

    void setTitle(const QString& s);
    void setMsg(const QString& s);
    void setBtn1Text(const QString& s);
    void setBtn2Text(const QString& s);
    void setBtn2Visible(bool visible);
    void setBtn2Prefer(bool prefer);

   private:
    void setupUi();

   private:
    QWidget* centralWidget_ = Q_NULLPTR;
    QPushButton* pushButtonClose_ = Q_NULLPTR;
    QPushButton* pushButton1_ = Q_NULLPTR;
    QPushButton* pushButton2_ = Q_NULLPTR;
    QLabel* labelMsg_ = Q_NULLPTR;
    QLabel* labelTitle_ = Q_NULLPTR;
};

#endif // !MSG_WND_H_
