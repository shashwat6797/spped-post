#ifndef REQUESTSERVICE_H
#define REQUESTSERVICE_H

#include "modules/request/SavedRequestModel.h"

#include <QObject>
#include <QVector>

class RequestRepository;

class RequestService : public QObject {
    Q_OBJECT

public:
    explicit RequestService(RequestRepository *repository, QObject *parent = nullptr);

    QVector<SavedRequest> allRequests() const;
    int saveRequest(const SavedRequest &request);
    bool deleteRequest(int id);
    QString lastError() const;

private:
    RequestRepository *m_repository = nullptr;
    QString m_lastError;
};

#endif // REQUESTSERVICE_H
