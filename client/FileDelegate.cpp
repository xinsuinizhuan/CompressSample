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
#include "FileDelegate.h"
#include "FileModel.h"
#include "OneLineEditer.h"

namespace {
const int kIconTxtSpace = 4;
void* qMalloc(size_t size) {
    return ::malloc(size);
}
void qFree(void* ptr) {
    ::free(ptr);
}

QImage qt_fromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h) {
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = w * h * 4;

    QImage image(w, h, QImage::Format_ARGB32_Premultiplied);
    if (image.isNull())
        return image;

    // Get bitmap bits
    uchar* data = (uchar*)qMalloc(bmi.bmiHeader.biSizeImage);

    if (GetDIBits(hdc, bitmap, 0, h, data, &bmi, DIB_RGB_COLORS)) {
        // Create image and copy data into image.
        for (int y = 0; y < h; ++y) {
            void* dest = (void*)image.scanLine(y);
            void* src = data + y * image.bytesPerLine();
            memcpy(dest, src, image.bytesPerLine());
        }
    }
    else {
        qWarning("qt_fromWinHBITMAP(), failed to get bitmap bits");
    }
    qFree(data);

    return image;
}

QPixmap fromWinHICON(HICON icon)  //qt4.7 QPixmap::fromWinHICON(hIcon)
{
    bool foundAlpha = false;
    HDC screenDevice = GetDC(0);
    HDC hdc = CreateCompatibleDC(screenDevice);
    ReleaseDC(0, screenDevice);

    ICONINFO iconinfo;
    bool result = GetIconInfo(icon, &iconinfo);  //x and y Hotspot describes the icon center
    if (!result)
        qWarning("QPixmap::fromWinHICON(), failed to GetIconInfo()");

    int w = iconinfo.xHotspot * 2;
    int h = iconinfo.yHotspot * 2;

    BITMAPINFOHEADER bitmapInfo;
    bitmapInfo.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.biWidth = w;
    bitmapInfo.biHeight = h;
    bitmapInfo.biPlanes = 1;
    bitmapInfo.biBitCount = 32;
    bitmapInfo.biCompression = BI_RGB;
    bitmapInfo.biSizeImage = 0;
    bitmapInfo.biXPelsPerMeter = 0;
    bitmapInfo.biYPelsPerMeter = 0;
    bitmapInfo.biClrUsed = 0;
    bitmapInfo.biClrImportant = 0;
    DWORD* bits;

    HBITMAP winBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bitmapInfo, DIB_RGB_COLORS, (VOID**)&bits, NULL, 0);
    HGDIOBJ oldhdc = (HBITMAP)SelectObject(hdc, winBitmap);
    DrawIconEx(hdc, 0, 0, icon, iconinfo.xHotspot * 2, iconinfo.yHotspot * 2, 0, 0, DI_NORMAL);
    QImage image = qt_fromWinHBITMAP(hdc, winBitmap, w, h);

    for (int y = 0; y < h && !foundAlpha; y++) {
        QRgb* scanLine = reinterpret_cast<QRgb*>(image.scanLine(y));
        for (int x = 0; x < w; x++) {
            if (qAlpha(scanLine[x]) != 0) {
                foundAlpha = true;
                break;
            }
        }
    }
    if (!foundAlpha) {
        //If no alpha was found, we use the mask to set alpha values
        DrawIconEx(hdc, 0, 0, icon, w, h, 0, 0, DI_MASK);
        QImage mask = qt_fromWinHBITMAP(hdc, winBitmap, w, h);

        for (int y = 0; y < h; y++) {
            QRgb* scanlineImage = reinterpret_cast<QRgb*>(image.scanLine(y));
            QRgb* scanlineMask = mask.isNull() ? 0 : reinterpret_cast<QRgb*>(mask.scanLine(y));
            for (int x = 0; x < w; x++) {
                if (scanlineMask && qRed(scanlineMask[x]) != 0)
                    scanlineImage[x] = 0;  //mask out this pixel
                else
                    scanlineImage[x] |= 0xff000000;  // set the alpha channel to 255
            }
        }
    }
    //dispose resources created by iconinfo call
    DeleteObject(iconinfo.hbmMask);
    DeleteObject(iconinfo.hbmColor);

    SelectObject(hdc, oldhdc);  //restore state
    DeleteObject(winBitmap);
    DeleteDC(hdc);
    return QPixmap::fromImage(image);
}
}  // namespace


FileDelegate::FileDelegate(QObject* parent /*= Q_NULLPTR*/) {
}

FileDelegate::~FileDelegate() {
}

void FileDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    if (index.column() == CI_DISPLAY_NAME) {  // 名称列
        const FileModel* fileMode = dynamic_cast<const FileModel*>(index.model());
        if (fileMode) {
            if (option.state & QStyle::State_Selected) {
                painter->fillRect(option.rect, QColor(0, 120, 215));
            }

            FileMeta fm = fileMode->getFileMeta(index);
            QPixmap pixmap;
            QString txt = index.data().toString();
            if (fm.ft == FileType::FT_DOTDOT_FOLDER) {
                txt = "..（上级目录）";
                pixmap.load(":/images/images/folder.png");
            }
            else if (fm.ft == FileType::FT_FOLDER) {
                pixmap.load(":/images/images/folder.png", "png");
            }
            else if (fm.ft == FileType::FT_LOGIC_DRIVER) {
                pixmap = getDriverIcon(fm.strAbsolutePath);
            }
            else {
                pixmap = getFileIcon(fm.strAbsolutePath);
            }

            const QWidget* widget = option.widget;
            float dpiScale = widget ? widget->devicePixelRatioF() : 1.f;
            QStyle* style = widget ? widget->style() : QApplication::style();
            float ratio = (float)pixmap.width() / (float)pixmap.height();

            int targetWidth = option.rect.height() - 2 * 2;
            int targetHeight = targetWidth;

            int pixScaledWidth = targetWidth;
            if (pixScaledWidth > pixmap.width())
                pixScaledWidth = pixmap.width();
            int pixScaledHeight = (float)pixScaledWidth / ratio;

            if (pixScaledHeight > targetHeight) {
                pixScaledHeight = targetHeight;
                pixScaledWidth = (float)pixScaledHeight * ratio;
            }

            QRect pixRect;
            pixRect.setLeft(option.rect.left() + (targetWidth - pixScaledWidth) / 2);
            pixRect.setTop(option.rect.top() + (targetHeight - pixScaledHeight) / 2);
            pixRect.setWidth(pixScaledWidth);
            pixRect.setHeight(pixScaledHeight);

            QPixmap scaledPixmap = pixmap.scaled(pixRect.width(), pixRect.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

            style->drawItemPixmap(painter, pixRect, (int)(Qt::AlignCenter), scaledPixmap);

            QRect txtRect;
            txtRect.setLeft(option.rect.left() + targetWidth + kIconTxtSpace);
            txtRect.setTop(option.rect.top());
            txtRect.setWidth(option.rect.width() - targetWidth - kIconTxtSpace);
            txtRect.setHeight(option.rect.height());
            style->drawItemText(painter, txtRect, (int)(Qt::AlignLeft | Qt::AlignVCenter), widget->palette(), true, txt);

            return;
        }
    }

    QStyledItemDelegate::paint(painter, option, index);
}

QWidget* FileDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    if (index.column() == CI_DISPLAY_NAME) {
        OneLineEditer* edit = new OneLineEditer(parent);
        edit->setFocusPolicy(Qt::StrongFocus);
        connect(edit, &OneLineEditer::editFinished, this, &FileDelegate::onEditFinished);
        return edit;
    }

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void FileDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    if (index.column() == CI_DISPLAY_NAME) {
        if (editor) {
            int imgWidth = option.rect.height() - 2 * 2;
            editor->setGeometry(option.rect.left() + imgWidth + kIconTxtSpace, option.rect.top(),
                                option.rect.width() - imgWidth - kIconTxtSpace, option.rect.height());
        }
        return;
    }

    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

void FileDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
    if (index.column() == 0) {
        OneLineEditer* customEdit = dynamic_cast<OneLineEditer*>(editor);
        if (customEdit) {
            customEdit->setText(index.data().toString());
        }
        return;
    }
    QStyledItemDelegate::setEditorData(editor, index);
}

void FileDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
    if (index.column() == CI_DISPLAY_NAME) {
        if (editor) {
            OneLineEditer* lineEdit = dynamic_cast<OneLineEditer*>(editor);
            QString txt = lineEdit->text();

            FileModel* fileModel = dynamic_cast<FileModel*>(model);
            if (fileModel) {
                bool ret = fileModel->rename(index, txt);
            }
        }
        return;
    }

    QStyledItemDelegate::setModelData(editor, model, index);
}

void FileDelegate::onEditFinished() {
    OneLineEditer* edit = (OneLineEditer*)sender();
    emit commitData(edit);
    emit closeEditor(edit);
}

QPixmap FileDelegate::getFileIcon(const QString& path) const {
    QPixmap pixmap;
    QFileInfo fileInfo(path);
    QString unParsedIconPath;

    if (fileInfo.isShortcut()) {
        unParsedIconPath = getShorcutIconPath(path);
    }
    else {
        unParsedIconPath = path;
    }

    if (!unParsedIconPath.isEmpty()) {
        QFileInfo iconFI(unParsedIconPath);
        if (iconFI.suffix().toLower() == "ico") {
            pixmap.load(unParsedIconPath);
        }
        else {
            WCHAR szPath[MAX_PATH] = {0};
            StringCchCopyW(szPath, MAX_PATH, unParsedIconPath.toStdWString().c_str());

            WORD dwIcoIndex = 0;
            HICON hIcon = ::ExtractAssociatedIconW(NULL, szPath, &dwIcoIndex);
            if (hIcon) {
                pixmap = fromWinHICON(hIcon);
                ::DestroyIcon(hIcon);
            }
        }
    }

    if (pixmap.isNull())
        pixmap.load(":/images/images/file.png");

    return pixmap;
}

QPixmap FileDelegate::getDriverIcon(const QString& driverRoot) const {
    QPixmap pixmap;
    WCHAR szPath[MAX_PATH] = {0};
    StringCchCopyW(szPath, MAX_PATH, driverRoot.toStdWString().c_str());

    WORD dwIcoIndex = 0;
    HICON hIcon = ::ExtractAssociatedIconW(NULL, szPath, &dwIcoIndex);
    if (hIcon) {
        pixmap = fromWinHICON(hIcon);
        ::DestroyIcon(hIcon);
    }

    return pixmap;
}

QString FileDelegate::getShorcutIconPath(const QString& path) const {
    QString ret;
    akali::ShortcutProperties sp;
    if (akali::ResolveShortcut(path.toStdWString(), sp)) {
        if (sp.icon.length() > 0)
            ret = QString::fromStdWString(sp.icon);
        else if (sp.target.length() > 0)
            ret = QString::fromStdWString(sp.target);
    }
    return ret;
}
