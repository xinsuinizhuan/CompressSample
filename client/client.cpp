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
#include "stdafx.h"
#include "client.h"
#include <Shlwapi.h>
#include "Util.h"
#include "CmdlineParse.h"
#include "ExtractOptionDialog.h"
#include "CompressOptionDialog.h"
#include "ExtractProgressDialog.h"
#include "CompressProgressDialog.h"
#include "AboutDialog.h"

client::client(QWidget* parent) :
    FramelessWindow<QWidget>(true, parent) {
    setupUi();

    connect(pushButtonMin_, &QPushButton::clicked, this, [this]() {
        showMinimized();
    });

    connect(pushButtonMax_, &QPushButton::clicked, this, [this]() {
        if (isMaximized())
            showNormal();
        else
            showMaximized();
    });

    connect(pushButtonClose_, &QPushButton::clicked, this, [this]() {
        close();
    });

    connect(pushButtonNew_, &QPushButton::clicked, this, &client::onCompressSelectedItems);

    connect(pushButtonUp_, &QPushButton::clicked, this, [this]() {
        QString curFolder = fileModel_->currentFolder();
        if (!curFolder.isEmpty()) {
            QDir dir(curFolder);
            dir.cdUp();
            QString absPath = QDir::toNativeSeparators(dir.absolutePath());
            relocateToFolder(absPath, absPath);
        }
    });

    connect(pushButtonDesktop_, &QPushButton::clicked, this, [this]() {
        fileModel_->updateToDesktop();

        if (lineEditPath_)
            lineEditPath_->setText(QDir::toNativeSeparators(fileModel_->currentFolderDisplayName()));
    });

    connect(pushButtonSearch_, &QPushButton::clicked, this, &client::onSearchItems);
    connect(editSearch_, &OneLineEditer::editFinished, this, &client::onSearchItems);
    connect(pushButtonExtractTo_, &QPushButton::clicked, this, &client::onPushButtonExtractToClicked);
    connect(pushButtonOneKeyExtractTo_, &QPushButton::clicked, this, &client::onPushButtonOnKeyExtractClicked);
    connect(pushButtonDel_, &QPushButton::clicked, this, &client::onDeleteSelectedItems);
    connect(pushButtonTool_, &QPushButton::clicked, this, [this]() {
        if (!toolWidget_) {
            toolWidget_ = new ToolWidget(this);
        }

        if (toolWidget_->isVisible()) {
            toolWidget_->hide();
        }
        else {
            QRect rc = pushButtonTool_->geometry();
            toolWidget_->move(rc.left() + pushButtonTool_->width(), 2);
            toolWidget_->show();
        }
    });

    connect(tableFileView_, &FileTable::doubleClicked, this, &client::onTableViewDoubleItemClicked);
    connect(tableFileView_, &FileTable::fileSelectionChanged, this, &client::onTableViewSelectionChanged);
    connect(tableFileView_, &FileTable::customContextMenuRequested, this, [this](QPoint pos) {
        QModelIndex idx = tableFileView_->indexAt(pos);
        if (idx.isValid()) {
            QItemSelectionModel* selections = tableFileView_->selectionModel();
            QModelIndexList selected = selections->selectedIndexes();
            if (getModelIndexListRows(selected) == 1) {
                FileMeta fm = fileModel_->getFileMeta(selected[0]);
                actionOpenWithThis_->setEnabled(FileUtil::IsSupportCompressFormat(fm.strName));
                actionRenameItem_->setEnabled(true);
            }
            else {
                actionOpenWithThis_->setEnabled(false);
                actionRenameItem_->setEnabled(false);
            }

            fileTableRightMenu_->exec(QCursor::pos());
        }
    });

    connect(actionOpenItem_, &QAction::triggered, this, &client::onOpenFirstSelectedItem);
    connect(actionSelectAllItem_, &QAction::triggered, this, &client::onSelectAllItems);
    connect(actionDeleteItem_, &QAction::triggered, this, &client::onDeleteSelectedItems);
    connect(actionRenameItem_, &QAction::triggered, this, &client::onRenameSelectedItem);
    connect(actionCreateNewFolder_, &QAction::triggered, this, &client::onCreateNewFolder);
    connect(actionOpenFolderInExplorer_, &QAction::triggered, this, &client::onOpenFolderInExplorer);
    connect(actionCopyItem_, &QAction::triggered, this, &client::onCopyItemToClipboard);
    connect(actionPasteItem_, &QAction::triggered, this, &client::onPasteClipboardToCurFolder);
    connect(actionOpenWithThis_, &QAction::triggered, this, &client::onOpenSelectedItemWithThis);

    connect(actionExtractTo_, &QAction::triggered, this, &client::onPushButtonExtractToClicked);
    connect(actionAddToZip_, &QAction::triggered, this, &client::onCompressSelectedItems);
    connect(actionOpenZip_, &QAction::triggered, this, &client::onOpenAZipClicked);
    connect(actionExit_, &QAction::triggered, this, &client::close);
    connect(actionOpenHomepage_, &QAction::triggered, this, [this]() {
        QThread* t = QThread::create([]() {
            ::ShellExecuteW(NULL, L"open", L"https://www.baidu.com", L"", L"", SW_SHOWNORMAL);
        });
        t->start();
    });
    connect(actionUpdate_, &QAction::triggered, this, [this]() {
    });
    connect(actionAbout_, &QAction::triggered, this, [this]() {
        AboutDialog* dlg = new AboutDialog(this);
        connect(dlg, &QDialog::finished, this, [dlg]() {
            dlg->deleteLater();
        });
        dlg->open();
    });

    fileModel_ = new FileModel();
    fileDelegate_ = new FileDelegate();
    tableFileView_->setItemDelegate(fileDelegate_);
    tableFileView_->setModel(fileModel_);

    fileModel_->updateToDesktop();

    if (lineEditPath_)
        lineEditPath_->setText(QDir::toNativeSeparators(fileModel_->currentFolderDisplayName()));

    parseCommandline();
}

void client::relocateToFolder(const QString& strFolder, const QString& strFolderDisplayName) {
    if (fileModel_)
        fileModel_->updateFolder(strFolder, strFolderDisplayName);

    if (lineEditPath_)
        lineEditPath_->setText(QDir::toNativeSeparators(fileModel_->currentFolderDisplayName()));
}

void client::createControls() {
    centralWidget_ = new QWidget();
    centralWidget_->setObjectName("client_centralWidget");

    widgetTop_ = new QWidget();
    widgetTop_->setObjectName("client_widgetTop");

    labelLogo_ = new QLabel();
    labelLogo_->setObjectName("client_labelLogo");
    labelLogo_->setFixedSize(29, 24);

    labelTitle_ = new QLabel(PRODUCT_NAME_CH_A);
    labelTitle_->setObjectName("client_labelTitle");

    pushButtonMin_ = new QPushButton();
    pushButtonMin_->setObjectName("client_pushButtonMin");
    pushButtonMin_->setFixedSize(18, 18);
    pushButtonMin_->setCursor(QCursor(Qt::PointingHandCursor));

    pushButtonMax_ = new QPushButton();
    pushButtonMax_->setObjectName("client_pushButtonMax");
    pushButtonMax_->setFixedSize(18, 18);
    pushButtonMax_->setCursor(QCursor(Qt::PointingHandCursor));

    pushButtonClose_ = new QPushButton();
    pushButtonClose_->setObjectName("client_pushButtonClose");
    pushButtonClose_->setFixedSize(18, 18);
    pushButtonClose_->setCursor(QCursor(Qt::PointingHandCursor));

    toolButtonFile_ = new QToolButton();
    toolButtonFile_->setObjectName("client_toolButtonFile");
    toolButtonFile_->setText("文件");
    toolButtonFile_->setPopupMode(QToolButton::InstantPopup);
    toolButtonFile_->setCursor(QCursor(Qt::PointingHandCursor));

    toolButtonOperate_ = new QToolButton();
    toolButtonOperate_->setObjectName("client_toolButtonOperate");
    toolButtonOperate_->setText("操作");
    toolButtonOperate_->setPopupMode(QToolButton::InstantPopup);
    toolButtonOperate_->setCursor(QCursor(Qt::PointingHandCursor));

    toolButtonHelp_ = new QToolButton();
    toolButtonHelp_->setObjectName("client_toolButtonHelp");
    toolButtonHelp_->setText("帮助");
    toolButtonHelp_->setPopupMode(QToolButton::InstantPopup);
    toolButtonHelp_->setCursor(QCursor(Qt::PointingHandCursor));

    pushButtonNew_ = new QToolButton();
    pushButtonNew_->setObjectName("client_pushButtonNew");
    pushButtonNew_->setFixedSize(50, 46);
    pushButtonNew_->setCursor(QCursor(Qt::PointingHandCursor));
    pushButtonNew_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    pushButtonNew_->setText("新建");
    pushButtonNew_->setIcon(QIcon(":/images/images/new.png"));
    pushButtonNew_->setIconSize(QSize(30, 25));

    pushButtonExtractTo_ = new QToolButton();
    pushButtonExtractTo_->setObjectName("client_pushButtonExtractTo");
    pushButtonExtractTo_->setFixedSize(50, 46);
    pushButtonExtractTo_->setCursor(QCursor(Qt::PointingHandCursor));
    pushButtonExtractTo_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    pushButtonExtractTo_->setText("解压到");
    pushButtonExtractTo_->setIcon(QIcon(":/images/images/extract_to.png"));
    pushButtonExtractTo_->setIconSize(QSize(30, 25));

    pushButtonOneKeyExtractTo_ = new QToolButton();
    pushButtonOneKeyExtractTo_->setObjectName("client_pushButtonOneKeyExtractTo");
    pushButtonOneKeyExtractTo_->setFixedSize(50, 46);
    pushButtonOneKeyExtractTo_->setCursor(QCursor(Qt::PointingHandCursor));
    pushButtonOneKeyExtractTo_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    pushButtonOneKeyExtractTo_->setText("一键解压");
    pushButtonOneKeyExtractTo_->setIcon(QIcon(":/images/images/onekey_extract.png"));
    pushButtonOneKeyExtractTo_->setIconSize(QSize(30, 25));

    pushButtonDel_ = new QToolButton();
    pushButtonDel_->setObjectName("client_pushButtonDel");
    pushButtonDel_->setFixedSize(50, 46);
    pushButtonDel_->setCursor(QCursor(Qt::PointingHandCursor));
    pushButtonDel_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    pushButtonDel_->setText("删除");
    pushButtonDel_->setIcon(QIcon(":/images/images/del.png"));
    pushButtonDel_->setIconSize(QSize(30, 25));

    pushButtonTool_ = new QToolButton();
    pushButtonTool_->setObjectName("client_pushButtonTool");
    pushButtonTool_->setFixedSize(50, 46);
    pushButtonTool_->setCursor(QCursor(Qt::PointingHandCursor));
    pushButtonTool_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    pushButtonTool_->setText("工具");
    pushButtonTool_->setIcon(QIcon(":/images/images/tool.png"));
    pushButtonTool_->setIconSize(QSize(30, 25));

    pushButtonUp_ = new QPushButton();
    pushButtonUp_->setObjectName("client_pushButtonUp");
    pushButtonUp_->setFixedSize(18, 18);
    pushButtonUp_->setCursor(QCursor(Qt::PointingHandCursor));

    pushButtonDesktop_ = new QPushButton();
    pushButtonDesktop_->setObjectName("client_pushButtonDesktop");
    pushButtonDesktop_->setFixedSize(18, 18);
    pushButtonDesktop_->setCursor(QCursor(Qt::PointingHandCursor));

    pushButtonLayout_ = new QPushButton();
    pushButtonLayout_->setObjectName("client_pushButtonLayout");
    pushButtonLayout_->setFixedSize(18, 18);
    pushButtonLayout_->setCursor(QCursor(Qt::PointingHandCursor));

    widgetPath_ = new QWidget();
    widgetPath_->setObjectName("client_widgetPath");
    widgetPath_->setFixedHeight(26);
    widgetPath_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    lineEditPath_ = new QLineEdit();
    lineEditPath_->setObjectName("client_lineEditPath");

    pushButtonDown_ = new QPushButton();
    pushButtonDown_->setObjectName("client_pushButtonDown");
    pushButtonDown_->setFixedSize(12, 8);
    pushButtonDown_->setCursor(QCursor(Qt::PointingHandCursor));

    widgetSearch_ = new QWidget();
    widgetSearch_->setObjectName("client_widgetSearch");
    widgetSearch_->setFixedSize(220, 26);

    pushButtonSearch_ = new QPushButton();
    pushButtonSearch_->setObjectName("client_pushButtonSearch");
    pushButtonSearch_->setFixedSize(11, 12);
    pushButtonSearch_->setCursor(QCursor(Qt::PointingHandCursor));

    editSearch_ = new OneLineEditer();
    editSearch_->setObjectName("client_editSearch");

    tableFileView_ = new FileTable();
    tableFileView_->setObjectName("client_tableFile");
    tableFileView_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tableFileView_->setShowGrid(false);
    tableFileView_->setFocusPolicy(Qt::NoFocus);
    tableFileView_->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableFileView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableFileView_->setSortingEnabled(true);
    tableFileView_->setContextMenuPolicy(Qt::CustomContextMenu);
    tableFileView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableFileView_->horizontalHeader()->setSortIndicatorShown(true);
    tableFileView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableFileView_->verticalHeader()->hide();
    tableFileView_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    tableFileView_->verticalHeader()->setDefaultSectionSize(18);

    widgetBottom_ = new QWidget();
    widgetBottom_->setObjectName("client_widgetBottom");
    widgetBottom_->setFixedHeight(20);
    widgetBottom_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    labelDoing_ = new QLabel("您正在浏览文件夹");
    labelDoing_->setObjectName("client_labelDoing");
}

void client::createActions() {
    actionOpenItem_ = new QAction("打开");
    actionOpenItem_->setShortcut(Qt::Key_Return);

    actionOpenWithThis_ = new QAction("用" PRODUCT_NAME_CH_A "浏览");
    actionOpenWithThis_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return));

    actionSelectAllItem_ = new QAction("全选");
    actionSelectAllItem_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_A));

    actionDeleteItem_ = new QAction("删除");
    actionDeleteItem_->setShortcut(QKeySequence(Qt::Key_Delete));

    actionRenameItem_ = new QAction("重命名");
    actionRenameItem_->setShortcut(QKeySequence(Qt::Key_F2));

    actionCreateNewFolder_ = new QAction("新建文件夹");

    actionCopyItem_ = new QAction("复制");
    actionCopyItem_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));

    actionPasteItem_ = new QAction("粘贴");
    actionPasteItem_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_V));

    actionOpenFolderInExplorer_ = new QAction("打开所在文件夹");

    fileTableRightMenu_ = new QMenu();
    fileTableRightMenu_->addAction(actionOpenItem_);
    fileTableRightMenu_->addSeparator();
    fileTableRightMenu_->addAction(actionOpenWithThis_);
    fileTableRightMenu_->addAction(actionSelectAllItem_);
    fileTableRightMenu_->addAction(actionDeleteItem_);
    fileTableRightMenu_->addAction(actionRenameItem_);
    fileTableRightMenu_->addAction(actionCreateNewFolder_);
    fileTableRightMenu_->addAction(actionCopyItem_);
    fileTableRightMenu_->addAction(actionPasteItem_);
    fileTableRightMenu_->addAction(actionOpenFolderInExplorer_);

    // 菜单栏
    actionOpenZip_ = new QAction("打开一个压缩文件");
    actionOpenZip_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));

    actionExit_ = new QAction("退出(&X)");

    fileMenu_ = new QMenu();
    fileMenu_->addAction(actionOpenZip_);
    fileMenu_->addAction(actionExit_);

    actionExtractTo_ = new QAction("解压到");
    actionExtractTo_->setShortcut(QKeySequence(Qt::ALT | Qt::Key_E));

    actionAddToZip_ = new QAction("添加到压缩文件");
    actionAddToZip_->setShortcut(QKeySequence(Qt::ALT | Qt::Key_A));

    operateMenu_ = new QMenu();
    operateMenu_->addAction(actionExtractTo_);
    operateMenu_->addAction(actionAddToZip_);

    actionOpenHomepage_ = new QAction("官方网站(&G)");
    actionUpdate_ = new QAction("在线升级(&C)");
    actionAbout_ = new QAction("关于(&A)");

    helpMenu_ = new QMenu();
    helpMenu_->addAction(actionOpenHomepage_);
    helpMenu_->addAction(actionUpdate_);
    helpMenu_->addAction(actionAbout_);

    this->addAction(actionOpenItem_);
    this->addAction(actionSelectAllItem_);
    this->addAction(actionRenameItem_);
    this->addAction(actionDeleteItem_);
    this->addAction(actionCopyItem_);
    this->addAction(actionPasteItem_);
}

void client::setupUi() {
    setObjectName("client");
    setWindowTitle(PRODUCT_NAME_CH_A);

    createActions();
    createControls();

    toolButtonFile_->setMenu(fileMenu_);
    toolButtonOperate_->setMenu(operateMenu_);
    toolButtonHelp_->setMenu(helpMenu_);

    // 布局
    //
    QHBoxLayout* hlTitle = new QHBoxLayout();
    hlTitle->setSpacing(0);
    hlTitle->setContentsMargins(0, 0, 0, 0);
    hlTitle->addSpacing(5);
    hlTitle->addWidget(labelLogo_);
    hlTitle->addSpacing(3);
    hlTitle->addWidget(labelTitle_);
    hlTitle->addStretch();
    hlTitle->addWidget(toolButtonFile_);
    hlTitle->addSpacing(10);
    hlTitle->addWidget(toolButtonOperate_);
    hlTitle->addSpacing(10);
    hlTitle->addWidget(toolButtonHelp_);
    hlTitle->addSpacing(40);
    hlTitle->addWidget(pushButtonMin_);
    hlTitle->addSpacing(10);
    hlTitle->addWidget(pushButtonMax_);
    hlTitle->addSpacing(10);
    hlTitle->addWidget(pushButtonClose_);
    hlTitle->addSpacing(10);

    QHBoxLayout* hlTools = new QHBoxLayout();
    hlTools->setSpacing(32);
    hlTools->setContentsMargins(0, 0, 0, 0);
    hlTools->addSpacing(10);
    hlTools->addWidget(pushButtonNew_);
    hlTools->addWidget(pushButtonExtractTo_);
    hlTools->addWidget(pushButtonOneKeyExtractTo_);
    hlTools->addWidget(pushButtonDel_);
    hlTools->addWidget(pushButtonTool_);
    hlTools->addStretch();

    QHBoxLayout* hlSearch = new QHBoxLayout();
    hlSearch->addSpacing(15);
    hlSearch->setContentsMargins(0, 0, 0, 0);
    hlSearch->addWidget(pushButtonSearch_);
    hlSearch->addWidget(editSearch_);
    widgetSearch_->setLayout(hlSearch);

    QHBoxLayout* hlPath = new QHBoxLayout();
    hlPath->addSpacing(0);
    hlPath->setContentsMargins(4, 0, 15, 0);
    hlPath->addWidget(lineEditPath_);
    hlPath->addWidget(pushButtonDown_);
    widgetPath_->setLayout(hlPath);

    QHBoxLayout* hlLocationHelp = new QHBoxLayout();
    hlLocationHelp->setSpacing(15);
    hlLocationHelp->setContentsMargins(0, 0, 0, 0);
    hlLocationHelp->addSpacing(10);
    hlLocationHelp->addWidget(pushButtonUp_);
    hlLocationHelp->addWidget(pushButtonDesktop_);
    //hlLocationHelp->addWidget(pushButtonLayout_);
    hlLocationHelp->addWidget(widgetPath_);
    hlLocationHelp->addWidget(widgetSearch_);
    hlLocationHelp->addSpacing(10);

    QVBoxLayout* vlTop = new QVBoxLayout();
    vlTop->setSpacing(0);
    vlTop->setContentsMargins(0, 0, 0, 0);
    vlTop->addSpacing(6);
    vlTop->addLayout(hlTitle);
    vlTop->addSpacing(10);
    vlTop->addLayout(hlTools);
    vlTop->addSpacing(14);
    vlTop->addLayout(hlLocationHelp);
    vlTop->addSpacing(10);

    widgetTop_->setLayout(vlTop);

    QHBoxLayout* hlCenter = new QHBoxLayout();
    hlCenter->setSpacing(0);
    hlCenter->setContentsMargins(2, 0, 2, 0);
    hlCenter->addWidget(tableFileView_);

    QHBoxLayout* hlBottom = new QHBoxLayout();
    hlBottom->setSpacing(0);
    hlBottom->setContentsMargins(6, 0, 30, 0);
    hlBottom->addWidget(labelDoing_);
    hlBottom->addStretch();

    widgetBottom_->setLayout(hlBottom);

    QVBoxLayout* vlMain = new QVBoxLayout();
    vlMain->setSpacing(10);
    vlMain->setContentsMargins(0, 0, 0, 0);
    vlMain->addWidget(widgetTop_);
    vlMain->addLayout(hlCenter);
    vlMain->addWidget(widgetBottom_);

    centralWidget_->setLayout(vlMain);

    QVBoxLayout* vlAll = new QVBoxLayout();
    vlAll->setSpacing(0);
    vlAll->setContentsMargins(12, 12, 12, 12);
    vlAll->addWidget(centralWidget_);

    this->setLayout(vlAll);
    this->resize(814, 536);
    this->setResizeable(true);
    this->setTitlebar({widgetTop_});

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setOffset(0, 2);
    shadow->setColor(QColor("#AAAAAA"));
    shadow->setBlurRadius(12);
    centralWidget_->setGraphicsEffect(shadow);
}

int client::getModelIndexListRows(const QModelIndexList& idxList) {
    int rows = 0;
    for (int i = 0; i < idxList.size(); i++) {
        if (idxList[i].column() == 0)
            rows++;
    }
    return rows;
}

bool client::getFileTableFirstSelection(FileMeta& fm) {
    QItemSelectionModel* selections = tableFileView_->selectionModel();
    QModelIndexList selected = selections->selectedIndexes();
    if (selected.size() > 0) {
        fm = fileModel_->getFileMeta(selected[0]);
        return true;
    }
    return false;
}

void client::onTableViewDoubleItemClicked(const QModelIndex& idx) {
    FileMeta fm = fileModel_->getFileMeta(idx);
    if (fm.fbs == FBS_FILESYSTEM) {
        if (!fm.strAbsolutePath.isEmpty()) {
            if (fm.ft == FileType::FT_LINK) {
                akali::ShortcutProperties sp;
                if (akali::ResolveShortcut(fm.strAbsolutePath.toStdWString(), sp)) {
                    if (sp.target.length() > 0) {
                        QString strNewPath = QString::fromStdWString(sp.target);
                        QFileInfo targetFI(strNewPath);
                        if (targetFI.isDir()) {
                            fileModel_->updateFolder(strNewPath, strNewPath);
                            lineEditPath_->setText(QDir::toNativeSeparators(strNewPath));
                        }
                    }
                }
            }
            else if (fm.ft == FileType::FT_FOLDER || fm.ft == FileType::FT_DOTDOT_FOLDER) {
                fileModel_->updateFolder(fm.strAbsolutePath, fm.strAbsolutePath);
                lineEditPath_->setText(QDir::toNativeSeparators(fileModel_->currentFolderDisplayName()));
            }
            else if (fm.ft == FT_LOGIC_DRIVER) {
                fileModel_->updateFolder(fm.strAbsolutePath, fm.strAbsolutePath);
                lineEditPath_->setText(QDir::toNativeSeparators(fileModel_->currentFolderDisplayName()));
            }
        }
    }
    else if (fm.fbs == FBS_COMPRESS_CONTENT_LIST) {
        if (fm.ft == FT_FOLDER || fm.ft == FileType::FT_DOTDOT_FOLDER) {
            QList<FileMeta> fms;

            FileMeta dotmeta;
            dotmeta.strName = "..";
            dotmeta.ft = FileType::FT_DOTDOT_FOLDER;
            dotmeta.strDisplayFileType = "文件夹";
            dotmeta.strOwnFolder = fm.strAbsolutePath;
            if (fm.strAbsolutePath.isEmpty()) {
                dotmeta.fbs = FileBelongSys::FBS_FILESYSTEM;
                dotmeta.strAbsolutePath = compressOwnerFolder_;
            }
            else {
                dotmeta.fbs = FileBelongSys::FBS_COMPRESS_CONTENT_LIST;
                dotmeta.strAbsolutePath = FileUtil::GetCompressUpLevelPath(fm.strAbsolutePath);
            }
            fms.append(dotmeta);

            for (QJsonObject::iterator it = compressContent_.begin(); it != compressContent_.end(); it++) {
                FileMeta newFM;
                newFM.fbs = FileBelongSys::FBS_COMPRESS_CONTENT_LIST;

                QString key = it.key();
                QJsonObject val = it.value().toObject();

                if (FileUtil::IsInCompressFolder(fm.strAbsolutePath, key)) {
                    newFM.strName = FileUtil::GetNameFromCompressPath(key);
                    newFM.strAbsolutePath = key;

                    if (val["FileType"].toString() == "Directory") {
                        newFM.ft = FileType::FT_FOLDER;
                        newFM.strDisplayFileType = "文件夹";
                        newFM.modifyTime = QDateTime::fromSecsSinceEpoch(val["LastModifiedTimestamp"].toInt());
                        newFM.iFileSize = 0;
                    }
                    else {
                        newFM.ft = FileType::FT_NORMAL_FILE;
                        newFM.modifyTime = QDateTime::fromSecsSinceEpoch(val["LastModifiedTimestamp"].toInt());
                        newFM.iFileSize = val["SizeInByte"].toInt();
                    }

                    fms.append(newFM);
                }
            }

            fileModel_->updateFolder(fms, compressName_);
        }
    }
}

void client::onTableViewSelectionChanged(QModelIndexList indexList) {
    int folderCount = 0;
    int fileCount = 0;
    qint64 fileSize = 0L;

    for (int i = 0; i < indexList.size(); i++) {
        if (indexList[i].row() == 0)  // 0 is ..
            continue;

        if (indexList[i].column() != 0)
            continue;

        FileMeta fm = fileModel_->getFileMeta(indexList[i]);
        if (fm.ft == FileType::FT_FOLDER) {
            folderCount++;
        }
        else if (fm.ft == FileType::FT_LINK || fm.ft == FileType::FT_NORMAL_FILE) {
            fileCount++;
            fileSize += fm.iFileSize;
        }
    }

    QString strMsg = "您正在浏览文件夹 ";
    if (fileCount > 0 || folderCount > 0)
        strMsg += "已经选择 ";

    if (fileCount > 0)
        strMsg += QString("%1（%2个文件）").arg(FileUtil::FileSizeToHuman(fileSize)).arg(fileCount);

    if (folderCount > 0) {
        if (fileCount > 0)
            strMsg += "和 ";
        strMsg += QString("%1 个文件夹").arg(folderCount);
    }

    labelDoing_->setText(strMsg);
}

void client::onOpenFirstSelectedItem() {
    FileMeta fm;
    if (getFileTableFirstSelection(fm)) {
        if (fm.ft == FileType::FT_LINK) {
            akali::ShortcutProperties sp;
            if (akali::ResolveShortcut(fm.strAbsolutePath.toStdWString(), sp)) {
                std::wstring stTarget = sp.target;
                QThread* t = QThread::create([stTarget]() {
                    ::ShellExecuteW(NULL, L"open", stTarget.c_str(), L"", L"", SW_SHOWNORMAL);
                });
                t->start();
            }
        }
        else if (fm.ft == FileType::FT_FOLDER) {
            relocateToFolder(fm.strAbsolutePath, fm.strAbsolutePath);
        }
        else {
            QThread* t = QThread::create([fm]() {
                std::wstring strPathW = fm.strAbsolutePath.toStdWString();
                ::ShellExecuteW(NULL, L"open", strPathW.c_str(), L"", L"", SW_SHOWNORMAL);
            });
            t->start();
        }
    }
}

void client::onSelectAllItems() {
    if (tableFileView_) {
        tableFileView_->selectAll();
    }
}

void client::onDeleteSelectedItems() {
    QItemSelectionModel* selections = tableFileView_->selectionModel();
    QModelIndexList indexList = selections->selectedIndexes();

    for (int i = 0; i < indexList.size(); i++) {
        if (indexList[i].row() == 0)  // 0 is ..
            continue;

        if (indexList[i].column() != 0)
            continue;

        FileMeta fm = fileModel_->getFileMeta(indexList[i]);
        if (fm.ft == FileType::FT_FOLDER) {
            FileUtil::MoveToTrash(fm.strAbsolutePath);
            fileModel_->reload();
        }
        else if (fm.ft == FileType::FT_LINK || fm.ft == FileType::FT_NORMAL_FILE) {
            FileUtil::MoveToTrash(fm.strAbsolutePath);
            fileModel_->reload();
        }
    }
}

void client::onRenameSelectedItem() {
    QItemSelectionModel* selections = tableFileView_->selectionModel();
    QModelIndexList indexList = selections->selectedIndexes();
    tableFileView_->clearSelection();

    for (int i = 0; i < indexList.size(); i++) {
        if (indexList[i].row() == 0)  // 0 is ..
            continue;

        if (indexList[i].column() != CI_DISPLAY_NAME)
            continue;

        tableFileView_->edit(indexList[i]);
        break;  // 仅处理第一个
    }
}

void client::onCreateNewFolder() {
    QString folder = fileModel_->currentFolder();
    QDir dir(folder);
    if (dir.mkdir("新建文件夹")) {
        fileModel_->reload();
    }
}

void client::onOpenFolderInExplorer() {
    QItemSelectionModel* selections = tableFileView_->selectionModel();
    QModelIndexList indexList = selections->selectedIndexes();

    for (int i = 0; i < indexList.size(); i++) {
        if (indexList[i].row() == 0)  // 0 is ..
            continue;

        if (indexList[i].column() != 0)
            continue;

        FileMeta fm = fileModel_->getFileMeta(indexList[i]);
        std::wstring strFolder = fm.strOwnFolder.toStdWString();
        ShellExecuteW(NULL, L"open", strFolder.c_str(), NULL, NULL, SW_SHOWNORMAL);
        break;
    }
}

void client::onSearchItems() {
    QString strSearchTxt = editSearch_->text();
    if (strSearchTxt.isEmpty())
        lineEditPath_->setText(QDir::toNativeSeparators(fileModel_->currentFolderDisplayName()));
    else
        lineEditPath_->setText("搜索结果");

    fileModel_->updateFilter(strSearchTxt);
}

void client::onCopyItemToClipboard() {
}

void client::onPasteClipboardToCurFolder() {
}

void client::onOpenSelectedItemWithThis() {
    FileMeta fm;
    if (!getFileTableFirstSelection(fm))
        return;

    browserCompressFile(fm.strAbsolutePath, fm.strOwnFolder);
}

void client::onPushButtonExtractToClicked() {
    QItemSelectionModel* selections = tableFileView_->selectionModel();
    QModelIndexList selected = selections->selectedIndexes();
    if (selected.size() == 0)
        return;

    FileMeta fm = fileModel_->getFileMeta(selected[0]);
    QString compressTarget = fm.strAbsolutePath;
    if (!FileUtil::IsSupportCompressFormat(fm.strName))
        return;

    ExtractOptionDialog* optDlg = new ExtractOptionDialog();
    QString defFolder = fileModel_->currentFolder();
    if (defFolder.isEmpty()) {
        defFolder = FileUtil::currentUserDesktopFolder();
    }
    optDlg->setDefaultFolder(defFolder);
    connect(optDlg, &ExtractOptionDialog::finished, this, [optDlg, compressTarget, this](int result) {
        if (result == 0) {
            decompressToFolder(compressTarget, optDlg->selectedFolder(), true);
        }
        optDlg->deleteLater();
    });
    optDlg->open();
}

void client::onPushButtonOnKeyExtractClicked() {
    QItemSelectionModel* selections = tableFileView_->selectionModel();
    QModelIndexList selected = selections->selectedIndexes();
    if (selected.size() == 0)
        return;

    FileMeta fm = fileModel_->getFileMeta(selected[0]);
    QString compressTarget = fm.strAbsolutePath;
    if (!FileUtil::IsSupportCompressFormat(fm.strName))
        return;

    QString zipPath = fm.strAbsolutePath;
    QFileInfo fiZip(fm.strAbsolutePath);
    QString zipName = fiZip.fileName();
    int pos = zipName.indexOf(".");
    if (pos != -1)
        zipName = zipName.mid(0, pos);
    QString zipOwnerFolder = fm.strOwnFolder;

    Extractor* extractor = new Extractor(true);
    connect(extractor, &Extractor::info, this, [extractor, zipPath, zipName, zipOwnerFolder, this](QJsonObject root) {
        int firstCanDisplayLevel = -1;
        bool firstCanDisplayIsDir = false;
        int level = 0;
        do {
            for (QJsonObject::iterator it = root.begin(); it != root.end(); it++) {
                QString key = it.key();
                QJsonObject val = it.value().toObject();
                if (FileUtil::IsCompressLevel(key, level)) {
                    firstCanDisplayLevel = level;
                    firstCanDisplayIsDir = val["FileType"].toString() == "Directory";
                    if (!firstCanDisplayIsDir) {
                        firstCanDisplayIsDir = ((key.indexOf("/") != key.length() - 1) || (key.indexOf("\\") != key.length() - 1));
                    }
                    break;
                }
            }

            if (firstCanDisplayLevel != -1) {
                break;
            }

            level++;
        } while (true);

        QString targetFolder;
        if (!firstCanDisplayIsDir) {
            QString folderName = zipName;
            int index = 1;
            do {
                targetFolder = QDir(zipOwnerFolder).absoluteFilePath(folderName);
                QDir dir(targetFolder);
                if (!dir.exists())
                    break;

                folderName += QString(" (%1)").arg(index++);
            } while (true);
        }
        else {
            targetFolder = zipOwnerFolder;
        }

        decompressToFolder(zipPath, targetFolder, false);

        extractor->deleteLater();
    });
    extractor->setArchive(fm.strAbsolutePath);
    extractor->getInfo();
}

void client::onOpenAZipClicked() {
    QString path = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "压缩文件", "", "压缩文件 (*.zip *.7z *.rar);;Zip (*.zip);;7Zip (*.7z);;RAR (*.rar)"));
    if (path.isEmpty())
        return;

    QFileInfo fi(path);
    QString strOwnerFolder = fi.absoluteDir().absolutePath();
    browserCompressFile(path, strOwnerFolder);
}

void client::recursiveFileList(const QString& strFolder, QStringList& filePaths) {
    if (strFolder.isEmpty())
        return;

    QDir dir(strFolder);
    if (!dir.exists())
        return;

    dir.setFilter(QDir::AllEntries | QDir::Hidden);
    QFileInfoList list = dir.entryInfoList();
    int i = 0;
    do {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.fileName() == "." | fileInfo.fileName() == "..") {
            i++;
            continue;
        }

        if (fileInfo.isDir()) {
            recursiveFileList(fileInfo.filePath(), filePaths);
        }
        else {
            filePaths.append(QDir::toNativeSeparators(fileInfo.filePath()));
        }
        i++;
    } while (i < list.size());
}

void client::parseCommandline() {
    std::wstring strCMD;
    CmdLineParser clp(::GetCommandLineW());
    if (clp.HasKey(L"contextmenu")) {
        std::wstring stParamKey = clp.GetVal(L"contextmenu");
        if (stParamKey.length() > 0) {
            RegKey regKey(HKEY_CURRENT_USER, L"SOFTWARE\\" PRODUCT_NAME_EN_W);
            if (regKey.Open(KEY_WRITE | KEY_READ, true) == S_OK) {
                regKey.GetSZValue(stParamKey.c_str(), strCMD);
                RegKey::DeleteKey(HKEY_CURRENT_USER, L"SOFTWARE\\" PRODUCT_NAME_EN_W, stParamKey.c_str(), true);
                regKey.Close();
            }
        }
    }
    else if (clp.HasKey(L"o")) {
        QString strCompressPath = QString::fromStdWString(clp.GetVal(L"o"));
        if (!strCompressPath.isEmpty()) {
            QFileInfo fi(strCompressPath);
            QString strOwnerFolder = fi.absoluteDir().absolutePath();
            browserCompressFile(strCompressPath, strOwnerFolder);
        }
    }

    if (strCMD.length() > 0) {
        CmdLineParser clpCMD(strCMD);
        if (clpCMD.HasKey(L"e")) {
            QString strCompressFilePath = QString::fromStdWString(clpCMD.GetVal(L"file"));
            QString strTargetFolderPath = QString::fromStdWString(clpCMD.GetVal(L"target"));

            if (!strCompressFilePath.isEmpty()) {
                if (strTargetFolderPath.isEmpty()) {
                    ExtractOptionDialog* optDlg = new ExtractOptionDialog();
                    optDlg->setDefaultFolder(FileUtil::currentUserDesktopFolder());
                    connect(optDlg, &ExtractOptionDialog::finished, this, [optDlg, strCompressFilePath, this](int result) {
                        if (result == 0) {
                            decompressToFolder(strCompressFilePath, optDlg->selectedFolder(), true);
                        }
                        optDlg->deleteLater();
                    });
                    optDlg->open();
                }
                else {
                    decompressToFolder(strCompressFilePath, strTargetFolderPath, true);
                }
            }

            return;
        }
        else if (clpCMD.HasKey(L"ec")) {
            QString strCompressFilePath = QString::fromStdWString(clpCMD.GetVal(L"file"));
            QFileInfo fi(strCompressFilePath);
            QString strTargetFolderPath = fi.absoluteDir().absolutePath();

            if (!strCompressFilePath.isEmpty() && !strTargetFolderPath.isEmpty()) {
                decompressToFolder(strCompressFilePath, strTargetFolderPath, true);
            }
            return;
        }
        else if (clpCMD.HasKey(L"a")) {
            QString strFilePaths = QString::fromStdWString(clpCMD.GetVal(L"files"));
            QString strCompressFilePath = QString::fromStdWString(clpCMD.GetVal(L"target"));

            QStringList fileList = strFilePaths.split("|", Qt::SkipEmptyParts);
            if (fileList.size() > 0) {
                if (strCompressFilePath.isEmpty()) {
                    CompressOptionDialog* optDlg = new CompressOptionDialog();
                    QString targetPath = FileUtil::GetRecommandCompressPath(fileList, "zip");
                    optDlg->setDefaultPath(targetPath);
                    connect(optDlg, &CompressOptionDialog::finished, this, [optDlg, fileList, this](int result) {
                        if (result == 0) {
                            compressToPath(fileList, optDlg->selectedPath(), true);
                        }
                        optDlg->deleteLater();
                    });
                    optDlg->open();
                }
                else {
                    compressToPath(fileList, strCompressFilePath, true);
                }
            }
            return;
        }
        else if (clpCMD.HasKey(L"o")) {
            QString strCompressPath = QString::fromStdWString(clpCMD.GetVal(L"o"));
            if (!strCompressPath.isEmpty()) {
                QFileInfo fi(strCompressPath);
                QString strOwnerFolder = fi.absoluteDir().absolutePath();
                browserCompressFile(strCompressPath, strOwnerFolder);
            }
        }
    }
    else {
        show();
        FileUtil::RegisterFileAssociate();
    }
}

void client::onCompressSelectedItems() {
    QStringList filePaths;
    QItemSelectionModel* selections = tableFileView_->selectionModel();
    QModelIndexList indexList = selections->selectedIndexes();
    for (int i = 0; i < indexList.size(); i++) {
        if (indexList[i].row() == 0)  // 0 is ..
            continue;

        if (indexList[i].column() != 0)  // 去重
            continue;

        filePaths.append(fileModel_->getFileMeta(indexList[i]).strAbsolutePath);
    }

    if (filePaths.size() == 0)
        return;

    CompressOptionDialog* optDlg = new CompressOptionDialog();
    QString targetPath = FileUtil::GetRecommandCompressPath(filePaths, "zip");
    optDlg->setDefaultPath(targetPath);
    connect(optDlg, &CompressOptionDialog::finished, this, [optDlg, filePaths, this](int result) {
        if (result == 0) {
            compressToPath(filePaths, optDlg->selectedPath(), true);
        }
        optDlg->deleteLater();
    });
    optDlg->open();
}

void client::browserCompressFile(const QString& compressPath, const QString& ownerFolder) {
    QFileInfo fiZip(compressPath);
    compressName_ = fiZip.fileName();
    compressOwnerFolder_ = ownerFolder;

    Extractor* extractor = new Extractor(true);
    connect(extractor, &Extractor::info, this, [extractor, this](QJsonObject root) {
        compressContent_ = root;

        QList<FileMeta> fms;

        FileMeta dotmeta;
        dotmeta.strName = "..";
        dotmeta.ft = FileType::FT_DOTDOT_FOLDER;
        dotmeta.fbs = FileBelongSys::FBS_FILESYSTEM;
        dotmeta.strOwnFolder = "";
        dotmeta.strAbsolutePath = compressOwnerFolder_;
        dotmeta.strDisplayFileType = "文件夹";

        fms.append(dotmeta);

        int firstCanDisplayLevel = -1;
        int level = 0;
        do {
            for (QJsonObject::iterator it = root.begin(); it != root.end(); it++) {
                QString key = it.key();
                if (FileUtil::IsCompressLevel(key, level)) {
                    firstCanDisplayLevel = level;
                    break;
                }
            }

            if (firstCanDisplayLevel != -1) {
                break;
            }

            level++;
        } while (true);

        for (QJsonObject::iterator it = root.begin(); it != root.end(); it++) {
            FileMeta newFM;
            newFM.fbs = FileBelongSys::FBS_COMPRESS_CONTENT_LIST;

            QString key = it.key();
            QJsonObject val = it.value().toObject();

            if (FileUtil::IsCompressLevel(key, firstCanDisplayLevel)) {
                newFM.strName = FileUtil::GetNameFromCompressPath(key);
                newFM.strAbsolutePath = key;

                if (val["FileType"].toString() == "Directory") {
                    newFM.ft = FileType::FT_FOLDER;
                    newFM.strDisplayFileType = "文件夹";
                    newFM.modifyTime = QDateTime::fromSecsSinceEpoch(val["LastModifiedTimestamp"].toInt());
                    newFM.iFileSize = 0;
                }
                else {
                    newFM.strName = FileUtil::GetNameFromCompressPath(key);
                    newFM.ft = FileType::FT_NORMAL_FILE;
                    newFM.modifyTime = QDateTime::fromSecsSinceEpoch(val["LastModifiedTimestamp"].toInt());
                    newFM.iFileSize = val["SizeInByte"].toInt();
                }

                fms.append(newFM);
            }
        }

        fileModel_->updateFolder(fms, compressName_);
        lineEditPath_->setText(QDir::toNativeSeparators(compressName_));

        extractor->deleteLater();
    });
    extractor->setArchive(compressPath);
    extractor->getInfo();
}

void client::compressToPath(const QStringList& paths, const QString& compressTarget, bool autoCloseAfterFinished, short compressFormat) {
    CompressProgressDialog* compressDlg = new CompressProgressDialog();
    compressDlg->setAutoCloseAfterFinished(autoCloseAfterFinished);
    connect(compressDlg, &CompressProgressDialog::finished, this, [compressDlg, this](int result) {
        compressDlg->deleteLater();

        if (fileModel_)
            fileModel_->reload();
    });
    compressDlg->compress(paths, compressTarget, compressFormat);
    compressDlg->open();
}

void client::decompressToFolder(const QString& compressPath, const QString& folder, bool autoCloseAfterFinished) {
    ExtractProgressDialog* extractDlg = new ExtractProgressDialog();
    extractDlg->setAutoCloseAfterFinished(autoCloseAfterFinished);
    connect(extractDlg, &ExtractProgressDialog::finished, this, [extractDlg, this](int result) {
        extractDlg->deleteLater();

        if (fileModel_)
            fileModel_->reload();
    });
    extractDlg->extract(compressPath, folder);
    extractDlg->open();
}
