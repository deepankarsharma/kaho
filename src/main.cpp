#include <kaho/mainwindow.h>

#include <QApplication>
#include <QStyleHints>
#include <QStyleFactory>

#ifdef Q_OS_MAC
#include <updater/CocoaInitializer.h>
#include <updater/SparkleAutoUpdater.h>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setGeometry(QRect(0, 0, 1200, 800));
    w.show();

    AutoUpdater* updater = nullptr;
#ifdef Q_OS_MAC
	CocoaInitializer initializer;
	updater = new SparkleAutoUpdater();
#endif
	if (updater) {
		updater->checkForUpdates();
        qDebug() << "automaticallyChecksForUpdates" << updater->automaticallyChecksForUpdates();
        qDebug() << "automaticallyDownloadsUpdates" << updater->automaticallyDownloadsUpdates();
	}

    return a.exec();
}
