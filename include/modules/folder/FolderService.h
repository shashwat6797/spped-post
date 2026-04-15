#ifndef FOLDERSERVICE_H
#define FOLDERSERVICE_H

#include "modules/folder/FolderModel.h"

#include <QObject>
#include <QVector>

class FolderRepository;

class FolderService : public QObject {
    Q_OBJECT

public:
    explicit FolderService(FolderRepository *repository, QObject *parent = nullptr);

    QVector<Folder> allFolders() const;
    int createFolder(const QString &name);
    bool renameFolder(int id, const QString &name);
    bool deleteFolder(int id);
    QString lastError() const;

private:
    FolderRepository *m_repository = nullptr;
    QString m_lastError;
};

#endif // FOLDERSERVICE_H
