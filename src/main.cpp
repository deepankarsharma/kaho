#include <kaho/mainwindow.h>

#include <QApplication>
#include <QStyleHints>
#include <QStyleFactory>
#include <kaho/file_utils.h>
#ifdef Q_OS_MAC
#include <updater/CocoaInitializer.h>
#include <updater/SparkleAutoUpdater.h>
#include <QStandardPaths>
#include <kaho/errors.h>
#endif

using namespace kaho;

// returns non zero on error
ErrorCode initialize() {
    RETURN_ON_ERROR(is_error(ensure_dir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/models")), ErrorCode::IO_FAILED, "initialize() -> Error creating models directory");
    return ErrorCode::OK;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setGeometry(QRect(0, 0, 1200, 800));
    if (!is_ok(initialize())) {
        qDebug() << "Error initializing";
        return -1;
    }
    w.show();    


#ifdef Q_OS_MAC
    AutoUpdater* updater = nullptr;
	CocoaInitializer initializer;
	updater = new SparkleAutoUpdater("https://mac-updates.dz9.org/appcast.xml");
    if (updater) {
        updater->setAutomaticallyChecksForUpdates(true);
        updater->setAutomaticallyDownloadsUpdates(true);
	}
#endif
    return a.exec();
}
