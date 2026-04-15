#include "modules/folder/FolderService.h"

#include "common/StringUtils.h"
#include "modules/folder/FolderRepository.h"

FolderService::FolderService(FolderRepository *repository, QObject *parent)
    : QObject(parent)
    , m_repository(repository)
{
}

QVector<Folder> FolderService::allFolders() const
{
    return m_repository->allFolders();
}

int FolderService::createFolder(const QString &name)
{
    const QString normalizedName = StringUtils::normalizedInput(name);
    if (normalizedName.isEmpty()) {
        m_lastError = QStringLiteral("Folder name is required");
        return -1;
    }

    const int id = m_repository->save(normalizedName);
    if (id < 0) {
        m_lastError = m_repository->lastError();
    }

    return id;
}

bool FolderService::renameFolder(int id, const QString &name)
{
    const QString normalizedName = StringUtils::normalizedInput(name);
    if (id < 0 || normalizedName.isEmpty()) {
        m_lastError = QStringLiteral("Folder name is required");
        return false;
    }

    if (!m_repository->rename(id, normalizedName)) {
        m_lastError = m_repository->lastError();
        return false;
    }

    return true;
}

bool FolderService::deleteFolder(int id)
{
    if (id < 0) {
        m_lastError = QStringLiteral("Select a folder to delete");
        return false;
    }

    if (!m_repository->deleteById(id)) {
        m_lastError = m_repository->lastError();
        return false;
    }

    return true;
}

QString FolderService::lastError() const
{
    return m_lastError;
}
