#ifndef RESOURCES_H
#define RESOURCES_H
#include "codingstyle.h" // include/codingstyle.h

#include <QHash>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QVariant>

class Loader;

class Resources {
public:
    static Resources& getInstance();

    void addResource(const QString& name, const QVariant& value);

    QSharedPointer<Loader> getLoader(const QString& name) const;
    QVariant getResource(const QString& name) const;
    QStringList getResourceUrlsBySuffix(const QString& suffix) const;

private:
    Resources();
    ~Resources() = default;
    Resources(const Resources&) = delete;
    Resources& operator=(const Resources&) = delete;

    void registerDefaultLoaders();
    void registerResourcesFromQrc();
    void resolveLoaderForResource(const QString& name, const QVariant& value);
    static QString normalizeResourcePath(const QString& value);
    static bool resourceExists(const QString& value);
    static QString extractProtocol(const QString& value);
    static QString extractSuffix(const QString& value);

    QHash<QString, QVariant> m_resources;
    QHash<QString, QSharedPointer<Loader>> m_resourceLoaders;
};

#endif // RESOURCES_H
