#ifndef FOLDERREPOSITORY_H
#define FOLDERREPOSITORY_H

#include "modules/folder/FolderModel.h"

#include <QObject>
#include <QVector>

class FolderRepository : public QObject {
    Q_OBJECT

public:
    explicit FolderRepository(QObject *parent = nullptr);

    QVector<Folder> allFolders() const;
    int save(const QString &name);
    bool rename(int id, const QString &name);
    bool deleteById(int id);
    QString lastError() const;

private:
    mutable QString m_lastError;
};

#endif // FOLDERREPOSITORY_H
