#ifndef RESOURCES_H
#define RESOURCES_H

#include <QHash>
#include <QMutex>
#include <QSharedPointer>
#include <QString>
#include <QVariant>

class Loader;

class Resources {
public:
    static Resources& getInstance();

    void addResource(const QString& name, const QVariant& value);
    QVariant getResource(const QString& name) const;

    QSharedPointer<Loader> getLoader(const QString& name) const;
    QVariant load(const QString& name) const;

private:
    Resources();
    ~Resources() = default;
    Resources(const Resources&) = delete;
    Resources& operator=(const Resources&) = delete;

    void registerDefaultLoaders();
    void resolveLoaderForResource(const QString& name, const QVariant& value);
    static QString extractProtocol(const QString& value);
    static QString extractSuffix(const QString& value);

    mutable QMutex m_mutex;
    QHash<QString, QVariant> m_resources;
    QHash<QString, QSharedPointer<Loader>> m_resourceLoaders;
};

#endif // RESOURCES_H
