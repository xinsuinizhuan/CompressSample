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
#include "FileUtil.h"
#ifdef WIN32
#include <ShlObj.h>
#endif
#include "Util.h"

qint64 FileUtil::GetFileSize(const QString& filePath) {
    qint64 fileSize = -1;
    std::wstring strFilePathW = filePath.toStdWString();
    HANDLE hFile = CreateFileW(strFilePathW.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        LARGE_INTEGER liFileSize;
        if (::GetFileSizeEx(hFile, &liFileSize)) {
            fileSize = liFileSize.QuadPart;
        }
        CloseHandle(hFile);
    }
    return fileSize;
}

QString FileUtil::FileSizeToHuman(qint64 num) {
    QString total;
    const qint64 kb = 1024;
    const qint64 mb = 1024 * kb;
    const qint64 gb = 1024 * mb;
    const qint64 tb = 1024 * gb;

    if (num >= tb)
        total = QString("%1 TB").arg(QString::number(qreal(num) / tb, 'f', 3));
    else if (num >= gb)
        total = QString("%1 GB").arg(QString::number(qreal(num) / gb, 'f', 2));
    else if (num >= mb)
        total = QString("%1 MB").arg(QString::number(qreal(num) / mb, 'f', 1));
    else if (num >= kb)
        total = QString("%1 KB").arg(QString::number(qreal(num) / kb, 'f', 1));
    else
        total = QString("%1 B").arg(num);

    return total;
}

bool FileUtil::IsSupportCompressFormat(const QString& fileName) {
    QFileInfo fi(fileName);
    QString suffixLower = fi.suffix().toLower();
    if (suffixLower == "zip" || suffixLower == "7z" || suffixLower == "rar")
        return true;
    return false;
}

bool FileUtil::MoveToTrash(const QString& path) {
    WCHAR szFolder[MAX_PATH] = {0};
    StringCchCopyW(szFolder, MAX_PATH, path.toStdWString().c_str());

    SHFILEOPSTRUCTW sfo = {0};
    sfo.wFunc = FO_DELETE;
    sfo.pFrom = szFolder;
    sfo.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_WANTNUKEWARNING;

    int res = SHFileOperation(&sfo);
    bool bFullSuccess = (res == 0 && !sfo.fAnyOperationsAborted);

    return bFullSuccess;
}

QString FileUtil::currentUserDesktopFolder() {
    wchar_t szDir[MAX_PATH] = {0};
    SHGetSpecialFolderPathW(NULL, szDir, CSIDL_DESKTOPDIRECTORY, 0);

    QString folder = QString::fromStdWString(szDir);
    return folder;
}

QString FileUtil::GetRecommandCompressPath(const QStringList& filePaths, const QString& suffix) {
    QString targetFolder;
    QString targetNameBase;
    if (filePaths.size() == 1) {
        QFileInfo fi(filePaths[0]);
        if (fi.isDir()) {
            QDir dir(filePaths[0]);
            dir.cdUp();
            targetFolder = dir.absolutePath();
            targetNameBase = fi.fileName();
        }
        else {
            targetFolder = fi.absoluteDir().absolutePath();
            targetNameBase = fi.fileName();
        }
    }
    else {
        targetFolder = FileUtil::currentUserDesktopFolder();
        targetNameBase = "新建压缩文件";
    }

    QString targetName = targetNameBase;
    int index = 1;
    do {
        if (!QFile::exists(QDir(targetFolder).absoluteFilePath(targetName + "." + suffix))) {
            break;
        }

        targetName = targetNameBase + QString(" (%1)").arg(index++);
    } while (true);

    return QDir::toNativeSeparators(QDir(targetFolder).absoluteFilePath(targetName + "." + suffix));
}

int FileUtil::StringIncludeTimes(const QString& src, const QString& search) {
    int times = 0;
    int pos = 0;
    int offset = 0;
    do {
        pos = src.indexOf(search, offset);
        if (pos == -1) {
            break;
        }
        times++;
        offset = pos + 1;
    } while (true);
    return times;
}

bool FileUtil::IsCompressLevel(const QString& path, int level) {
    bool result = false;
    if (path.isEmpty())
        return false;

    QString newPath = path;
    if (newPath.endsWith("\\") || newPath.endsWith("/"))
        newPath = newPath.mid(0, newPath.length() - 1);

    int totalTimes = StringIncludeTimes(newPath, "\\") + StringIncludeTimes(newPath, "/");
    return totalTimes == level;
}

bool FileUtil::IsInCompressFolder(const QString& folder, const QString& path) {
    if (folder.isEmpty()) {
        int pos = path.indexOf("/");
        return (pos == -1 || (pos == path.length() - 1));
    }

    if (folder == path)
        return false;

    int pos = path.indexOf(folder);
    if (pos != 0)
        return false;

    QString s = path.mid(folder.length());
    pos = s.indexOf("/");
    if (pos == -1 || pos == 0)
        return true;

    if (pos == s.length() - 1)
        return true;

    return false;
}

QString FileUtil::GetNameFromCompressPath(const QString& path) {
    int pos = path.lastIndexOf("/");
    if (pos == -1)
        return path;

    if (pos == path.length() - 1) {
        QString s = path.mid(0, path.length() - 1);
        pos = s.lastIndexOf("/");
        if (pos == -1) {
            return s;
        }
        else {
            return s.mid(pos + 1);
        }
    }
    else {
        return path.mid(pos + 1);
    }
}

QString FileUtil::GetCompressUpLevelPath(const QString& path) {
    if (path.isEmpty())
        return "";

    int pos = path.lastIndexOf("/");
    if (pos == -1)
        return "";

    if (pos == path.length() - 1) {
        QString s = path.mid(0, pos);
        pos = s.lastIndexOf("/");
        if (pos == -1)
            return "";
        else
            return s.mid(0, pos + 1);
    }
    else {
        return path.mid(0, pos + 1);
    }
}

QStringList FileUtil::GetListOfDrives() {
    QStringList driverList;
    DWORD uDriveMask = GetLogicalDrives();
    if (uDriveMask == 0)
        return driverList;

    WCHAR szDrive[] = L"A:\\";
    while (uDriveMask) {
        if (uDriveMask & 1) {
            driverList.append(QString::fromStdWString(szDrive));
        }
        ++szDrive[0];
        uDriveMask >>= 1;
    }

    return driverList;
}

QString FileUtil::GetVolumeName(const QString& driverRoot) {
    QString strName;
    WCHAR szVolName[MAX_PATH] = {0};
    if (GetVolumeInformationW(driverRoot.toStdWString().c_str(), szVolName, MAX_PATH, NULL, NULL, NULL, NULL, 0)) {
        strName = QString::fromStdWString(szVolName);
    }
    if (strName.isEmpty()) {
        switch (GetDriveTypeW(driverRoot.toStdWString().c_str())) {
            case DRIVE_FIXED: {
                strName = "本地磁盘";
                break;
            }
            case DRIVE_REMOVABLE: {
                strName = "可移动磁盘";
                break;
            }
            case DRIVE_NO_ROOT_DIR: {
                strName = "未知磁盘";
                break;
            }
            case DRIVE_REMOTE: {
                strName = "网络驱动器";
                break;
            }
            case DRIVE_CDROM: {
                strName = "光盘驱动器";
                break;
            }
            case DRIVE_RAMDISK: {
                strName = "RAM磁盘";
                break;
            }
            case DRIVE_UNKNOWN: {
                strName = "未知磁盘";
                break;
            }
        }
    }
    return strName;
}

bool FileUtil::RegisterFileAssociate() {
    wchar_t szExePath[MAX_PATH] = {0};
    GetModuleFileNameW(NULL, szExePath, MAX_PATH);
    std::wstring strExePath = szExePath;
    std::wstring strIconPath = L"\"" + strExePath + L"\"";
    std::wstring strOpenCMD = L"\"" + strExePath + L"\" -o=\"%1\"";

    {
        RegKey regKey1(HKEY_CLASSES_ROOT, L".zip");
        if (regKey1.Open(KEY_ALL_ACCESS, true) == S_OK) {
            regKey1.SetSZValue(NULL, PRODUCT_NAME_EN_W L".zip");

            RegKey regKey2(HKEY_CLASSES_ROOT, PRODUCT_NAME_EN_W L".zip");
            if (regKey2.Open(KEY_ALL_ACCESS, true) == S_OK) {
                regKey2.SetSZValue(NULL, L"Zip压缩文件");
            }

            RegKey regKey3(HKEY_CLASSES_ROOT, PRODUCT_NAME_EN_W L".zip\\DefaultIcon");
            if (regKey3.Open(KEY_ALL_ACCESS, true) == S_OK) {
                regKey3.SetSZValue(NULL, strIconPath);
            }

            RegKey regKey4(HKEY_CLASSES_ROOT, PRODUCT_NAME_EN_W L".zip\\Shell\\Open\\Command");
            if (regKey4.Open(KEY_ALL_ACCESS, true) == S_OK) {
                regKey4.SetSZValue(NULL, strOpenCMD);
            }
        }
    }

    {
        RegKey regKey1(HKEY_CLASSES_ROOT, L".rar");
        if (regKey1.Open(KEY_ALL_ACCESS, true) == S_OK) {
            regKey1.SetSZValue(NULL, PRODUCT_NAME_EN_W L".rar");

            RegKey regKey2(HKEY_CLASSES_ROOT, PRODUCT_NAME_EN_W L".rar");
            if (regKey2.Open(KEY_ALL_ACCESS, true) == S_OK) {
                regKey2.SetSZValue(NULL, L"Rar压缩文件");
            }

            RegKey regKey3(HKEY_CLASSES_ROOT, PRODUCT_NAME_EN_W L".rar\\DefaultIcon");
            if (regKey3.Open(KEY_ALL_ACCESS, true) == S_OK) {
                regKey3.SetSZValue(NULL, strIconPath);
            }

            RegKey regKey4(HKEY_CLASSES_ROOT, PRODUCT_NAME_EN_W L".rar\\Shell\\Open\\Command");
            if (regKey4.Open(KEY_ALL_ACCESS, true) == S_OK) {
                regKey4.SetSZValue(NULL, strOpenCMD);
            }
        }
    }

    {
        RegKey regKey1(HKEY_CLASSES_ROOT, L".7z");
        if (regKey1.Open(KEY_ALL_ACCESS, true) == S_OK) {
            regKey1.SetSZValue(NULL, PRODUCT_NAME_EN_W L".7z");

            RegKey regKey2(HKEY_CLASSES_ROOT, PRODUCT_NAME_EN_W L".7z");
            if (regKey2.Open(KEY_ALL_ACCESS, true) == S_OK) {
                regKey2.SetSZValue(NULL, L"7z压缩文件");
            }

            RegKey regKey3(HKEY_CLASSES_ROOT, PRODUCT_NAME_EN_W L".7z\\DefaultIcon");
            if (regKey3.Open(KEY_ALL_ACCESS, true) == S_OK) {
                regKey3.SetSZValue(NULL, strIconPath);
            }

            RegKey regKey4(HKEY_CLASSES_ROOT, PRODUCT_NAME_EN_W L".7z\\Shell\\Open\\Command");
            if (regKey4.Open(KEY_ALL_ACCESS, true) == S_OK) {
                regKey4.SetSZValue(NULL, strOpenCMD);
            }
        }
    }
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0);
    return true;
}

