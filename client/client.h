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
