#include "modules/request/RequestRepository.h"

#include "core/DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

RequestRepository::RequestRepository(QObject *parent)
    : QObject(parent)
{
}

QVector<SavedRequest> RequestRepository::allRequests() const
{
    QVector<SavedRequest> requests;
    QSqlQuery query(DatabaseManager::instance().database());

    if (!query.exec(QStringLiteral(
        "SELECT id, folder_id, name, method, url "
        "FROM requests ORDER BY folder_id IS NOT NULL, folder_id, name ASC"
    ))) {
        m_lastError = query.lastError().text();
        return requests;
    }

    while (query.next()) {
        requests.push_back({
            query.value(0).toInt(),
            query.value(1).isNull() ? -1 : query.value(1).toInt(),
            query.value(2).toString(),
            query.value(3).toString(),
            query.value(4).toString()
        });
    }

    return requests;
}

int RequestRepository::save(const SavedRequest &request)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral(
        "INSERT INTO requests (folder_id, name, method, url) "
        "VALUES (:folder_id, :name, :method, :url)"
    ));
    query.bindValue(
        QStringLiteral(":folder_id"),
        request.folderId >= 0 ? QVariant(request.folderId) : QVariant()
    );
    query.bindValue(QStringLiteral(":name"), request.name);
    query.bindValue(QStringLiteral(":method"), request.method);
    query.bindValue(QStringLiteral(":url"), request.url);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return -1;
    }

    return query.lastInsertId().toInt();
}

bool RequestRepository::update(const SavedRequest &request)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral(
        "UPDATE requests SET folder_id = :folder_id, name = :name, method = :method, url = :url "
        "WHERE id = :id"
    ));
    query.bindValue(
        QStringLiteral(":folder_id"),
        request.folderId >= 0 ? QVariant(request.folderId) : QVariant()
    );
    query.bindValue(QStringLiteral(":name"), request.name);
    query.bindValue(QStringLiteral(":method"), request.method);
    query.bindValue(QStringLiteral(":url"), request.url);
    query.bindValue(QStringLiteral(":id"), request.id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    return true;
}

bool RequestRepository::deleteById(int id)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral("DELETE FROM requests WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    return true;
}

QString RequestRepository::lastError() const
{
    return m_lastError;
}
