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
