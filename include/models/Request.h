#ifndef REQUEST_H
#define REQUEST_H

#include <QString>
#include <QUrl>

struct Request {
    QString method;
    QUrl url;
};

#endif // REQUEST_H
