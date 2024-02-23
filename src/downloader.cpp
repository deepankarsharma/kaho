#include <kaho/downloader.h>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProgressDialog>
#include <QFile>
#include <QUrl>
#include <QObject>
#include <QDir>
#include <QLocale>

namespace kaho {
//    Downloader::Downloader(QObject *parent) : QObject(parent) {
//        connect(&m_manager, &QNetworkAccessManager::finished, this, &Downloader::onDownloadFinished);
//    }
//
//    void Downloader::downloadFile(const QUrl &url, const QString &destinationPath) {
//        QNetworkRequest request(url);
//        m_manager.get(request);
//        this->destinationPath = destinationPath;
//    }
//
//    void Downloader::onDownloadFinished(QNetworkReply *reply) {
//        if (reply->error() == QNetworkReply::NoError) {
//            QFile file(destinationPath);
//            if (file.open(QIODevice::WriteOnly)) {
//                file.write(reply->readAll());
//                file.close();
//                qDebug() << "Download completed!";
//            } else {
//                qDebug() << "Error: Couldn't open file for writing.";
//            }
//        } else {
//            qDebug() << "Error:" << reply->errorString();
//        }
//
//        reply->deleteLater();
//        QCoreApplication::quit();
//    }
//
//
//    void DownloadThread::run() {
//        QNetworkAccessManager manager;
//        QNetworkRequest request(m_url);
//
//        QNetworkReply *reply = manager.get(request);
//        connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 bytesRead, qint64 totalBytes) {
//            emit downloadProgress(bytesRead, totalBytes);
//        });
//
//        //connect(reply, &QNetworkReply::finished, this, &DownloadManager::downloadFinished);
////    connect(reply, &QIODevice::readyRead, [this, reply]() {
////        if (!file.isOpen()) {
////            file.setFileName("downloaded_file"); // Set your file name here
////            file.open(QIODevice::WriteOnly);
////        }
////        file.write(reply->readAll());
////    });
//
//        QEventLoop loop;
//        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
//        loop.exec();
//
//        if (reply->error() == QNetworkReply::NoError) {
//            QFile file(m_destinationPath);
//            if (file.open(QIODevice::WriteOnly)) {
//                file.write(reply->readAll());
//                file.close();
//                qDebug() << "Download completed!";
//            } else {
//                qDebug() << "Error: Couldn't open file for writing.";
//            }
//        } else {
//            qDebug() << "Error:" << reply->errorString();
//        }
//
//        reply->deleteLater();
//        emit downloadFinished();
//    }

    ErrorCode downloadFile(const QUrl& url, const QString& downloadDirectory) {
        qDebug() << "url " << url << " downloading to " << downloadDirectory;

        QNetworkAccessManager *manager = new QNetworkAccessManager();

        QNetworkReply *reply = manager->get(QNetworkRequest(url));

        // Progress dialog
        QProgressDialog progress("Downloading model on first run of app...", "Cancel", 0, 0, nullptr);
        progress.setWindowModality(Qt::WindowModal); // Make the dialog modal

        // Determine filename from the URL
        QString filename = QUrl(url.path()).fileName();
        if (filename.isEmpty()) {
            filename = "downloaded_file"; // Default fallback name
        }

        // Construct the full save path
        QString saveFilePath = QDir(downloadDirectory).filePath(filename);

        // Open the file right away for writing
        QFile file(saveFilePath);
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "Error opening file for writing:" << file.errorString();
            reply->deleteLater();
            manager->deleteLater();
            return ErrorCode::IO_FAILED;
        }

        // Connect download progress signal
        QObject::connect(reply, &QNetworkReply::downloadProgress,
                [&progress, &file, reply](qint64 bytesReceived, qint64 bytesTotal) {
                    file.write(reply->readAll());
                    // Update label with download status
                    // Update label with download status (human-readable)
                    QString statusText = QString("Downloading model on first run of app...\n\n%1 of %2 downloaded")
                            .arg(QLocale::system().formattedDataSize(bytesReceived))
                            .arg(QLocale::system().formattedDataSize(bytesTotal));
                    progress.setLabelText(statusText);

                    // Update progress bar if total size is known
                    if (bytesTotal != -1) {
                        progress.setMaximum(100);
                        progress.setValue(int( 100 * bytesReceived/bytesTotal));
                    }
                });

        QObject::connect(reply, &QNetworkReply::finished, [&reply, &progress, &manager, downloadDirectory, url, &file]() {
            progress.hide();
            if (reply->error()) {
                qDebug() << "Error: " << reply->errorString();
                reply->deleteLater();
                manager->deleteLater();
                return ErrorCode::IO_FAILED; // Exit on error
            }

            file.close();
            reply->deleteLater();
            manager->deleteLater();
            return ErrorCode::OK;
        });

        progress.exec(); // Show the progress dialog (exec() is blocking)
        return ErrorCode::OK;
    }

} // namespace kaho
