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
