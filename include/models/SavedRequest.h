#ifndef SAVEDREQUEST_H
#define SAVEDREQUEST_H

#include <QString>

struct SavedRequest {
    int id = -1;
    int folderId = -1;
    QString name;
    QString method;
    QString url;
};

#endif // SAVEDREQUEST_H
