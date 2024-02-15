#include <kaho/server.h>

#include <QCoreApplication>

Server::Server() : m_process(0) {}

void Server::start() {
    m_process = new QProcess(this);
    m_process->setReadChannel(QProcess::StandardOutput);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    QStringList arguments;
    arguments << "-m" << "~/models/openhermes-2.5-mistral-7b.Q4_K_M.gguf";
    auto program = QCoreApplication::applicationDirPath() +"/"+QString("server");
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
