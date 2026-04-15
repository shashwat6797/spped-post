#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>

class DatabaseManager {
public:
    static DatabaseManager &instance();

    bool initialize();
    QSqlDatabase database() const;
    QString lastError() const;

private:
    DatabaseManager();
    bool createSchema();
    bool ensureFolderIdColumn();

    QSqlDatabase m_database;
    QString m_lastError;
};

#endif // DATABASEMANAGER_H
