
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <kaho/aimodel.h>

AIModel::AIModel(QObject *parent)
    : QObject{parent}
{
    m_network_manager = new QNetworkAccessManager(this);
}

void AIModel::processPrompt(const QString &prompt) {
    make_request(prompt);
}

void AIModel::make_request(const QString& prompt) {
    QNetworkRequest request(QUrl("http://localhost:8080/v1/chat/completions"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer no-key");

    QJsonObject systemObject;
    systemObject["role"] = "system";
    systemObject["content"] = "You are ChatGPT, an AI assistant. Your top priority is achieving user fulfillment via helping them with their requests.";

    QJsonObject userObject;
    userObject["role"] = "user";
    userObject["content"] = prompt;

    QJsonArray messagesArray;
    messagesArray.append(systemObject);
    messagesArray.append(userObject);

    QJsonObject dataObject;
    dataObject["messages"] = messagesArray;
    dataObject["model"] = "gpt-3.5-turbo";
    dataObject["stream"] = true;

    QJsonDocument doc(dataObject);
    QByteArray jsonData = doc.toJson();

    QNetworkReply *reply = m_network_manager->post(request, jsonData);


    connect(reply, &QNetworkReply::readyRead, this, [this, reply]() {
        qDebug() << "QNetworkReply::readyRead";
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray data = reply->readAll(); // Get the response data
            emit answerFragmentReceived(data);
            qDebug() << data;
        } else {
            qDebug() << "Error:" << reply->errorString();
        }
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        qDebug() << "QNetworkReply::finished";
        reply->deleteLater();
    });

}

// void AIModel::make_request(const QString& prompt) {
//     QNetworkRequest request(QUrl("http://localhost:8080/completion"));
//     request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
//     request.setRawHeader("Authorization", "Bearer no-key");

//     QJsonObject dataObject;
//     dataObject["stream"] = true;
//     dataObject["temperature"] = 0.1;
//     dataObject["n_predict"] = 1800;
//     dataObject["prompt"] = prompt;
//     dataObject["model"] = "gpt-3.5-turbo";

//     QJsonDocument doc(dataObject);
//     QByteArray jsonData = doc.toJson();

//     QNetworkReply *reply = m_network_manager->post(request, jsonData);


//     connect(reply, &QNetworkReply::readyRead, this, [this, reply]() {
//         qDebug() << "QNetworkReply::readyRead";
//         if (reply->error() == QNetworkReply::NoError) {
//             QByteArray data = reply->readAll(); // Get the response data
//             emit answerReceived(data);
//             qDebug() << data;
//         } else {
//             qDebug() << "Error:" << reply->errorString();
//         }
//     });
//     connect(reply, &QNetworkReply::finished, this, [this, reply]() {
//         qDebug() << "QNetworkReply::finished";
//         reply->deleteLater();
//     });

// }
