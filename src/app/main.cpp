#include <QApplication>

#include "global.h"
#include "db.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(APP_NAME);
    app.setApplicationDisplayName(APP_DISPLAY_NAME);
    app.setApplicationVersion(APP_VERSION_STR);
    app.setProperty("settings-path", qApp->applicationDirPath().append("/" APP_SETTINGS_FILENAME));

    db::init();

    MainWindow mw;
    mw.showMaximized();

    return app.exec();
}
