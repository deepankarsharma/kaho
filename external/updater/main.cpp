/*
 * Copyright (C) 2008 Remko Troncon
 */

#include <QApplication>
#include <QLabel>
#include <QDebug>

#ifdef Q_OS_MAC
#include "CocoaInitializer.h"
#include "SparkleAutoUpdater.h"
#endif

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	QLabel l("This is an auto-updating application");
	l.show();

	AutoUpdater* updater;
#ifdef Q_OS_MAC
	CocoaInitializer initializer;
	updater = new SparkleAutoUpdater();
#endif
	if (updater) {
		updater->checkForUpdates();
        qDebug() << "automaticallyChecksForUpdates" << updater->automaticallyChecksForUpdates();
        qDebug() << "automaticallyDownloadsUpdates" << updater->automaticallyDownloadsUpdates();
	}
	return app.exec();
}
