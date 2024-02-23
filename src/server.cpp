#include <kaho/server.h>
#include <kaho/localmodelregistry.h>

#include <QCoreApplication>

namespace kaho {
    Server::Server() {}

    void Server::start(const QString& url) {
        m_process = new QProcess(this);
        m_process->setReadChannel(QProcess::StandardOutput);
        m_process->setProcessChannelMode(QProcess::MergedChannels);
        m_process->setWorkingDirectory(QCoreApplication::applicationDirPath());
        QStringList arguments;
        auto model_file_path = LocalModelRegistry::resolve_filename(url);
        arguments << "-m" << model_file_path;
        auto program = QCoreApplication::applicationDirPath() + "/" + QString("server");
        m_process->start(program, arguments);
        connect(m_process, &QProcess::started, this, &Server::processStarted);
        connect(m_process, &QProcess::errorOccurred, this, &Server::processError);
    }

    void Server::processStarted() {
        qDebug() << "Process started successfully";
    }

    void Server::processError(QProcess::ProcessError error) {
        qDebug() << "Process had an error: " << error;
    }

    void Server::stop() {
        m_process->terminate();
    }
} // namespace kaho
