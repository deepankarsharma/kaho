#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QThread>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = nullptr);
    void downloadFile(const QUrl& url, const QString& destinationPath);
private slots:
    void onDownloadFinished(QNetworkReply *reply);
private:
    QNetworkAccessManager m_manager;
    QString destinationPath;
};


class DownloadThread : public QThread
{
    Q_OBJECT
public:
    explicit DownloadThread(const QUrl &url, const QString &destinationPath, QObject *parent = nullptr)
        : QThread(parent), m_url(url), m_destinationPath(destinationPath) {}

signals:
    void downloadFinished();
    void downloadProgress(qint64 bytesRead, qint64 totalBytes);

protected:
    void run() override;

private:
    QUrl m_url;
    QString m_destinationPath;
};
