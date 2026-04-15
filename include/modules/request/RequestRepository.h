#ifndef REQUESTREPOSITORY_H
#define REQUESTREPOSITORY_H

#include "modules/request/SavedRequestModel.h"

#include <QObject>
#include <QVector>

class RequestRepository : public QObject {
    Q_OBJECT

public:
    explicit RequestRepository(QObject *parent = nullptr);

    QVector<SavedRequest> allRequests() const;
    int save(const SavedRequest &request);
    bool update(const SavedRequest &request);
    bool deleteById(int id);
    QString lastError() const;

private:
    mutable QString m_lastError;
};

#endif // REQUESTREPOSITORY_H
