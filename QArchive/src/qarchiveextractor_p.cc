#include <QCoreApplication>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>

#include "qarchiveextractor_p.hpp"
#include "qarchive_enums.hpp"

extern "C" {
#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
}

#if defined(__APPLE__)
#define st_atim st_atimespec.tv_sec
#define st_ctim st_ctimespec.tv_sec
#define st_mtim st_mtimespec.tv_sec
#elif defined(_WIN32) && !defined(__CYGWIN__)
#define st_atim st_atime
#define st_ctim st_ctime
#define st_mtim st_mtime
#else
#define st_atim st_atim.tv_sec
#define st_ctim st_ctim.tv_sec
#define st_mtim st_mtim.tv_sec
#endif

// Helpful macros to check if an archive error is caused due to
// faulty passwords.
// Expects a pointer to a struct archive , returns 1 if password
// is needed or incorrect.
#define PASSWORD_NEEDED(a) !qstrcmp(archive_error_string(a) ,"Passphrase required for this entry")
#define PASSWORD_INCORRECT(a) !qstrcmp(archive_error_string(a) , "Incorrect passphrase")

using namespace QArchive;

/// MutableMemoryFile class provides a memory files which has both setters and getters 
/// unlike MemoryFile which only has getters.
/// This will force the users to not mess up the integrity of a MemoryFile like 
/// deleting the internal pointers which will be automatically freed by MemoryFile 
/// destructor.
MutableMemoryFile::MutableMemoryFile() { }
MutableMemoryFile::~MutableMemoryFile() {
    m_Buffer.clear();
}


void MutableMemoryFile::setFileInformation(const QJsonObject &info) {
    m_FileInformation = info;
}

void MutableMemoryFile::setBuffer(QBuffer *buffer) {
    m_Buffer.reset(buffer);
}

QJsonObject MutableMemoryFile::getFileInformation() {
    return m_FileInformation;
}

QSharedPointer<QBuffer> MutableMemoryFile::getBuffer() {
    return m_Buffer;
}
/// ---


static QJsonObject getArchiveEntryInformation(archive_entry *entry) {
    QJsonObject CurrentEntry;
    QString CurrentFile = QString::fromStdWString(archive_entry_pathname_w(entry));

    auto entry_stat = archive_entry_stat(entry);
    qint64 size = (qint64)entry_stat->st_size;
    QString sizeUnits = "Bytes";
    if(size == 0) {
        sizeUnits = "None";
        size = 0;
    } else if(size < 1024) {
        sizeUnits = "Bytes";
        size = size;
    } else if(size >= 1024 && size < 1048576) {
        sizeUnits = "KiB";
        size /= 1024;
    } else if(size >= 1048576 && size < 1073741824) {
        sizeUnits = "MiB";
        size /= 1048576;
    } else {
        sizeUnits = "GiB";
        size /= 1073741824;
    }

    // MSVC (and maybe Windows in general?) workaround
#if defined(_WIN32) && !defined(__CYGWIN__)
    qint64 blockSizeInBytes = 512;
    qint64 blocks = (qint64) (entry_stat->st_size / blockSizeInBytes);
#else
    qint64 blockSizeInBytes = (qint64)entry_stat->st_blksize;
    qint64 blocks = (qint64)entry_stat->st_blocks;
#endif
    auto lastAccessT = entry_stat->st_atim;
    auto lastModT = entry_stat->st_mtim;
    auto lastStatusModT = entry_stat->st_ctim;

    auto ft = archive_entry_filetype(entry);
    QString FileType;
    switch(ft) {
    case AE_IFREG: // Regular file
        FileType = "RegularFile";
        break;
    case AE_IFLNK: // Link
        FileType = "SymbolicLink";
        break;
    case AE_IFSOCK: // Socket
        FileType = "Socket";
        break;
    case AE_IFCHR: // Character Device
        FileType = "CharacterDevice";
        break;
    case AE_IFBLK: // Block Device
        FileType = "BlockDevice";
        break;
    case AE_IFDIR: // Directory.
        FileType = "Directory";
        break;
    case AE_IFIFO: // Named PIPE. (fifo)
        FileType = "NamedPipe";
        break;
    default:
        FileType = "UnknownFile";
        break;
    };

    QFile fileInfo(CurrentFile);
    // Set the values.
    if(FileType != "RegularFile") {
        CurrentEntry.insert("FileName", getDirectoryFileName(CurrentFile));
    } else {
        CurrentEntry.insert("FileName", fileInfo.fileName());
    }

    CurrentEntry.insert("FileType", QJsonValue(FileType));
    CurrentEntry.insert("Size", QJsonValue(size));
    CurrentEntry.insert("SizeUnit", sizeUnits);
    CurrentEntry.insert("BlockSize", QJsonValue(blockSizeInBytes));
    CurrentEntry.insert("BlockSizeUnit", "Bytes");
    CurrentEntry.insert("Blocks", QJsonValue(blocks));
    CurrentEntry.insert("SizeInByte", QJsonValue((qint64)entry_stat->st_size));

    if(lastAccessT) {
        CurrentEntry.insert("LastAccessedTimestamp", QJsonValue((qint64)lastAccessT));
        CurrentEntry.insert("LastAccessedTime",
                            QJsonValue(
                                (
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
                                    QDateTime::fromSecsSinceEpoch(lastAccessT)
#else
                                    QDateTime::fromTime_t(lastAccessT)
#endif
                                ).toString(Qt::ISODate)));
    } else {
        CurrentEntry.insert("LastAccessedTimestamp", QJsonValue(0));
        CurrentEntry.insert("LastAccessedTime", "Unknown");
    }

    if(lastModT) {
        CurrentEntry.insert("LastModifiedTimestamp", QJsonValue((qint64)lastModT));
        CurrentEntry.insert("LastModifiedTime",
                            QJsonValue((
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
                                           QDateTime::fromSecsSinceEpoch(lastModT)
#else
                                           QDateTime::fromTime_t(lastModT)
#endif
                                       ).toString(Qt::ISODate)));
    } else {
        CurrentEntry.insert("LastModifiedTimestamp", QJsonValue(0));
        CurrentEntry.insert("LastModifiedTime", "Unknown");
    }

    if(lastStatusModT) {
        CurrentEntry.insert("LastStatusModifiedTimestamp", QJsonValue((qint64)lastStatusModT));
        CurrentEntry.insert("LastStatusModifiedTime",
                            QJsonValue((
#if (QT_VERSION >= QT_VERSION_CHECK(5, 8, 0))
                                           QDateTime::fromSecsSinceEpoch(lastStatusModT)
#else
                                           QDateTime::fromTime_t(lastStatusModT)
#endif
                                       ).toString(Qt::ISODate)));
    } else {
        CurrentEntry.insert("LastStatusModifiedTimestamp", QJsonValue(0));
        CurrentEntry.insert("LastStatusModifiedTime", "Unknown");
    }

    return CurrentEntry;
}

// ExtractorPrivate constructor constructs the object which is the private class
// implementation to the DiskExtractor.
// This class is responsible for extraction and information retrival of the data
// inside an archive.
// This class only extracts the data to the disk and hence the name DiskExtractor.
// This class will not be able to extract or work in-memory.
ExtractorPrivate::ExtractorPrivate(bool memoryMode)
    : QObject(),
      n_Flags(ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | ARCHIVE_EXTRACT_SECURE_NODOTDOT) {
    b_MemoryMode = memoryMode;

    m_Info.reset(new QJsonObject);
    m_ExtractFilters.reset(new QStringList);

    if(b_MemoryMode) {
        m_ExtractedFiles.reset(new QVector<MemoryFile>);
    }
}

ExtractorPrivate::~ExtractorPrivate() {
    clear();
}

// Sets the given pointer to QIODevice as the Archive file itself.
void ExtractorPrivate::setArchive(QIODevice *archive) {
    if(b_Started || b_Paused) {
        return;
    }
    clear();
    m_Archive = archive;
    return;
}

// Sets the archive path as the given QString which will be later
// opened to be used as the Archive file.
void ExtractorPrivate::setArchive(const QString &archivePath) {
    if(b_Started || b_Paused || archivePath.isEmpty()) {
        return;
    }
    clear();
    m_ArchivePath = archivePath;
    return;
}

// Blocksize to be used when extracting the given archive.
void ExtractorPrivate::setBlockSize(int n) {
    if(b_Started || b_Paused) {
        return;
    }
    n_BlockSize = n;
    return;
}

// Sets the directory where the extraction data to be extracted.
void ExtractorPrivate::setOutputDirectory(const QString &destination) {
    if(b_MemoryMode || b_Started || b_Paused || destination.isEmpty()) {
        return;
    }
    m_OutputDirectory = destination + "/";
    return;
}

// Enables/Disables the progress of the extraction with respect to the
// given bool.
void ExtractorPrivate::setCalculateProgress(bool c) {
    b_NoProgress = !c;
    return;
}

// Sets the password for the archive when extracting the data.
// This method should be accessible even if the extractor is started
// since the user may set password anytime.
void ExtractorPrivate::setPassword(const QString &passwd) {
#if ARCHIVE_VERSION_NUMBER >= 3003003
    if(passwd.isEmpty()) {
        return;
    }
    m_Password = passwd;
#else
    (void)passwd;
#endif
    return;
}

// Adds extract filters , if set , only the files in the filter
// will be extracted , the filter has to correspond to the exact
// path given in the archive.
void ExtractorPrivate::addFilter(const QString &filter) {
    if(b_Started || b_Paused || filter.isEmpty()) {
        return;
    }
    *(m_ExtractFilters.data()) << filter;
    return;
}

// Overload of addFilter to accept list of QStrings.
void ExtractorPrivate::addFilter(const QStringList &filters) {
    if(b_Started || b_Paused || filters.isEmpty()) {
        return;
    }
    *(m_ExtractFilters.data()) << filters;
    return;
}

// Clears all internal data and sets it back to default.
void ExtractorPrivate::clear() {
    if(b_Started) {
        return;
    }
    n_BlockSize = 10240;
    n_PasswordTriedCountGetInfo = n_PasswordTriedCountExtract = 0;
    n_TotalEntries = -1;
    b_PauseRequested = b_CancelRequested = b_Paused = b_Started = b_Finished = b_ArchiveOpened = false;

    // TODO: do we need to reset n_BytesTotal here?
    n_BytesProcessed = 0;
    n_BytesTotal = 0;

    m_ArchivePath.clear();
#if ARCHIVE_VERSION_NUMBER >= 3003003
    m_Password.clear();
#endif
    m_OutputDirectory.clear();
    m_ArchiveRead.clear();
    m_ArchiveWrite.clear();
    m_Info.reset(new QJsonObject);
    m_ExtractFilters->clear();

    if(b_MemoryMode) {
        m_ExtractedFiles.reset(new QVector<MemoryFile>);
    }

    if(b_QIODeviceOwned) {
        m_Archive->close();
        m_Archive->deleteLater();
    } else {
        m_Archive = nullptr;
    }
    m_CurrentArchiveEntry = nullptr;
    b_QIODeviceOwned = false;
    return;
}

// Returns the information of the archive through info signal.
// BM: getInfo()
void ExtractorPrivate::getInfo() {
    if(!m_Info->isEmpty()) {
        emit info(*(m_Info.data()));
        return;
    }
    short errorCode = NoError;

    // Open the Archive.
    if((errorCode = openArchive()) != NoError) {
        emit error(errorCode);
        return;
    }

    errorCode = processArchiveInformation();
    if(!errorCode) {
        emit info(*(m_Info.data()));
    }
#if ARCHIVE_VERSION_NUMBER >= 3003003
    else if(errorCode == ArchivePasswordIncorrect || errorCode == ArchivePasswordNeeded) {
        emit getInfoRequirePassword(n_PasswordTriedCountGetInfo);
        ++n_PasswordTriedCountGetInfo;
    }
#endif
    else {
        emit error(errorCode);
    }
    return;
}

void ExtractorPrivate::start() {
    if(b_Started || b_Paused) {
        return;
    }

    short errorCode = NoError;

    // Open the Archive.
    if((errorCode = openArchive()) != NoError) {
        emit error(errorCode);
        return;
    }

    // Check and Set Output Directory.
    // If it's not memory mode.
    if(!b_MemoryMode) {
        if(!m_OutputDirectory.isEmpty()) {
            if((errorCode = checkOutputDirectory()) != NoError) {
                emit error(errorCode );
                return;
            }
        }
    }

    // All Okay then start the extraction.
    b_Started = true;
    b_Finished = false;
    emit started();

    // Get basic information about the archive if the user wants progress on the
    // extraction.
    if(n_TotalEntries == -1 && !b_NoProgress) {
        errorCode = getTotalEntriesCount();
        if(n_TotalEntries == -1) {
            // If the total entries is unchanged then there must be an
            // error.
#if ARCHIVE_VERSION_NUMBER >= 3003003
            b_Started = false;
            if(errorCode == ArchivePasswordIncorrect || errorCode == ArchivePasswordNeeded) {
                emit extractionRequirePassword(n_PasswordTriedCountExtract);
                ++n_PasswordTriedCountExtract;
            }
#endif
            emit error(errorCode);
            return;
        }
    }

    n_BytesProcessed = 0;

    errorCode = extract();
    if(errorCode == NoError) {
        b_Started = false;
        b_Finished = true;
        m_Archive->close();
        if(!b_MemoryMode) {
            emit diskFinished();
        } else {
            emit memoryFinished(new MemoryExtractorOutput(m_ExtractedFiles.take()));
            m_ExtractedFiles.reset(new QVector<MemoryFile>);
        }
    }
#if ARCHIVE_VERSION_NUMBER >= 3003003
    else if(errorCode == ArchivePasswordIncorrect || errorCode == ArchivePasswordNeeded) {
        b_Started = false;
        emit extractionRequirePassword(n_PasswordTriedCountExtract);
        ++n_PasswordTriedCountExtract;
        emit error(errorCode);
    }
#endif
    else if(errorCode == OperationCanceled) {
        b_Started = false;
        emit canceled();
    } else if(errorCode == OperationPaused) {
        b_Started = false;
        b_Paused = true;
        emit paused();
    } else {
        b_Started = false;
        emit error(errorCode );
    }
    return;
}

// Pauses the extractor.
void ExtractorPrivate::pause() {
    if(b_Started && !b_Paused) {
        b_PauseRequested = true;
    }
    return;
}

// Resumes the extractor.
void ExtractorPrivate::resume() {
    if(!b_Paused) {
        return;
    }
    b_Paused = false;
    b_Started = true;
    emit resumed();

    short ret = extract();
    if(ret == NoError) {
        b_Started = false;
        b_Finished = true;
        m_Archive->close();
        if(!b_MemoryMode) {
            emit diskFinished();
        } else {
            emit memoryFinished(new MemoryExtractorOutput(m_ExtractedFiles.take()));
            m_ExtractedFiles.reset(new QVector<MemoryFile>);
        }
    }
#if ARCHIVE_VERSION_NUMBER >= 3003003
    else if(ret == ArchivePasswordIncorrect || ret == ArchivePasswordNeeded) {
        b_Started = false;
        emit extractionRequirePassword(n_PasswordTriedCountExtract);
        ++n_PasswordTriedCountExtract;
        emit error(ret);
    }
#endif
    else if(ret == OperationCanceled) {
        b_Started = false;
        emit canceled();
    } else if(ret == OperationPaused) {
        b_Started = false;
        b_Paused = true;
        emit paused();
    } else {
        b_Started = false;
        emit error(ret );
    }
    return;
}

// Cancels the extraction.
void ExtractorPrivate::cancel() {
    if(b_Started && !b_Paused && !b_Finished) {
        b_CancelRequested = true;
    }
    return;
}


short ExtractorPrivate::openArchive() {
    if(m_ArchivePath.isEmpty() && !m_Archive) {
        return ArchiveNotGiven;
    } else if(b_ArchiveOpened) {
        return NoError;
    }
    // Check and Open the given archive.
    //
    // Note:
    // At this point of code either m_ArchivePath or m_Archive has to be
    // set or else the function should have exited with an error signal.
    if(!m_ArchivePath.isEmpty()) {

        QFileInfo info(m_ArchivePath);
        // Check if the file exists.
        if(!info.exists()) {
            return ArchiveDoesNotExists;
        } else if(!info.isFile()) { // Check if its really a file.
            return InvalidArchiveFile;
        }

        // Check if we have the permission to read it.
        auto perm = info.permissions();
        if(
            !(perm & QFileDevice::ReadUser) &&
            !(perm & QFileDevice::ReadGroup) &&
            !(perm & QFileDevice::ReadOther)
        ) {
            return NoPermissionToReadArchive;
        }


        QFile *file;
        try {
            file = new QFile(this);
        } catch ( ... ) {
            m_Archive = nullptr; // Just a precaution.
            return NotEnoughMemory;
        }
        file->setFileName(m_ArchivePath);

        // Finally open the file.
        if(!file->open(QIODevice::ReadOnly)) {
            file->deleteLater();
            return CannotOpenArchive;
        }

        b_QIODeviceOwned = true;
        m_Archive = (QIODevice*)file;
    } else {
        /// Open the archive if it's not opened.
        if(!m_Archive->isOpen() && !m_Archive->open(QIODevice::ReadOnly)) {
            return ArchiveIsNotOpened;
        }

        if(!m_Archive->isReadable()) { // Check if it is readable.
            return ArchiveIsNotReadable;
        }
    }
    b_ArchiveOpened = true;
    return NoError;
}

short ExtractorPrivate::checkOutputDirectory() {
    QFileInfo info(m_OutputDirectory + "/");
    // Check if its a directory and not a file , Also check if it exists.
    if(!info.exists() || !info.isDir()) {
        return InvalidOutputDirectory;
    }

    // Check if we have the permission to read and write.
    if(!info.isWritable() || !info.isReadable()) {
        return NoPermissionToWrite;
    }
    m_OutputDirectory = info.absoluteFilePath();
    return NoError;
}

short ExtractorPrivate::extract() {
    if(m_Archive == nullptr) {
        return ArchiveNotGiven;
    }
    int ret = 0;
    short err = NoError;
    archive_entry *entry = nullptr;

    if(m_ArchiveRead.isNull() && (b_MemoryMode || m_ArchiveWrite.isNull())) {
        n_ProcessedEntries = 0;

        m_ArchiveRead = QSharedPointer<struct archive>(archive_read_new(), ArchiveReadDestructor);
        if(!b_MemoryMode) {
            m_ArchiveWrite = QSharedPointer<struct archive>(archive_write_disk_new(), ArchiveWriteDestructor);
            if(!m_ArchiveRead.data() || !m_ArchiveWrite.data()) {
                m_ArchiveRead.clear();
                m_ArchiveWrite.clear();
                return NotEnoughMemory;
            }
        } else if(!m_ArchiveRead.data()) {
            m_ArchiveRead.clear();
            return NotEnoughMemory;
        }

#if ARCHIVE_VERSION_NUMBER >= 3003003
        if(!m_Password.isEmpty()) {
            archive_read_add_passphrase(m_ArchiveRead.data(), m_Password.toUtf8().constData());
        }
#endif
        archive_read_support_format_all(m_ArchiveRead.data());
        archive_read_support_filter_all(m_ArchiveRead.data());

        if((ret = archiveReadOpenQIODevice(m_ArchiveRead.data(), n_BlockSize, m_Archive))) {
            m_ArchiveRead.clear();
            m_ArchiveWrite.clear();
            return ArchiveReadError;
        }

        if(!b_MemoryMode) {
            if((ret = archive_write_disk_set_options(m_ArchiveWrite.data(), n_Flags))) {
                m_ArchiveRead.clear();
                m_ArchiveWrite.clear();
                return ArchiveWriteError;
            }
        }

    }
    for (;;) {
        if(m_CurrentArchiveEntry) {
            err = writeData(m_CurrentArchiveEntry);
            if(err == OperationPaused) {
                return err;
            } else if(err) { // NoError = 0
                m_ArchiveRead.clear();
                m_ArchiveWrite.clear();
                return err;
            }
            ++n_ProcessedEntries;

            // Report final progress signal after extracting the file fully.
            if(n_BytesTotal > 0 && n_TotalEntries > 0) {
                emit progress(QString::fromStdWString(archive_entry_pathname_w(m_CurrentArchiveEntry)),
                              n_ProcessedEntries,
                              n_TotalEntries,
                              n_BytesProcessed, n_BytesTotal);
            } else {
                emit progress(QString::fromStdWString(archive_entry_pathname_w(m_CurrentArchiveEntry)),
                              1,
                              1,
                              1,
                              1);

            }

            m_CurrentArchiveEntry = nullptr;
        }
        ret = archive_read_next_header(m_ArchiveRead.data(), &entry);
        if (ret == ARCHIVE_EOF) {
            break;
        }
        if (ret != ARCHIVE_OK) {
            err = ArchiveCorrupted;
            if(PASSWORD_NEEDED(m_ArchiveRead.data())) {
                err = ArchivePasswordNeeded;
            } else if(PASSWORD_INCORRECT(m_ArchiveRead.data())) {
                err = ArchivePasswordIncorrect;
            }
            m_ArchiveRead.clear();
            m_ArchiveWrite.clear();
            return err;
        }

        err = writeData(entry);
        if(err == OperationPaused) {
            return err;
        } else if(err) { // NoError = 0
            m_ArchiveRead.clear();
            m_ArchiveWrite.clear();
            return err;
        }
        ++n_ProcessedEntries;

        // Report final progress signal after extracting the file fully.
        if(n_BytesTotal > 0 && n_TotalEntries > 0) {
            emit progress(QString::fromStdWString(archive_entry_pathname_w(entry)),
                          n_ProcessedEntries,
                          n_TotalEntries,
                          n_BytesProcessed, n_BytesTotal);
        } else {
            emit progress(QString::fromStdWString(archive_entry_pathname_w(entry)),
                          1,
                          1,
                          1,
                          1);

        }
        QCoreApplication::processEvents(); // call event loop for the signal to take effect.
    }

    /* free memory. */
    m_ArchiveRead.clear();
    m_ArchiveWrite.clear();
    return NoError;
}

short ExtractorPrivate::writeData(struct archive_entry *entry) {
    if(m_ArchiveRead.isNull() || (!b_MemoryMode && m_ArchiveWrite.isNull()) || m_Archive == nullptr) {
        return ArchiveNotGiven;
    }

    if(!m_ExtractFilters->isEmpty() &&
            !m_ExtractFilters->contains(QString::fromStdWString(archive_entry_pathname_w(entry)))) {
        return NoError;
    }

    if(!b_MemoryMode) {
        if(!m_OutputDirectory.isEmpty()) {
            QDir dir(m_OutputDirectory);
            QString outputPath = dir.absoluteFilePath(QString::fromStdWString(archive_entry_pathname_w(entry)));
            archive_entry_set_pathname_utf8(entry, outputPath.toUtf8().constData());
        }
    }

    MutableMemoryFile currentNode;
    int ret = ARCHIVE_OK;
    if(m_CurrentArchiveEntry != entry) {
        if(!b_MemoryMode) {
            ret = archive_write_header(m_ArchiveWrite.data(), entry);
        } else {
            currentNode.setFileInformation(getArchiveEntryInformation(entry));

            /// Skip Directories.
            if((currentNode.getFileInformation()).value("FileType").toString() == "Directory") {
                return NoError;
            }

            currentNode.setBuffer(new QBuffer);

            if((currentNode.getBuffer())->open(QIODevice::ReadWrite) == false) {
                return ArchiveHeaderWriteError;
            }

            m_CurrentMemoryFile = currentNode;
        }
    } else {
        currentNode = m_CurrentMemoryFile;
    }
    if (ret == ARCHIVE_OK) {
        const void *buff;
        size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
        int64_t offset;
#else
        off_t offset;
#endif
        for (;;) {
            ret = archive_read_data_block(m_ArchiveRead.data(), &buff, &size, &offset);
            if (ret == ARCHIVE_EOF) {
                break;
            } else if (ret != ARCHIVE_OK) {
                short err = ArchiveCorrupted;
                if(PASSWORD_NEEDED(m_ArchiveRead.data())) {
                    err = ArchivePasswordNeeded;
                } else if(PASSWORD_INCORRECT(m_ArchiveRead.data())) {
                    err = ArchivePasswordIncorrect;
                }
                return err;
            } else {
                if(!b_MemoryMode) {
                    ret = archive_write_data_block(m_ArchiveWrite.data(), buff, size, offset);
                    if (ret != ARCHIVE_OK) {
                        return ArchiveWriteError;
                    }
                } else {
                    (currentNode.getBuffer())->seek(offset);
                    if((currentNode.getBuffer())->write((const char*)buff, size) == -1) {
                        return ArchiveWriteError;
                    }
                }
                n_BytesProcessed += size;
                if(n_BytesTotal > 0 && n_TotalEntries > 0) {
                    emit progress(QString::fromStdWString(archive_entry_pathname_w(entry)),
                                  n_ProcessedEntries,
                                  n_TotalEntries,
                                  n_BytesProcessed, n_BytesTotal);
                }

            }

            // Allow the execution of the event loop
            QCoreApplication::processEvents();

            // Check for pause and cancel requests.
            if(b_PauseRequested) {
                b_PauseRequested = false;
                m_CurrentArchiveEntry = entry;
                return OperationPaused;
            } else if(b_CancelRequested) {
                b_CancelRequested = false;
                return OperationCanceled;
            }

        }
    } else {
        return ArchiveHeaderWriteError;
    }

    if(!b_MemoryMode) {
        ret = archive_write_finish_entry(m_ArchiveWrite.data());
        if (ret == ARCHIVE_FATAL) {
            return ArchiveHeaderWriteError;
        }
    } else {
        (currentNode.getBuffer())->close();
        m_ExtractedFiles->append(
            MemoryFile(
                currentNode.getFileInformation(),
                currentNode.getBuffer()));
        m_CurrentMemoryFile = MutableMemoryFile();
    }
    return NoError;
}

short ExtractorPrivate::getTotalEntriesCount() {
    if(!m_Archive) {
        return ArchiveNotGiven;
    }

    n_BytesTotal = 0;
    int ret = 0;
    int count = 0;
    archive_entry *entry = nullptr;
    struct archive *inArchive = archive_read_new();
    if(!inArchive) {
        return NotEnoughMemory;
    }

#if ARCHIVE_VERSION_NUMBER >= 3003003
    if(!m_Password.isEmpty()) {
        archive_read_add_passphrase(inArchive, m_Password.toUtf8().constData());
    }
#endif

    archive_read_support_format_all(inArchive);
    archive_read_support_filter_all(inArchive);
    if((ret = archiveReadOpenQIODevice(inArchive, n_BlockSize, m_Archive))) {
        archive_read_close(inArchive);
        archive_read_free(inArchive);
        return ArchiveReadError;
    }
    for (;;) {
        ret = archive_read_next_header(inArchive, &entry);
        if (ret == ARCHIVE_EOF) {
            break;
        }
        if (ret != ARCHIVE_OK) {
            short err = ArchiveCorrupted;
            if(PASSWORD_NEEDED(inArchive)) {
                err = ArchivePasswordNeeded;
            } else if(PASSWORD_INCORRECT(inArchive)) {
                err = ArchivePasswordIncorrect;
            }
            archive_read_close(inArchive);
            archive_read_free(inArchive);
            return err;
        }
        count += 1;
        n_BytesTotal += archive_entry_size(entry);
        QCoreApplication::processEvents();
    }

    // set total number of entries.
    n_TotalEntries = count;

    // free memory.
    archive_read_close(inArchive);
    archive_read_free(inArchive);
    return NoError;
}

short ExtractorPrivate::processArchiveInformation() {
    if(m_Archive == nullptr) {
        return ArchiveNotGiven;
    }

    int ret = 0;
    archive_entry *entry = nullptr;
    struct archive *inArchive = archive_read_new();
    if(!inArchive) {
        return NotEnoughMemory;
    }
#if ARCHIVE_VERSION_NUMBER >= 3003003
    if(!m_Password.isEmpty()) {
        archive_read_add_passphrase(inArchive, m_Password.toUtf8().constData());
    }
#endif
    archive_read_support_format_all(inArchive);
    archive_read_support_filter_all(inArchive);

    if((ret = archiveReadOpenQIODevice(inArchive, n_BlockSize, m_Archive))) {
        archive_read_close(inArchive);
        archive_read_free(inArchive);
        return ArchiveReadError;
    }
    for (;;) {
        ret = archive_read_next_header(inArchive, &entry);
        if (ret == ARCHIVE_EOF) {
            break;
        }
        if (ret != ARCHIVE_OK) {
            short err = ArchiveCorrupted;
            if(PASSWORD_NEEDED(inArchive)) {
                err = ArchivePasswordNeeded;
            } else if(PASSWORD_INCORRECT(inArchive)) {
                err = ArchivePasswordIncorrect;
            }
            archive_read_close(inArchive);
            archive_read_free(inArchive);
            return err;
        }

        QString CurrentFile = QString::fromStdWString(archive_entry_pathname_w(entry));
        QJsonObject CurrentEntry = getArchiveEntryInformation(entry);
        m_Info->insert(CurrentFile, CurrentEntry);
        n_BytesTotal += archive_entry_size(entry);
        QCoreApplication::processEvents();
    }

    // set total number of entries.
    n_TotalEntries = m_Info->size();

    // free memory.
    archive_read_close(inArchive);
    archive_read_free(inArchive);
    return NoError;
}
