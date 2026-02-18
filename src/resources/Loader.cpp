#include "resources/Loader.h"
#include <QFile>
#include <QThread>
#include <QDebug>

// Loader base class
QString Loader::getProtocol(const QString& url) {
    int pos = url.indexOf(QLatin1String("://"));
    if (pos != -1) {
        return url.left(pos);
    }
    return QStringLiteral("file");  // Default to file protocol
}

QString Loader::getExtension(const QString& url) {
    int pos = url.lastIndexOf(QLatin1Char('.'));
    if (pos != -1 && pos < url.length() - 1) {
        return url.mid(pos + 1).toLower();
    }
    return QString();
}

// FileLoader
bool FileLoader::canLoad(const QString& url) const {
    QString protocol = getProtocol(url);
    return protocol == QLatin1String("file") || !url.contains(QLatin1String("://"));
}

bool FileLoader::readFile(const QString& path, QByteArray& outData) const {
    // Strip file:// protocol if present
    QString filePath = path;
    if (filePath.startsWith(QLatin1String("file://"))) {
        filePath = filePath.mid(7);
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    outData = file.readAll();
    return file.error() == QFile::NoError;
}

// QrcLoader
bool QrcLoader::canLoad(const QString& url) const {
    return getProtocol(url) == QLatin1String("qrc");
}

bool QrcLoader::readQrcResource(const QString& path, QByteArray& outData) const {
    // Convert qrc:// URL to Qt resource path (:/)
    QString resourcePath = path;
    if (resourcePath.startsWith(QLatin1String("qrc://"))) {
        resourcePath = QLatin1String(":/") + resourcePath.mid(6);
    }

    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "QRC loading failed:" << resourcePath;
        return false;
    }

    outData = file.readAll();
    return true;
}

// FileImageLoader
bool FileImageLoader::isImageExtension(const QString& ext) const {
    return ext == QLatin1String("png") || ext == QLatin1String("jpg") || ext == QLatin1String("jpeg") || 
           ext == QLatin1String("bmp") || ext == QLatin1String("gif") || ext == QLatin1String("webp");
}

bool FileImageLoader::canLoad(const QString& url) const {
    if (!FileLoader::canLoad(url)) {
        return false;
    }
    return isImageExtension(getExtension(url));
}

QSharedPointer<Resource> FileImageLoader::loadSync(const QString& url) {
    auto resource = QSharedPointer<TextureResource>::create(url);
    resource->setState(Resource::State::Loading);

    QByteArray data;
    if (readFile(url, data)) {
        // TODO: Actual image decoding (using stb_image or similar)
        // For now, just mark as loaded with dummy dimensions
        resource->setDimensions(100, 100);
        resource->setState(Resource::State::Loaded);
        qDebug() << "Loaded image:" << url << "(" << data.size() << "bytes)";
    } else {
        resource->setState(Resource::State::Failed);
        qWarning() << "Failed to load image:" << url;
    }

    return resource;
}

void FileImageLoader::loadAsync(const QString& url, LoadCallback callback) {
    // Launch async loading in a separate thread
    // Note: We capture url and callback by value to avoid use-after-free
    QThread* thread = QThread::create([url, callback]() {
        // Create a temporary loader for this operation
        FileImageLoader tempLoader;
        auto resource = tempLoader.loadSync(url);
        bool success = resource && resource->isLoaded();
        if (callback) {
            callback(resource, success);
        }
    });
    // Auto-delete the thread object when finished to avoid memory leaks
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

// QrcImageLoader
bool QrcImageLoader::isImageExtension(const QString& ext) const {
    return ext == QLatin1String("png") || ext == QLatin1String("jpg") || ext == QLatin1String("jpeg") || 
           ext == QLatin1String("bmp") || ext == QLatin1String("gif") || ext == QLatin1String("webp");
}

bool QrcImageLoader::canLoad(const QString& url) const {
    if (!QrcLoader::canLoad(url)) {
        return false;
    }
    return isImageExtension(getExtension(url));
}

QSharedPointer<Resource> QrcImageLoader::loadSync(const QString& url) {
    auto resource = QSharedPointer<TextureResource>::create(url);
    resource->setState(Resource::State::Loading);

    QByteArray data;
    if (readQrcResource(url, data)) {
        // TODO: Actual image decoding
        resource->setDimensions(100, 100);
        resource->setState(Resource::State::Loaded);
    } else {
        resource->setState(Resource::State::Failed);
        qWarning() << "Failed to load QRC image:" << url;
    }

    return resource;
}

void QrcImageLoader::loadAsync(const QString& url, LoadCallback callback) {
    // Launch async loading in a separate thread
    // Note: We capture url and callback by value to avoid use-after-free
    QThread* thread = QThread::create([url, callback]() {
        // Create a temporary loader for this operation
        QrcImageLoader tempLoader;
        auto resource = tempLoader.loadSync(url);
        bool success = resource && resource->isLoaded();
        if (callback) {
            callback(resource, success);
        }
    });
    // Auto-delete the thread object when finished to avoid memory leaks
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}
