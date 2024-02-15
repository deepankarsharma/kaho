#pragma once

#include <QObject>
#include <QProcess>

class Server : public QObject
{
    Q_OBJECT
public:
    Server();
    void start();
signals:
    void answerFragmentReceived(const QString &text);

public slots:
    void processStarted();
    void processError(QProcess::ProcessError error);
private:
    QProcess* m_process;
};


