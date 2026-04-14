#include "services/RequestRepository.h"

#include <QDir>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>

namespace {
QString databasePath()
{
    QString dataDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (dataDirectory.isEmpty()) {
        dataDirectory = QDir::homePath() + QStringLiteral("/.postman-app");
    }

    QDir().mkpath(dataDirectory);
    return QDir(dataDirectory).filePath(QStringLiteral("requests.sqlite"));
}
}

RequestRepository::RequestRepository(QObject *parent)
    : QObject(parent)
    , m_database(QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("requests")))
{
    m_database.setDatabaseName(databasePath());
}

bool RequestRepository::initialize()
{
    if (!m_database.open()) {
        m_lastError = m_database.lastError().text();
        return false;
    }

    QSqlQuery query(m_database);
    const bool foldersOk = query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS folders ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL"
        ")"
    ));

    if (!foldersOk) {
        m_lastError = query.lastError().text();
        return false;
    }

    const bool requestsOk = query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS requests ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "method TEXT NOT NULL,"
        "url TEXT NOT NULL,"
        "folder_id INTEGER NULL,"
        "FOREIGN KEY(folder_id) REFERENCES folders(id) ON DELETE SET NULL"
        ")"
    ));

    if (!requestsOk) {
        m_lastError = query.lastError().text();
        return false;
    }

    return ensureFolderIdColumn();
}

QVector<Folder> RequestRepository::allFolders() const
{
    QVector<Folder> folders;
    QSqlQuery query(m_database);

    if (!query.exec(QStringLiteral("SELECT id, name FROM folders ORDER BY name ASC"))) {
        return folders;
    }

    while (query.next()) {
        folders.push_back({
            query.value(0).toInt(),
            query.value(1).toString()
        });
    }

    return folders;
}

QVector<SavedRequest> RequestRepository::allRequests() const
{
    QVector<SavedRequest> requests;
    QSqlQuery query(m_database);

    if (!query.exec(QStringLiteral(
        "SELECT id, folder_id, name, method, url "
        "FROM requests ORDER BY folder_id IS NOT NULL, folder_id, name ASC"
    ))) {
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

int RequestRepository::saveFolder(const QString &name)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("INSERT INTO folders (name) VALUES (:name)"));
    query.bindValue(QStringLiteral(":name"), name);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return -1;
    }

    return query.lastInsertId().toInt();
}

bool RequestRepository::renameFolder(int id, const QString &name)
{
    QSqlQuery query(m_database);
    query.prepare(QStringLiteral("UPDATE folders SET name = :name WHERE id = :id"));
    query.bindValue(QStringLiteral(":name"), name);
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    return true;
}

bool RequestRepository::deleteFolder(int id)
{
    QSqlQuery updateRequests(m_database);
    updateRequests.prepare(QStringLiteral("UPDATE requests SET folder_id = NULL WHERE folder_id = :id"));
    updateRequests.bindValue(QStringLiteral(":id"), id);

    if (!updateRequests.exec()) {
        m_lastError = updateRequests.lastError().text();
        return false;
    }

    QSqlQuery deleteFolder(m_database);
    deleteFolder.prepare(QStringLiteral("DELETE FROM folders WHERE id = :id"));
    deleteFolder.bindValue(QStringLiteral(":id"), id);

    if (!deleteFolder.exec()) {
        m_lastError = deleteFolder.lastError().text();
        return false;
    }

    return true;
}

int RequestRepository::save(const SavedRequest &request)
{
    QSqlQuery query(m_database);
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
    QSqlQuery query(m_database);
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
    QSqlQuery query(m_database);
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

bool RequestRepository::ensureFolderIdColumn()
{
    QSqlQuery columns(m_database);
    if (!columns.exec(QStringLiteral("PRAGMA table_info(requests)"))) {
        m_lastError = columns.lastError().text();
        return false;
    }

    while (columns.next()) {
        if (columns.value(1).toString() == QStringLiteral("folder_id")) {
            return true;
        }
    }

    QSqlQuery alter(m_database);
    if (!alter.exec(QStringLiteral("ALTER TABLE requests ADD COLUMN folder_id INTEGER NULL"))) {
        m_lastError = alter.lastError().text();
        return false;
    }

    return true;
}
