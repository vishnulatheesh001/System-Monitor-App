#include "mainwindow.h"
#include <QFile>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create lock file to signal C backend that UI is running
    QFile lockFile("C:/temp/ui_running.lock");
    if (lockFile.open(QIODevice::WriteOnly)) {
        lockFile.close();
    }

    MainWindow w;
    w.show();

    QFile::remove("C:/temp/ui_running.lock");

    return a.exec();
}
