#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <QString>

struct SavedRequest;

namespace StringUtils {
QString normalizedInput(const QString &value);
QString requestDisplayText(const SavedRequest &request);
}

#endif // STRINGUTILS_H
