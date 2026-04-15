#include "modules/folder/FolderRepository.h"

#include "core/DatabaseManager.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

FolderRepository::FolderRepository(QObject *parent)
    : QObject(parent)
{
}

QVector<Folder> FolderRepository::allFolders() const
{
    QVector<Folder> folders;
    QSqlQuery query(DatabaseManager::instance().database());

    if (!query.exec(QStringLiteral("SELECT id, name FROM folders ORDER BY name ASC"))) {
        m_lastError = query.lastError().text();
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

int FolderRepository::save(const QString &name)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral("INSERT INTO folders (name) VALUES (:name)"));
    query.bindValue(QStringLiteral(":name"), name);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return -1;
    }

    return query.lastInsertId().toInt();
}

bool FolderRepository::rename(int id, const QString &name)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare(QStringLiteral("UPDATE folders SET name = :name WHERE id = :id"));
    query.bindValue(QStringLiteral(":name"), name);
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    return true;
}

bool FolderRepository::deleteById(int id)
{
    QSqlDatabase database = DatabaseManager::instance().database();

    QSqlQuery updateRequests(database);
    updateRequests.prepare(QStringLiteral("UPDATE requests SET folder_id = NULL WHERE folder_id = :id"));
    updateRequests.bindValue(QStringLiteral(":id"), id);

    if (!updateRequests.exec()) {
        m_lastError = updateRequests.lastError().text();
        return false;
    }

    QSqlQuery deleteFolder(database);
    deleteFolder.prepare(QStringLiteral("DELETE FROM folders WHERE id = :id"));
    deleteFolder.bindValue(QStringLiteral(":id"), id);

    if (!deleteFolder.exec()) {
        m_lastError = deleteFolder.lastError().text();
        return false;
    }

    return true;
}

QString FolderRepository::lastError() const
{
    return m_lastError;
}
