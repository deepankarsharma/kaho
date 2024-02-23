#pragma once

#include <QObject>
#include <QProcess>


namespace kaho {
    class Server : public QObject
    {
    Q_OBJECT
    public:
        Server();
        void start(const QString& url);
        void stop();
    signals:
        void answerFragmentReceived(const QString &text);

    public slots:
        void processStarted();
        void processError(QProcess::ProcessError error);
    private:
        QProcess* m_process;
    };
} // namespace kaho



