#include "core/DatabaseManager.h"

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

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager manager;
    return manager;
}

DatabaseManager::DatabaseManager()
    : m_database(QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("requests")))
{
    m_database.setDatabaseName(databasePath());
}

bool DatabaseManager::initialize()
{
    if (m_database.isOpen()) {
        return true;
    }

    if (!m_database.open()) {
        m_lastError = m_database.lastError().text();
        return false;
    }

    return createSchema();
}

QSqlDatabase DatabaseManager::database() const
{
    return m_database;
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

bool DatabaseManager::createSchema()
{
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

bool DatabaseManager::ensureFolderIdColumn()
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
