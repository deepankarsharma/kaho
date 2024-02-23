#pragma once

#include <QString>
#include <QFile>
#include <QTextStream>
#include <kaho/errors.h>

namespace kaho {
    int to_file(const QString& path, const QByteArray& byteArray);
    QString from_file(const QString& path);

// returns true if the directory was created successfully
    ErrorCode ensure_dir(const QString& path);

    enum class KahoPath {
        ModelsDir
    };

    QString resolve_path(KahoPath path);
}
