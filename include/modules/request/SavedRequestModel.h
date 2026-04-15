#ifndef SAVEDREQUESTMODEL_H
#define SAVEDREQUESTMODEL_H

#include <QString>

struct SavedRequest {
    int id = -1;
    int folderId = -1;
    QString name;
    QString method;
    QString url;
};

#endif // SAVEDREQUESTMODEL_H
