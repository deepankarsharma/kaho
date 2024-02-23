#include <kaho/server.h>
#include <kaho/localmodelregistry.h>

#include <QCoreApplication>
#include <QTcpServer>

int findOpenPort()
{
    QTcpServer server;
    int port = 8080; // Start checking from port 1024
    while (port < 65535) { // Iterate through the range of valid ports
        if (server.listen(QHostAddress::Any, port)) {
            qDebug() << "Server is listening on port:" << port;
            return port; // Port is available
        } else {
            port++; // Try the next port
        }
    }
    qDebug() << "No open port found";
    return -1; // No open port found
}

namespace kaho {
    Server::Server() {}

    void Server::start(const QString& url) {
        m_process = new QProcess(this);
        m_process->setReadChannel(QProcess::StandardOutput);
        m_process->setProcessChannelMode(QProcess::MergedChannels);
        m_process->setWorkingDirectory(QCoreApplication::applicationDirPath());
        QStringList arguments;
        auto model_file_path = LocalModelRegistry::resolve_filename(url);
        auto port = findOpenPort();
        if (port < 0) {
            qDebug() << "Unable to start server since we could not find an open port";
            return;
        }
        arguments << "-m" << model_file_path << "--port" << QString::number(port);
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
