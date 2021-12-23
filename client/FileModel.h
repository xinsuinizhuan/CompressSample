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

enum ColIdentify {
    CI_DISPLAY_NAME = 0,
    CI_DISPLAY_FILESIZE = 1,
    CI_DISPLAY_FILETYPE = 2,
    CI_DISPLAY_MODIFYDATE = 3
};

enum FileBelongSys {
    FBS_FILESYSTEM = 0,
    FBS_COMPRESS_CONTENT_LIST = 1,
    FBS_SPECIAL = 2
};

enum FileType {
    FT_NORMAL_FILE = 0,
    FT_LINK = 1,
    FT_FOLDER = 2,
    FT_DOTDOT_FOLDER = 3,
    FT_LOGIC_DRIVER = 4
};

struct FileMeta {
    QString strName;
    QString strOwnFolder;
    QString strAbsolutePath;
    QString strDisplayFileType;
    qint64 iFileSize;
    QDateTime modifyTime;
    FileType ft;
    FileBelongSys fbs;

    FileMeta() {
        iFileSize = -1;
        ft = FileType::FT_NORMAL_FILE;
        fbs = FileBelongSys::FBS_FILESYSTEM;
    }
};

class FileModel : public QAbstractTableModel {
    Q_OBJECT
   public:
    FileModel(QObject* parent = Q_NULLPTR);
    virtual ~FileModel();

    void updateToDesktop();
    void updateFolder(const QString& folder, const QString& displayName);
    void updateFolder(const QList<FileMeta>& originFileMetas, const QString& displayName);

    void updateFilter(const QString& filter);

    QString currentFolder() const;
    QString currentFolderDisplayName() const;

    QList<FileMeta> shownFileMetas() const;
    QString getAbsolutePathByIndex(const QModelIndex& idx) const;
    FileMeta getFileMeta(const QModelIndex& idx) const;
    void reload();
    bool rename(const QModelIndex& idx, const QString& newName);

   protected:
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

   private:
    FileMeta createDotDotFileMeta(const QString& folder) const;
    QList<FileMeta> parseFolderItems(const QString& folder) const;
    QList<FileMeta> createLogicDiskItems(const QString& ownerFolder) const;
    void doUpdate();

   private:
    QString curFolderDisplayName_;
    QString curFolder_;
    QString curFilter_;

    QList<FileMeta> originFileMetas_;
    QList<FileMeta> shownFileMetas_;
};
