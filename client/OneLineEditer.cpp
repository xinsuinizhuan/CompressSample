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
#include "OneLineEditer.h"

OneLineEditer::OneLineEditer(QWidget* parent /*= Q_NULLPTR*/) :
    QLineEdit(parent) {
    this->setFocusPolicy(Qt::StrongFocus);
    this->installEventFilter(this);
}

bool OneLineEditer::eventFilter(QObject* obj, QEvent* e) {
    if (obj == this && e->type() == QEvent::KeyRelease) {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(e);
        if (keyEvent) {
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
                emit editFinished();
                e->ignore();
                return true;
            }
        }
    }
    return QLineEdit::eventFilter(obj, e);
}