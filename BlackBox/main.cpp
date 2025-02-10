#include <QApplication>
#include "mainwindow.h"
#include <QGoodWindow>

int main(int argc, char *argv[])
{
    QGoodWindow::setup();

    QApplication app(argc, argv);
    QLocale::setDefault(QLocale::Russian);

    MainWindow mw;
    mw.show();

    return app.exec();
}
