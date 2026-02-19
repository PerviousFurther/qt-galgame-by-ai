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
#include <QUrl>

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
    return m_initialized;
}

void Loader::markInitialized() {
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
{
}

QVariant BitmapLoader::load(const QVariant& source) {
    const QString path = source.isValid() ? source.toString() : getSourceUrl();
    if (path.isEmpty()) {
        qWarning() << "BitmapLoader has empty source URL";
        return {};
    }
    setSourceUrl(path);

    if (!m_runtimeSuffix.isEmpty()) {
        const QString pathSuffix = QFileInfo(path).suffix().toLower();
        if (!pathSuffix.isEmpty() && pathSuffix != m_runtimeSuffix.toLower()) {
            qWarning() << "BitmapLoader suffix mismatch, expected" << m_runtimeSuffix << "got" << pathSuffix;
        }
    }

    QSharedPointer<Resource> cached = findCachedResource(path);
    if (!cached.isNull()) {
        markInitialized();
        return QVariant::fromValue(cached);
    }

    const QString qrcPath = path.startsWith("qrc:/") ? (":" + path.mid(4)) : path;
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
    const QString path = source.isValid() ? source.toString() : getSourceUrl();
    if (path.isEmpty()) {
        qWarning() << "VideoLoader has empty source URL";
        return {};
    }
    setSourceUrl(path);

    QSharedPointer<Resource> cached = findCachedResource(path);
    if (!cached.isNull()) {
        markInitialized();
        return QVariant::fromValue(cached);
    }

    const bool isQrc = path.startsWith("qrc:/") || path.startsWith(":/");
    if (!isQrc && !QFileInfo::exists(path)) {
        qWarning() << "VideoLoader source file does not exist:" << path;
        return {};
    }

    const QUrl mediaUrl = isQrc
        ? (path.startsWith("qrc:/") ? QUrl(path) : QUrl(QString("qrc") + path))
        : QUrl::fromLocalFile(QFileInfo(path).absoluteFilePath());

    m_mediaPlayer->setSource(mediaUrl);
    auto videoResource = QSharedPointer<ChatSessionResource>::create(path);
    videoResource->setDataSize(1);
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
    const QString path = source.isValid() ? source.toString() : getSourceUrl();
    if (path.isEmpty()) {
        qWarning() << "JsonLoader has empty source URL";
        return {};
    }
    setSourceUrl(path);

    QSharedPointer<Resource> cached = findCachedResource(path);
    if (!cached.isNull()) {
        markInitialized();
        return QVariant::fromValue(cached);
    }

    QFile file(path.startsWith("qrc:/") ? (":" + path.mid(4)) : path);
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
        const QJsonArray loadersArray = doc.object().value("loaders").toArray();
        for (const QJsonValue& value : loadersArray) {
            const QJsonObject obj = value.toObject();
            const QString loaderSource = obj.value("source").toString();
            const int separator = loaderSource.indexOf("://");
            const QString protocol = separator >= 0 ? loaderSource.left(separator) : "file";
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
