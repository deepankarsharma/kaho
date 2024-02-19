#include <kaho/downloader.h>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDebug>


Downloader::Downloader(QObject *parent) : QObject(parent)
{
    connect(&m_manager, &QNetworkAccessManager::finished, this, &Downloader::onDownloadFinished);
}

void Downloader::downloadFile(const QUrl &url, const QString &destinationPath)
{
    QNetworkRequest request(url);
    m_manager.get(request);
    this->destinationPath = destinationPath;
}

void Downloader::onDownloadFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QFile file(destinationPath);
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(reply->readAll());
            file.close();
            qDebug() << "Download completed!";
        }
        else
        {
            qDebug() << "Error: Couldn't open file for writing.";
        }
    }
    else
    {
        qDebug() << "Error:" << reply->errorString();
    }

    reply->deleteLater();
    QCoreApplication::quit();
}



void DownloadThread::run()
{
    QNetworkAccessManager manager;
    QNetworkRequest request(m_url);

    QNetworkReply *reply = manager.get(request);
    connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 bytesRead, qint64 totalBytes) {
        emit downloadProgress(bytesRead, totalBytes);
    });

    //connect(reply, &QNetworkReply::finished, this, &DownloadManager::downloadFinished);
//    connect(reply, &QIODevice::readyRead, [this, reply]() {
//        if (!file.isOpen()) {
//            file.setFileName("downloaded_file"); // Set your file name here
//            file.open(QIODevice::WriteOnly);
//        }
//        file.write(reply->readAll());
//    });

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError)
    {
        QFile file(m_destinationPath);
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(reply->readAll());
            file.close();
            qDebug() << "Download completed!";
        }
        else
        {
            qDebug() << "Error: Couldn't open file for writing.";
        }
    }
    else
    {
        qDebug() << "Error:" << reply->errorString();
    }

    reply->deleteLater();
    emit downloadFinished();
}
