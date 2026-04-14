#ifndef REQUESTREPOSITORY_H
#define REQUESTREPOSITORY_H

#include "models/Folder.h"
#include "models/SavedRequest.h"

#include <QObject>
#include <QSqlDatabase>
#include <QVector>

class RequestRepository : public QObject {
    Q_OBJECT

public:
    explicit RequestRepository(QObject *parent = nullptr);

    bool initialize();
    QVector<Folder> allFolders() const;
    QVector<SavedRequest> allRequests() const;
    int saveFolder(const QString &name);
    bool renameFolder(int id, const QString &name);
    bool deleteFolder(int id);
    int save(const SavedRequest &request);
    bool update(const SavedRequest &request);
    bool deleteById(int id);
    QString lastError() const;

private:
    bool ensureFolderIdColumn();

    QSqlDatabase m_database;
    QString m_lastError;
};

#endif // REQUESTREPOSITORY_H
