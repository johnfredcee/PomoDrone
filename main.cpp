#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);  // Prevents the app from quitting when hidden

    MainWindow w;
    w.hide();

    return app.exec();
}
