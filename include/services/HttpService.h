#ifndef HTTPSERVICE_H
#define HTTPSERVICE_H

#include "models/Request.h"

#include <QNetworkAccessManager>
#include <QObject>

class HttpService : public QObject {
    Q_OBJECT

public:
    explicit HttpService(QObject *parent = nullptr);

    void sendGet(const Request &request);

signals:
    void requestStarted();
    void responseReceived(const QString &body);
    void requestFailed(const QString &message);

private:
    QNetworkAccessManager m_networkAccessManager;
};

#endif // HTTPSERVICE_H
