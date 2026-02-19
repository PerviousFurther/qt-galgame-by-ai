#include "codingstyle.h" // include/codingstyle.h
#include "resources/Loader.h"

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
#include <QMutexLocker>
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

QString resolveSourcePath(Loader* loader, const QVariant& source) {
    const QString path = source.isValid() ? source.toString() : loader->getSourceUrl();
    loader->setSourceUrl(path);
    return path;
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
    m_sourceUrl = sourceUrl;
}

const QString& Loader::getSourceUrl() const {
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
    return m_lastResource;
}

QList<QSharedPointer<Loader>> Loader::getGeneratedLoaders() const {
    return m_generatedLoaders;
}

void Loader::setGeneratedLoaders(const QList<QSharedPointer<Loader>>& loaders) {
    m_generatedLoaders = loaders;
}

BitmapLoader::BitmapLoader(const QString& suffix, QObject* parent)
    : ComposedLoader<FileProtocolTag, BitmapSuffixTag>(parent)
    , m_runtimeSuffix(suffix)
    , m_runtimeSuffixLower(suffix.toLower())
{
}

QVariant BitmapLoader::load(const QVariant& source) {
    const QString path = resolveSourcePath(this, source);
    if (path.isEmpty()) {
        qWarning() << "BitmapLoader has empty source URL";
        return {};
    }

    if (!m_runtimeSuffix.isEmpty()) {
        const QString pathSuffix = QFileInfo(path).suffix().toLower();
        if (!pathSuffix.isEmpty() && pathSuffix != m_runtimeSuffixLower) {
            qWarning() << "BitmapLoader suffix mismatch, expected" << m_runtimeSuffix << "got" << pathSuffix;
            return {};
        }
    }

    QSharedPointer<Resource> cached = findCachedResource(path);
    if (!cached.isNull()) {
        markInitialized();
        return QVariant::fromValue(cached);
    }

    const QString qrcPath = normalizeQrcPath(path);
    QImageReader reader(qrcPath);
    QImage image = reader.read();
    if (image.isNull()) {
        qWarning() << "BitmapLoader failed to read image:" << path << reader.errorString();
        return {};
    }

    auto textureResource = QSharedPointer<TextureResource>::create(path);
    textureResource->setDimensions(image.width(), image.height());
    textureResource->setState(Resource::State::Loaded);
    cacheResource(path, textureResource);
    markInitialized();
    qDebug() << "BitmapLoader loaded image:" << path << "size:" << image.size();
    return QVariant::fromValue(QSharedPointer<Resource>(textureResource));
}

VideoLoader::VideoLoader(QObject* parent)
    : ComposedLoader<FileProtocolTag, VideoSuffixTag>(parent)
    , m_mediaPlayer(new QMediaPlayer(this))
{
}

QVariant VideoLoader::load(const QVariant& source) {
    const QString path = resolveSourcePath(this, source);
    if (path.isEmpty()) {
        qWarning() << "VideoLoader has empty source URL";
        return {};
    }

    QSharedPointer<Resource> cached = findCachedResource(path);
    if (!cached.isNull()) {
        markInitialized();
        return QVariant::fromValue(cached);
    }

    if (!path.startsWith("qrc:/") && !path.startsWith(":/") && !QFileInfo::exists(path)) {
        qWarning() << "VideoLoader source file does not exist:" << path;
        return {};
    }

    const QUrl mediaUrl = toMediaUrl(path);

    m_mediaPlayer->setSource(mediaUrl);
    auto videoResource = QSharedPointer<ChatSessionResource>::create(path);
    // TODO: Populate real video resource size when metadata extraction is implemented.
    videoResource->setDataSize(0);
    videoResource->setState(Resource::State::Loaded);
    cacheResource(path, videoResource);
    markInitialized();

    qDebug() << "VideoLoader prepared media source:" << mediaUrl;
    return QVariant::fromValue(QSharedPointer<Resource>(videoResource));
}

JsonLoader::JsonLoader(QObject* parent)
    : ComposedLoader<FileProtocolTag, JsonSuffixTag>(parent)
{
}

QVariant JsonLoader::load(const QVariant& source) {
    const QString path = resolveSourcePath(this, source);
    if (path.isEmpty()) {
        qWarning() << "JsonLoader has empty source URL";
        return {};
    }

    QSharedPointer<Resource> cached = findCachedResource(path);
    if (!cached.isNull()) {
        markInitialized();
        return QVariant::fromValue(cached);
    }

    QFile file(normalizeQrcPath(path));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "JsonLoader failed to open:" << path;
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
        if (!rootObject.contains("loaders") || !rootObject.value("loaders").isArray()) {
            qWarning() << "JsonLoader expected 'loaders' array in root object";
            setGeneratedLoaders({});
            auto sessionResource = QSharedPointer<ChatSessionResource>::create(path);
            sessionResource->setDataSize(static_cast<size_t>(data.size()));
            sessionResource->setState(Resource::State::Loaded);
            cacheResource(path, sessionResource);
            markInitialized();
            return QVariant::fromValue(QSharedPointer<Resource>(sessionResource));
        }
        const QJsonArray loadersArray = rootObject.value("loaders").toArray();
        for (const QJsonValue& value : loadersArray) {
            const QJsonObject obj = value.toObject();
            const QString loaderSource = obj.value("source").toString();
            const int separator = loaderSource.indexOf("://");
            QString protocol;
            if (loaderSource.startsWith("qrc:/") || loaderSource.startsWith(":/")) {
                protocol = "qrc";
            } else if (separator >= 0) {
                protocol = loaderSource.left(separator);
            } else {
                protocol = "file";
            }
            const QString suffix = QFileInfo(loaderSource).suffix().toLower();
            PropertyMap properties;
            properties["source"] = loaderSource;
            QSharedPointer<Loader> loader = Registration::getInstance().createLoader(protocol, suffix, properties);
            if (!loader.isNull()) {
                generatedLoaders.append(loader);
            }
        }
    }
    setGeneratedLoaders(generatedLoaders);

    auto sessionResource = QSharedPointer<ChatSessionResource>::create(path);
    sessionResource->setDataSize(static_cast<size_t>(data.size()));
    sessionResource->setState(Resource::State::Loaded);
    cacheResource(path, sessionResource);
    markInitialized();
    return QVariant::fromValue(QSharedPointer<Resource>(sessionResource));
}
