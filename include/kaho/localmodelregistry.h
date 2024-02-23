#pragma once
#include <QString>
#include <kaho/errors.h>

namespace kaho {

    class LocalModelRegistry {
    public:
        bool modelExists(const QString& url);
        ErrorCode downloadModel(const QString& url);
        ErrorCode ensureModel(const QString& url);
        static QString resolve_filename(const QString& url);
    };

} // namespace kaho