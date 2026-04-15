#include "common/StringUtils.h"

#include "modules/request/SavedRequestModel.h"

namespace StringUtils {

QString normalizedInput(const QString &value)
{
    return value.trimmed();
}

QString requestDisplayText(const SavedRequest &request)
{
    return QStringLiteral("%1  %2").arg(request.method, request.name);
}

}
