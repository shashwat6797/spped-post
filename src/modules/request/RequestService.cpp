#include "modules/request/RequestService.h"

#include "common/StringUtils.h"
#include "modules/request/RequestRepository.h"

RequestService::RequestService(RequestRepository *repository, QObject *parent)
    : QObject(parent)
    , m_repository(repository)
{
}

QVector<SavedRequest> RequestService::allRequests() const
{
    return m_repository->allRequests();
}

int RequestService::saveRequest(const SavedRequest &request)
{
    SavedRequest normalizedRequest = request;
    normalizedRequest.name = StringUtils::normalizedInput(request.name);
    normalizedRequest.url = StringUtils::normalizedInput(request.url);

    if (normalizedRequest.name.isEmpty() || normalizedRequest.url.isEmpty()) {
        m_lastError = QStringLiteral("Request name and URL are required");
        return -1;
    }

    const int savedId = normalizedRequest.id >= 0
        ? (m_repository->update(normalizedRequest) ? normalizedRequest.id : -1)
        : m_repository->save(normalizedRequest);

    if (savedId < 0) {
        m_lastError = m_repository->lastError();
    }

    return savedId;
}

bool RequestService::deleteRequest(int id)
{
    if (id < 0) {
        m_lastError = QStringLiteral("Select a request to delete");
        return false;
    }

    if (!m_repository->deleteById(id)) {
        m_lastError = m_repository->lastError();
        return false;
    }

    return true;
}

QString RequestService::lastError() const
{
    return m_lastError;
}
