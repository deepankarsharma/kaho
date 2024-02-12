#include <kaho/mainwindow.h>

#include <QApplication>
#include <QStyleHints>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    // QApplication::setStyle(QStyleFactory::create("fusion"));
    auto styleHints = QGuiApplication::styleHints();
    auto colorScheme = styleHints->colorScheme();
    qDebug() << colorScheme;
    w.setGeometry(QRect(0, 0, 1200, 800));
    w.show();
    return a.exec();
}
