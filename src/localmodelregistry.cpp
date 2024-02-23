#include <kaho/localmodelregistry.h>
#include <kaho/file_utils.h>
#include <kaho/downloader.h>
#include <kaho/errors.h>
#include <QFileInfo>
#include <QFile>
#include <QDir>

namespace kaho {

    QString extract_filename(const QString& url_) {
        QUrl url(url_);
        QString filePath = url.path();
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();
        return fileName;
    }

    QString url_to_filepath(const QString& url_) {
        auto file_name = extract_filename(url_);
        return QDir(resolve_path(KahoPath::ModelsDir)).filePath(file_name);
    }

    bool LocalModelRegistry::modelExists(const QString& url_) {
        auto full_path = url_to_filepath(url_);
        return QFile::exists( full_path);

    }

    ErrorCode LocalModelRegistry::downloadModel(const QString& url) {
        return downloadFile(url, resolve_path(KahoPath::ModelsDir));
    }

    ErrorCode LocalModelRegistry::ensureModel(const QString& url) {
        if (!modelExists(url)) {
            return downloadModel(url);
        }
        return ErrorCode::OK;
    }

    QString LocalModelRegistry::resolve_filename(const QString& url) {
        return url_to_filepath(url);
    }

} // namespace kaho