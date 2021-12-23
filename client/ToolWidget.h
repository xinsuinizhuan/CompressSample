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
#include "HttpManager.h"
#include "teemo/teemo.h"

class ToolItem : public QWidget {
    Q_OBJECT
   public:
    ToolItem(const TaskMeta& tm, QWidget* parent = Q_NULLPTR);
    ~ToolItem();

   signals:
    void itemClicked(TaskMeta tm);

   private:
    void setupUi();

   private:
    QPushButton* pushButton_ = Q_NULLPTR;
    QLabel* labelName_ = Q_NULLPTR;

   private:
    TaskMeta toolMeta_;
};

class ToolWidget : public FramelessWindow<QWidget> {
    Q_OBJECT
   public:
    ToolWidget(QWidget* parent = Q_NULLPTR);
    ~ToolWidget();

   protected:
    void setupUi();
    void leaveEvent(QEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
   private slots:
    void onItemClicked(TaskMeta tm);
    void updateList();
   private:
    void querySoftList();
   private:
    QWidget* centralWidget_ = Q_NULLPTR;
    QTableWidget* toolTable_ = Q_NULLPTR;

   private:
    std::vector<TaskMeta> toolMetas_;
    std::vector<std::shared_ptr<teemo::Teemo>> teemos_;
};
