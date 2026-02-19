#include "codingstyle.h" // include/codingstyle.h
#include "resources/Loader.h"

#include "core/Execution.h"
#include "factory/Registration.h"
#include "resources/Resource.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMediaPlayer>
#include <QMetaObject>
#include <QMutexLocker>
#include <QPointer>
#include <QUrl>

namespace {
QString normalizeQrcPath(const QString& path) {
    if (path.startsWith("qrc:/")) {
        return ":" + path.mid(4);
    }
    return path;
}

QUrl toMediaUrl(const QString& path) {
    if (path.startsWith("qrc:/")) {
        return QUrl(path);
    }
    if (path.startsWith(":/")) {
        return QUrl("qrc" + path);
    }
    return QUrl::fromLocalFile(QFileInfo(path).absoluteFilePath());
}

QString resolveProtocol(const QString& source) {
    if (source.startsWith("qrc:/") || source.startsWith(":/")) {
        return "qrc";
    }
    const int pos = source.indexOf("://");
    if (pos >= 0) {
        return source.left(pos);
    }
    return "file";
}
}

Loader::Loader(const QString& protocol, const QString& suffix, QObject* parent)
    : QObject(parent)
    , m_protocol(protocol)
    , m_suffix(suffix)
    , m_sourceUrl()
    , m_initialized(false)
{
}

const QString& Loader::getProtocol() const {
    return m_protocol;
}

const QString& Loader::getSuffix() const {
    return m_suffix;
}

void Loader::setSourceUrl(const QString& sourceUrl) {
    QMutexLocker locker(&m_resourceMutex);
    m_sourceUrl = sourceUrl;
}

QString Loader::getSourceUrl() const {
    QMutexLocker locker(&m_resourceMutex);
    return m_sourceUrl;
}

bool Loader::isInitialized() const {
    QMutexLocker locker(&m_initializedMutex);
    return m_initialized;
}

void Loader::markInitialized() {
    QMutexLocker locker(&m_initializedMutex);
    m_initialized = true;
}

Loader& Loader::load(const QVariant& source, bool async) {
    const QString sourceUrl = source.isValid() ? source.toString() : getSourceUrl();
    if (sourceUrl.isEmpty()) {
        emit loadFailed("Loader source URL is empty for " + getProtocol() + ":" + getSuffix());
        return *this;
    }

    auto completeLoad = [sourceUrl](Loader* loader) {
        if (!loader) {
            return;
        }

        QSharedPointer<Resource> resource = loader->loadImpl(sourceUrl);
        if (resource.isNull()) {
            const QString errorMessage = "Loader failed to parse resource: " + sourceUrl;
            QPointer<Loader> guarded(loader);
            QMetaObject::invokeMethod(loader, [guarded, errorMessage]() {
                if (guarded) {
                    emit guarded->loadFailed(errorMessage);
                }
            }, Qt::QueuedConnection);
            return;
        }

        {
            QMutexLocker locker(&loader->m_resourceMutex);
            loader->cacheResource(sourceUrl, resource);
        }
        loader->markInitialized();
        QPointer<Loader> guarded(loader);
        QMetaObject::invokeMethod(loader, [guarded]() {
            if (guarded) {
                emit guarded->loadFinished(guarded.data());
            }
        }, Qt::QueuedConnection);
    };

    if (async) {
        QPointer<Loader> self(this);
        Execution::getInstance().dispatchAsyncTask([self, completeLoad]() {
            if (!self) {
                return;
            }
            completeLoad(self.data());
        });
    } else {
        completeLoad(this);
    }
    return *this;
}

Loader& Loader::unload(bool async) {
    auto completeUnload = [](Loader* loader) {
        if (!loader) {
            return;
        }

        loader->unloadImpl();
        {
            QMutexLocker locker(&loader->m_resourceMutex);
            loader->m_resourceCache.clear();
            loader->m_lastResource.clear();
        }
        {
            QMutexLocker initializedLocker(&loader->m_initializedMutex);
            loader->m_initialized = false;
        }
        QPointer<Loader> guarded(loader);
        QMetaObject::invokeMethod(loader, [guarded]() {
            if (guarded) {
                emit guarded->unloadFinished(guarded.data());
            }
        }, Qt::QueuedConnection);
    };

    if (async) {
        QPointer<Loader> self(this);
        Execution::getInstance().dispatchAsyncTask([self, completeUnload]() {
            if (!self) {
                return;
            }
            completeUnload(self.data());
        });
    } else {
        completeUnload(this);
    }
    return *this;
}

QObject* Loader::get() const {
    QMutexLocker locker(&m_resourceMutex);
    if (m_lastResource.isNull()) {
        return nullptr;
    }
    return m_lastResource->get();
}

void Loader::unloadImpl() {
}

void Loader::cacheResource(const QString& sourceUrl, const QSharedPointer<Resource>& resource) {
    if (sourceUrl.isEmpty() || resource.isNull()) {
        return;
    }
    m_resourceCache[sourceUrl] = resource;
    m_lastResource = resource;
}

QSharedPointer<Resource> Loader::findCachedResource(const QString& sourceUrl) const {
    return m_resourceCache.value(sourceUrl);
}

QSharedPointer<Resource> Loader::getCachedResource() const {
    QMutexLocker locker(&m_resourceMutex);
    return m_lastResource;
}

QList<QSharedPointer<Loader>> Loader::getGeneratedLoaders() const {
    QMutexLocker locker(&m_resourceMutex);
    return m_generatedLoaders;
}

void Loader::setGeneratedLoaders(const QList<QSharedPointer<Loader>>& loaders) {
    QMutexLocker locker(&m_resourceMutex);
    m_generatedLoaders = loaders;
}

BitmapLoader::BitmapLoader(const QString& suffix, QObject* parent)
    : ComposedLoader<FileProtocolTag, BitmapSuffixTag>(parent)
    , m_runtimeSuffix(suffix)
    , m_runtimeSuffixLower(suffix.toLower())
{
}

QSharedPointer<Resource> BitmapLoader::loadImpl(const QString& sourceUrl) {
    if (!m_runtimeSuffix.isEmpty()) {
        const QString pathSuffix = QFileInfo(sourceUrl).suffix().toLower();
        if (!pathSuffix.isEmpty() && pathSuffix != m_runtimeSuffixLower) {
            qWarning() << "BitmapLoader suffix mismatch, expected" << m_runtimeSuffix << "got" << pathSuffix;
            return {};
        }
    }

    QSharedPointer<Resource> cached = findCachedResource(sourceUrl);
    if (!cached.isNull()) {
        return cached;
    }

    const QString qrcPath = normalizeQrcPath(sourceUrl);
    QImageReader reader(qrcPath);
    QImage image = reader.read();
    if (image.isNull()) {
        qWarning() << "BitmapLoader failed to read image:" << sourceUrl << reader.errorString();
        return {};
    }

    auto textureResource = QSharedPointer<TextureResource>::create(sourceUrl);
    textureResource->setDimensions(image.width(), image.height());
    textureResource->setState(Resource::State::Loaded);
    // Payload object is intentionally parentless and owned by Resource's shared pointer.
    QObject* payload = new QObject();
    payload->setProperty("image", image);
    textureResource->set(payload);
    qDebug() << "BitmapLoader loaded image:" << sourceUrl << "size:" << image.size();
    return textureResource;
}

VideoLoader::VideoLoader(QObject* parent)
    : ComposedLoader<FileProtocolTag, VideoSuffixTag>(parent)
    , m_mediaPlayer(new QMediaPlayer(this))
{
}

QSharedPointer<Resource> VideoLoader::loadImpl(const QString& sourceUrl) {
    QSharedPointer<Resource> cached = findCachedResource(sourceUrl);
    if (!cached.isNull()) {
        return cached;
    }

    if (!sourceUrl.startsWith("qrc:/") && !sourceUrl.startsWith(":/") && !QFileInfo::exists(sourceUrl)) {
        qWarning() << "VideoLoader source file does not exist:" << sourceUrl;
        return {};
    }

    const QUrl mediaUrl = toMediaUrl(sourceUrl);
    m_mediaPlayer->setSource(mediaUrl);

    auto videoResource = QSharedPointer<ChatSessionResource>::create(sourceUrl);
    videoResource->setDataSize(0);
    videoResource->setState(Resource::State::Loaded);
    // Payload object is intentionally parentless and owned by Resource's shared pointer.
    QObject* payload = new QObject();
    payload->setProperty("url", mediaUrl);
    videoResource->set(payload);
    qDebug() << "VideoLoader prepared media source:" << mediaUrl;
    return videoResource;
}

JsonLoader::JsonLoader(QObject* parent)
    : ComposedLoader<FileProtocolTag, JsonSuffixTag>(parent)
{
}

QSharedPointer<Resource> JsonLoader::loadImpl(const QString& sourceUrl) {
    QSharedPointer<Resource> cached = findCachedResource(sourceUrl);
    if (!cached.isNull()) {
        return cached;
    }

    QFile file(normalizeQrcPath(sourceUrl));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "JsonLoader failed to open:" << sourceUrl;
        return {};
    }

    const QByteArray data = file.readAll();
    file.close();
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JsonLoader parse error:" << parseError.errorString();
        return {};
    }

    QList<QSharedPointer<Loader>> generatedLoaders;
    if (doc.isObject()) {
        const QJsonObject rootObject = doc.object();
        const QJsonArray loadersArray = rootObject.value("loaders").toArray();
        for (const QJsonValue& value : loadersArray) {
            const QJsonObject obj = value.toObject();
            const QString loaderSource = obj.value("source").toString();
            const QString protocol = resolveProtocol(loaderSource);
            const QString suffix = QFileInfo(loaderSource).suffix().toLower();
            PropertyMap properties;
            properties["source"] = loaderSource;
            properties["protocol"] = protocol;
            properties["suffix"] = suffix;
            QSharedPointer<QObject> object = Registration::getInstance().create("Native", properties);
            if (object.isNull()) {
                continue;
            }
            QSharedPointer<Loader> loader = object.dynamicCast<Loader>();
            if (!loader.isNull()) {
                generatedLoaders.append(loader);
            }
        }
    }
    setGeneratedLoaders(generatedLoaders);

    auto sessionResource = QSharedPointer<ChatSessionResource>::create(sourceUrl);
    sessionResource->setDataSize(static_cast<size_t>(data.size()));
    sessionResource->setState(Resource::State::Loaded);
    // Payload object is intentionally parentless and owned by Resource's shared pointer.
    QObject* payload = new QObject();
    payload->setProperty("json", doc.toJson(QJsonDocument::Compact));
    sessionResource->set(payload);
    return sessionResource;
}
