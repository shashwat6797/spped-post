#include "modules/request/HttpService.h"

#include <QNetworkReply>
#include <QNetworkRequest>

HttpService::HttpService(QObject *parent)
    : QObject(parent)
    , m_networkAccessManager(this)
{
}

void HttpService::sendGet(const Request &request)
{
    emit requestStarted();

    QNetworkRequest networkRequest(request.url);
    auto *reply = m_networkAccessManager.get(networkRequest);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const QByteArray responseBody = reply->readAll();

        if (reply->error() != QNetworkReply::NoError) {
            emit requestFailed(reply->errorString());
        } else {
            emit responseReceived(QString::fromUtf8(responseBody));
        }

        reply->deleteLater();
    });
}
