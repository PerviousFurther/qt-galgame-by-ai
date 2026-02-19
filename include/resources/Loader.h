#ifndef LOADER_H
#define LOADER_H
#include "codingstyle.h" // include/codingstyle.h

#include <QObject>
#include <QHash>
#include <QList>
#include <QMutex>
#include <QSharedPointer>
#include <QString>
#include <QVariant>

class QMediaPlayer;
class Resource;

struct FileProtocolTag {
    static QString value() { return "file"; }
};

struct QrcProtocolTag {
    static QString value() { return "qrc"; }
};

struct BitmapSuffixTag {
    static QString value() { return "bmp"; }
};

struct VideoSuffixTag {
    static QString value() { return "mp4"; }
};

struct JsonSuffixTag {
    static QString value() { return "json"; }
};

class Loader : public QObject {
    Q_OBJECT
public:
    explicit Loader(const QString& protocol, const QString& suffix, QObject* parent = nullptr);
    ~Loader() override = default;

    const QString& getProtocol() const;
    const QString& getSuffix() const;
    void setSourceUrl(const QString& sourceUrl);
    QString getSourceUrl() const;
    bool isInitialized() const;

    Loader& load(const QVariant& source = {}, bool async = true);
    Loader& unload(bool async = true);
    QObject* get() const;

    virtual QSharedPointer<Resource> loadImpl(const QString& sourceUrl) = 0;
    virtual void unloadImpl();

    QSharedPointer<Resource> getCachedResource() const;
    QList<QSharedPointer<Loader>> getGeneratedLoaders() const;

signals:
    void loadFinished(Loader* loader);
    void unloadFinished(Loader* loader);
    void loadFailed(const QString& error);

protected:
    void markInitialized();
    void cacheResource(const QString& sourceUrl, const QSharedPointer<Resource>& resource);
    QSharedPointer<Resource> findCachedResource(const QString& sourceUrl) const;
    void setGeneratedLoaders(const QList<QSharedPointer<Loader>>& loaders);

private:
    QString m_protocol;
    QString m_suffix;
    QString m_sourceUrl;
    bool m_initialized;
    mutable QMutex m_initializedMutex{QMutex::NonRecursive};
    mutable QMutex m_resourceMutex{QMutex::NonRecursive};
    QHash<QString, QSharedPointer<Resource>> m_resourceCache;
    QSharedPointer<Resource> m_lastResource;
    QList<QSharedPointer<Loader>> m_generatedLoaders;
};

template <typename ProtocolTag, typename SuffixTag>
class ComposedLoader : public Loader {
public:
    explicit ComposedLoader(QObject* parent = nullptr)
        : Loader(ProtocolTag::value(), SuffixTag::value(), parent)
    {
    }
};

class BitmapLoader : public ComposedLoader<FileProtocolTag, BitmapSuffixTag> {
    Q_OBJECT
public:
    // Runtime suffix is used for validation while template suffix identifies composed semantic category.
    explicit BitmapLoader(const QString& suffix = "bmp", QObject* parent = nullptr);
    QSharedPointer<Resource> loadImpl(const QString& sourceUrl) override;

private:
    QString m_runtimeSuffix;
    QString m_runtimeSuffixLower;
};

class VideoLoader : public ComposedLoader<FileProtocolTag, VideoSuffixTag> {
    Q_OBJECT
public:
    explicit VideoLoader(QObject* parent = nullptr);
    QSharedPointer<Resource> loadImpl(const QString& sourceUrl) override;

private:
    QMediaPlayer* m_mediaPlayer;
};

class JsonLoader : public ComposedLoader<FileProtocolTag, JsonSuffixTag> {
    Q_OBJECT
public:
    explicit JsonLoader(QObject* parent = nullptr);
    QSharedPointer<Resource> loadImpl(const QString& sourceUrl) override;
};

Q_DECLARE_METATYPE(QSharedPointer<Loader>)

#endif // LOADER_H
