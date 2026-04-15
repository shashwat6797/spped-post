#ifndef REQUESTMODEL_H
#define REQUESTMODEL_H

#include <QString>
#include <QUrl>

struct Request {
    QString method;
    QUrl url;
};

#endif // REQUESTMODEL_H
