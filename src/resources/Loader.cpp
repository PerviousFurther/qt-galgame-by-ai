#include "codingstyle.h" // include/codingstyle.h
#include "resources/Loader.h"

#include <QDebug>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QMediaPlayer>
#include <QUrl>

Loader::Loader(const QString& protocol, const QString& suffix, QObject* parent)
    : QObject(parent)
    , m_protocol(protocol)
    , m_suffix(suffix)
{
}

const QString& Loader::getProtocol() const {
    return m_protocol;
}

const QString& Loader::getSuffix() const {
    return m_suffix;
}

BitmapLoader::BitmapLoader(const QString& suffix, QObject* parent)
    : Loader("file", suffix, parent)
{
}

QVariant BitmapLoader::load(const QVariant& source) {
    const QString path = source.toString();
    QImageReader reader(path);
    QImage image = reader.read();
    if (image.isNull()) {
        qWarning() << "BitmapLoader failed to read image:" << path << reader.errorString();
        return {};
    }
    qDebug() << "BitmapLoader loaded image:" << path << "size:" << image.size();
    return image;
}

VideoLoader::VideoLoader(QObject* parent)
    : Loader("file", "mp4", parent)
    , m_mediaPlayer(new QMediaPlayer(this))
{
}

QVariant VideoLoader::load(const QVariant& source) {
    const QString path = source.toString();
    const QUrl mediaUrl = QUrl::fromLocalFile(QFileInfo(path).absoluteFilePath());
    if (!QFileInfo::exists(path)) {
        qWarning() << "VideoLoader source file does not exist:" << path;
        return {};
    }
    m_mediaPlayer->setSource(mediaUrl);
    qDebug() << "VideoLoader prepared media source:" << mediaUrl;
    return mediaUrl;
}
