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

#include <QtWidgets/QWidget>
#include "FramelessWindow.hpp"
#include "FileTable.h"
#include "FileModel.h"
#include "FileDelegate.h"
#include "OneLineEditer.h"
#include "QArchive"
#include "ToolWidget.h"

using namespace QArchive;

class client : public FramelessWindow<QWidget> {
    Q_OBJECT
   public:
    client(QWidget* parent = Q_NULLPTR);

    void relocateToFolder(const QString& strFolder, const QString& strFolderDisplayName);

   private:
    void createControls();
    void createActions();
    void setupUi();
    int getModelIndexListRows(const QModelIndexList& idxList);
    bool getFileTableFirstSelection(FileMeta& fm);
    void recursiveFileList(const QString& strFolder, QStringList& filePaths);
    void parseCommandline();
   private slots:
    void onTableViewDoubleItemClicked(const QModelIndex& idx);
    void onTableViewSelectionChanged(QModelIndexList indexList);
    void onOpenFirstSelectedItem();
    void onSelectAllItems();
    void onDeleteSelectedItems();
    void onRenameSelectedItem();
    void onCreateNewFolder();
    void onOpenFolderInExplorer();
    void onSearchItems();
    void onCopyItemToClipboard();
    void onPasteClipboardToCurFolder();
    void onOpenSelectedItemWithThis();
    void onPushButtonExtractToClicked();
    void onPushButtonOnKeyExtractClicked();
    void onOpenAZipClicked();

    void onCompressSelectedItems();
    void browserCompressFile(const QString& compressPath, const QString& ownerFolder);
    void compressToPath(const QStringList& paths, const QString& compressTarget, bool autoCloseAfterFinished, short compressFormat = QArchive::ZipFormat);
    void decompressToFolder(const QString& compressPath, const QString& folder, bool autoCloseAfterFinished);

   private:
    QWidget* centralWidget_ = Q_NULLPTR;
    QWidget* widgetTop_ = Q_NULLPTR;
    QLabel* labelLogo_ = Q_NULLPTR;
    QLabel* labelTitle_ = Q_NULLPTR;
    QPushButton* pushButtonMin_ = Q_NULLPTR;
    QPushButton* pushButtonMax_ = Q_NULLPTR;
    QPushButton* pushButtonClose_ = Q_NULLPTR;

    QToolButton* toolButtonFile_ = Q_NULLPTR;
    QToolButton* toolButtonOperate_ = Q_NULLPTR;
    QToolButton* toolButtonHelp_ = Q_NULLPTR;

    QToolButton* pushButtonExtractTo_ = Q_NULLPTR;
    QToolButton* pushButtonNew_ = Q_NULLPTR;
    QToolButton* pushButtonOneKeyExtractTo_ = Q_NULLPTR;
    QToolButton* pushButtonDel_ = Q_NULLPTR;
    QToolButton* pushButtonTool_ = Q_NULLPTR;

    QPushButton* pushButtonUp_ = Q_NULLPTR;
    QPushButton* pushButtonDesktop_ = Q_NULLPTR;
    QPushButton* pushButtonLayout_ = Q_NULLPTR;

    QPushButton* pushButtonSearch_ = Q_NULLPTR;
    OneLineEditer* editSearch_ = Q_NULLPTR;
    QWidget* widgetSearch_ = Q_NULLPTR;

    QLineEdit* lineEditPath_ = Q_NULLPTR;
    QPushButton* pushButtonDown_ = Q_NULLPTR;
    QWidget* widgetPath_ = Q_NULLPTR;

    FileTable* tableFileView_ = Q_NULLPTR;

    QWidget* widgetBottom_ = Q_NULLPTR;
    QLabel* labelDoing_ = Q_NULLPTR;

    QAction* actionOpenItem_ = Q_NULLPTR;
    QAction* actionSelectAllItem_ = Q_NULLPTR;
    QAction* actionDeleteItem_ = Q_NULLPTR;
    QAction* actionRenameItem_ = Q_NULLPTR;
    QAction* actionCreateNewFolder_ = Q_NULLPTR;
    QAction* actionCopyItem_ = Q_NULLPTR;
    QAction* actionPasteItem_ = Q_NULLPTR;
    QAction* actionOpenFolderInExplorer_ = Q_NULLPTR;
    QAction* actionOpenWithThis_ = Q_NULLPTR;

    QAction* actionOpenZip_ = Q_NULLPTR;
    QAction* actionExit_ = Q_NULLPTR;
    QAction* actionExtractTo_ = Q_NULLPTR;
    QAction* actionAddToZip_ = Q_NULLPTR;
    QAction* actionOpenHomepage_ = Q_NULLPTR;
    QAction* actionUpdate_ = Q_NULLPTR;
    QAction* actionAbout_ = Q_NULLPTR;

    QMenu* fileTableRightMenu_ = Q_NULLPTR;
    QMenu* fileMenu_ = Q_NULLPTR;
    QMenu* operateMenu_ = Q_NULLPTR;
    QMenu* helpMenu_ = Q_NULLPTR;

    ToolWidget* toolWidget_ = Q_NULLPTR;

   private:
    FileModel* fileModel_ = Q_NULLPTR;
    FileDelegate* fileDelegate_ = Q_NULLPTR;

    QJsonObject compressContent_;
    QString compressOwnerFolder_;
    QString compressName_;
};
