#pragma once
#include <QObject>
#include <QNetworkAccessManager>

namespace kaho {
    class AIModel : public QObject
    {
    Q_OBJECT
    public:
        explicit AIModel(QObject *parent = nullptr);
    private:
        void make_request(const QString& prompt);
    signals:
        void answerFragmentReceived(const QString &text);

    public slots:
        void processPrompt(const QString &prompt);

    private:
        QNetworkAccessManager* m_network_manager;
    };
}
