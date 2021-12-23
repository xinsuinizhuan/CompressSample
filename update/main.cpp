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
#include "UpdateWnd.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[]) {
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    QIcon winIco(":/update/images/logo.ico");
    if (!winIco.isNull())
        a.setWindowIcon(winIco);

    QString qss;
    QFile qssFile(":/update/update.qss");
    qssFile.open(QFile::ReadOnly);
    if (qssFile.isOpen()) {
        qss = QString::fromUtf8(qssFile.readAll());
        qssFile.close();

        qApp->setStyleSheet(qss);
    }

    UpdateWnd w;
    return a.exec();
}
