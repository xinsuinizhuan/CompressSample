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
#include "FileModel.h"
#include "Util.h"
#ifdef WIN32
#include <ShlObj.h>
#endif

namespace {
bool fileSizeLessThan(const FileMeta& left, const FileMeta& right) {
    if (left.strName == ".")
        return true;
    else if (right.strName == ".")
        return false;
    else if (left.strName == "..")
        return true;
    else if (right.strName == "..")
        return false;

    return (left.iFileSize < right.iFileSize);
}

bool fileSizeMoreThan(const FileMeta& left, const FileMeta& right) {
    if (left.strName == ".")
        return true;
    else if (right.strName == ".")
        return false;
    else if (left.strName == "..")
        return true;
    else if (right.strName == "..")
        return false;

    return (left.iFileSize > right.iFileSize);
}

bool fileNameLessThan(const FileMeta& left, const FileMeta& right) {
    if (left.strName == ".")
        return true;
    else if (right.strName == ".")
        return false;
    else if (left.strName == "..")
        return true;
    else if (right.strName == "..")
        return false;

    return (left.strName < right.strName);
}

bool fileNameMoreThan(const FileMeta& left, const FileMeta& right) {
    if (left.strName == ".")
        return true;
    else if (right.strName == ".")
        return false;
    else if (left.strName == "..")
        return true;
    else if (right.strName == "..")
        return false;

    return (left.strName > right.strName);
}

bool fileModifyTimeLessThan(const FileMeta& left, const FileMeta& right) {
    if (left.strName == ".")
        return true;
    else if (right.strName == ".")
        return false;
    else if (left.strName == "..")
        return true;
    else if (right.strName == "..")
        return false;

    return (left.modifyTime < right.modifyTime);
}

bool fileModifyTimeMoreThan(const FileMeta& left, const FileMeta& right) {
    if (left.strName == ".")
        return true;
    else if (right.strName == ".")
        return false;
    else if (left.strName == "..")
        return true;
    else if (right.strName == "..")
        return false;

    return (left.modifyTime > right.modifyTime);
}

bool fileTypeLessThan(const FileMeta& left, const FileMeta& right) {
    if (left.strName == ".")
        return true;
    else if (right.strName == ".")
        return false;
    else if (left.strName == "..")
        return true;
    else if (right.strName == "..")
        return false;

    return (left.strDisplayFileType < right.strDisplayFileType);
}

bool fileTypeMoreThan(const FileMeta& left, const FileMeta& right) {
    if (left.strName == ".")
        return true;
    else if (right.strName == ".")
        return false;
    else if (left.strName == "..")
        return true;
    else if (right.strName == "..")
        return false;

    return (left.strDisplayFileType > right.strDisplayFileType);
}
}  // namespace

FileModel::FileModel(QObject* parent /*= Q_NULLPTR*/) :
    QAbstractTableModel(parent) {
}

FileModel::~FileModel() {
}

void FileModel::updateToDesktop() {
    wchar_t szDir[MAX_PATH] = {0};
    SHGetSpecialFolderPathW(NULL, szDir, CSIDL_DESKTOPDIRECTORY, 0);

    wchar_t szPublicDir[MAX_PATH] = {0};
    SHGetSpecialFolderPathW(NULL, szPublicDir, CSIDL_COMMON_DESKTOPDIRECTORY, 0);

    QList<FileMeta> fileMetas;
  
    fileMetas.append(createDotDotFileMeta(QString::fromStdWString(szDir)));
    fileMetas.append(createLogicDiskItems(QString::fromStdWString(szDir)));
    fileMetas.append(parseFolderItems(QString::fromStdWString(szDir)));
    fileMetas.append(parseFolderItems(QString::fromStdWString(szPublicDir)));

    updateFolder(fileMetas, "����");
}

void FileModel::updateFolder(const QString& folder, const QString& displayName) {
    if (folder.isEmpty())
        return;

    curFolder_ = folder;
    curFolderDisplayName_ = displayName;
    originFileMetas_.clear();

    originFileMetas_.append(createDotDotFileMeta(folder));
    originFileMetas_.append(parseFolderItems(folder));

    doUpdate();
}

void FileModel::updateFolder(const QList<FileMeta>& originFileMetas, const QString& displayName) {
    originFileMetas_ = originFileMetas;
    curFolderDisplayName_ = displayName;

    doUpdate();
}

void FileModel::updateFilter(const QString& filter) {
    curFilter_ = filter;

    doUpdate();
}

QString FileModel::currentFolder() const {
    return curFolder_;
}

QString FileModel::currentFolderDisplayName() const {
    return curFolderDisplayName_;
}

QList<FileMeta> FileModel::shownFileMetas() const {
    return shownFileMetas_;
}

QString FileModel::getAbsolutePathByIndex(const QModelIndex& idx) const {
    QString path;
    if (idx.isValid()) {
        int row = idx.row();
        if (shownFileMetas_.size() > row) {
            path = shownFileMetas_[row].strAbsolutePath;
        }
    }
    return path;
}

FileMeta FileModel::getFileMeta(const QModelIndex& idx) const {
    FileMeta fm;
    if (idx.isValid()) {
        fm = shownFileMetas_[idx.row()];
    }
    return fm;
}

void FileModel::reload() {
    if (curFolderDisplayName_ == "����")
        updateToDesktop();
    else
        updateFolder(curFolder_, curFolderDisplayName_);
}

bool FileModel::rename(const QModelIndex& idx, const QString& newName) {
    if (shownFileMetas_.size() <= idx.row())
        return false;
    FileMeta fm = shownFileMetas_[idx.row()];
    QDir dir(fm.strOwnFolder);
    QString strOldObsPath = dir.absoluteFilePath(fm.strName);
    QString strNewObsPath = dir.absoluteFilePath(newName);

    bool ret = QFile::rename(strOldObsPath, strNewObsPath);
    if (ret) {
        reload();
    }
    return ret;
}

int FileModel::rowCount(const QModelIndex& parent) const {
    return shownFileMetas_.size();
}

int FileModel::columnCount(const QModelIndex& parent) const {
    return 4;
}

QVariant FileModel::data(const QModelIndex& index, int role) const {
    if (role == Qt::TextAlignmentRole) {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    else if (role == Qt::DisplayRole) {
        QString value;
        int row = index.row();
        if (row < shownFileMetas_.size()) {
            int col = index.column();
            if (col == CI_DISPLAY_NAME) {
                value = shownFileMetas_[row].strName;
            }
            else if (col == CI_DISPLAY_FILESIZE) {
                if (shownFileMetas_[row].iFileSize != -1)
                    value = FileUtil::FileSizeToHuman(shownFileMetas_[row].iFileSize);
            }
            else if (col == CI_DISPLAY_FILETYPE) {
                value = shownFileMetas_[row].strDisplayFileType;
            }
            else if (col == CI_DISPLAY_MODIFYDATE) {
                if (shownFileMetas_[row].modifyTime.isValid())
                    value = shownFileMetas_[row].modifyTime.toString("yyyy-MM-dd HH:mm:ss");
            }
        }
        return value;
    }
    else {
        return QVariant();
    }
}

QVariant FileModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == CI_DISPLAY_NAME) {
            return QString("����");
        }
        else if (section == CI_DISPLAY_FILESIZE) {
            return QString("��С");
        }
        else if (section == CI_DISPLAY_FILETYPE) {
            return QString("����");
        }
        else if (section == CI_DISPLAY_MODIFYDATE) {
            return QString("�޸�����");
        }
    }
    else if (orientation == Qt::Horizontal && role == Qt::TextAlignmentRole) {
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    return QVariant();
}

void FileModel::sort(int column, Qt::SortOrder order /*= Qt::AscendingOrder*/) {
    if (column == CI_DISPLAY_NAME) {
        if (order == Qt::AscendingOrder) {
            std::sort(shownFileMetas_.begin(), shownFileMetas_.end(), fileNameLessThan);
        }
        else {
            std::sort(shownFileMetas_.begin(), shownFileMetas_.end(), fileNameMoreThan);
        }
    }
    else if (column == CI_DISPLAY_FILESIZE) {
        if (order == Qt::AscendingOrder) {
            std::sort(shownFileMetas_.begin(), shownFileMetas_.end(), fileSizeLessThan);
        }
        else {
            std::sort(shownFileMetas_.begin(), shownFileMetas_.end(), fileSizeMoreThan);
        }
    }
    else if (column == CI_DISPLAY_FILETYPE) {
        if (order == Qt::AscendingOrder) {
            std::sort(shownFileMetas_.begin(), shownFileMetas_.end(), fileTypeLessThan);
        }
        else {
            std::sort(shownFileMetas_.begin(), shownFileMetas_.end(), fileTypeMoreThan);
        }
    }
    else if (column == CI_DISPLAY_MODIFYDATE) {
        if (order == Qt::AscendingOrder) {
            std::sort(shownFileMetas_.begin(), shownFileMetas_.end(), fileModifyTimeLessThan);
        }
        else {
            std::sort(shownFileMetas_.begin(), shownFileMetas_.end(), fileModifyTimeMoreThan);
        }
    }

    emit layoutChanged();
}

Qt::ItemFlags FileModel::flags(const QModelIndex& index) const {
    Qt::ItemFlags flags = QAbstractTableModel::flags(index);
    if (index.column() == 0) {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

FileMeta FileModel::createDotDotFileMeta(const QString& folder) const {
    QDir dir(folder);
    dir.cdUp();
    FileMeta dotmeta;
    dotmeta.strName = "..";
    dotmeta.ft = FileType::FT_DOTDOT_FOLDER;
    dotmeta.fbs = FileBelongSys::FBS_FILESYSTEM;
    dotmeta.strOwnFolder = folder;
    dotmeta.strAbsolutePath = QDir::toNativeSeparators(dir.absolutePath());
    dotmeta.strDisplayFileType = "�ļ���";

    return dotmeta;
}

QList<FileMeta> FileModel::parseFolderItems(const QString& folder) const {
    QList<FileMeta> fileMetas;
    QDir directory(folder);
    QStringList fileNames = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (QStringList::size_type i = 0; i != fileNames.size(); ++i) {
        QString fileName = fileNames.at(i);
        FileMeta meta;
        meta.strName = fileName;
        meta.strOwnFolder = folder;
        meta.fbs = FileBelongSys::FBS_FILESYSTEM;
        meta.strAbsolutePath = QDir::toNativeSeparators(directory.absoluteFilePath(fileName));

        QFileInfo fileInfo(meta.strAbsolutePath);
        meta.modifyTime = fileInfo.lastModified();
        if (fileInfo.isShortcut()) {
            meta.ft = FileType::FT_LINK;
            meta.strDisplayFileType = "��ݷ�ʽ";
            meta.iFileSize = FileUtil::GetFileSize(meta.strAbsolutePath);
        }
        else if (fileInfo.isDir()) {
            meta.ft = FileType::FT_FOLDER;
            meta.strDisplayFileType = "�ļ���";
        }
        else {  // file
            meta.ft = FileType::FT_NORMAL_FILE;
            meta.iFileSize = fileInfo.size();

            WCHAR szPath[MAX_PATH] = {0};
            StringCchCopyW(szPath, MAX_PATH, meta.strAbsolutePath.toStdWString().c_str());
            SHFILEINFOW sfi = {0};
            if (SHGetFileInfoW(szPath, 0, &sfi, sizeof(sfi), SHGFI_TYPENAME) != 0) {
                meta.strDisplayFileType = QString::fromStdWString(sfi.szTypeName);
            }
            else {
                meta.strDisplayFileType = "�ļ�";
            }
        }

        fileMetas.push_back(meta);
    }

    return fileMetas;
}

QList<FileMeta> FileModel::createLogicDiskItems(const QString& ownerFolder) const {
    QList<FileMeta> fms;
    QStringList driversList = FileUtil::GetListOfDrives();
    for (int i = 0; i < driversList.size(); i++) {
        FileMeta meta;
        meta.strName = QString("%1 (%2)").arg(FileUtil::GetVolumeName(driversList[i])).arg(driversList[i]);
        meta.ft = FileType::FT_LOGIC_DRIVER;
        meta.fbs = FileBelongSys::FBS_FILESYSTEM;
        meta.strOwnFolder = ownerFolder;
        meta.strAbsolutePath = driversList[i];
        meta.strDisplayFileType = "���ش���";

        fms.append(meta);
    }
    return fms;
}

void FileModel::doUpdate() {
    shownFileMetas_.clear();
    for (int i = 0; i < originFileMetas_.size(); i++) {
        if (curFilter_.isEmpty()) {
            shownFileMetas_.push_back(originFileMetas_[i]);
        }
        else {
            if (originFileMetas_[i].strName.indexOf(curFilter_) != -1) {
                shownFileMetas_.push_back(originFileMetas_[i]);
            }
        }
    }

    emit layoutChanged();
}
