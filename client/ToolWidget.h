/*
***************************************************************************************************
* 许可声明：
* 本软件（含源码、二进制）为开源软件，遵循MIT开源许可协议（若以下声明与MIT许可协议发送冲突，以本声明为准）。
* 任何个人或组织都可以不受限制的使用、修改该软件。
* 任何个人或组织都可以以源代码和二进制的形式重新分发、使用该软件。
* 任何个人或组织都可以不受限制地将该软件（或修改后的任意版本）用于商业、非商业的用途。
* 
* 免责声明：
* 作者不对使用、修改、分发（以及其他任何形式的使用）该软件（及其他修改后的任意版本）所产生的后果负有任何法律责任。
* 作者也不对该软件的安全性、稳定性做出任何保证。
* 
* 使用、修改、分发该软件时需要保留上述声明，且默认已经同意上述声明。
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
