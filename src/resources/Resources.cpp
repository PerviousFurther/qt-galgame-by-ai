#include "codingstyle.h" // include/codingstyle.h
#include "resources/Resources.h"

#include "factory/Registration.h"
#include "resources/Loader.h"

#include <QDebug>
#include <QMutexLocker>
#include <QStringList>

Resources::Resources() {
    registerDefaultLoaders();
}

Resources& Resources::getInstance() {
    static Resources instance;
    return instance;
}

void Resources::registerDefaultLoaders() {
    Registration& registration = Registration::getInstance();
    const QStringList bitmapSuffixes = {"bmp", "png", "jpg", "jpeg", "webp"};
    for (const QString& suffix : bitmapSuffixes) {
        registration.registerLoader({"file", suffix, "Native", "BitmapLoader"});
        registration.registerLoader({"qrc", suffix, "Native", "BitmapLoader"});
    }
    registration.registerLoader({"file", "mp4", "Native", "VideoLoader"});
    registration.registerLoader({"qrc", "mp4", "Native", "VideoLoader"});
    registration.registerLoader({"file", "json", "Native", "JsonLoader"});
    registration.registerLoader({"qrc", "json", "Native", "JsonLoader"});
}

void Resources::addResource(const QString& name, const QVariant& value) {
    QMutexLocker locker(&m_mutex);
    m_resources[name] = value;
    resolveLoaderForResource(name, value);
}

QVariant Resources::getResource(const QString& name) const {
    QMutexLocker locker(&m_mutex);
    return m_resources.value(name);
}

QSharedPointer<Loader> Resources::getLoader(const QString& name) const {
    QMutexLocker locker(&m_mutex);
    return m_resourceLoaders.value(name);
}

QVariant Resources::load(const QString& name) const {
    QSharedPointer<Loader> loader;
    {
        QMutexLocker locker(&m_mutex);
        if (!m_resources.contains(name)) {
            qWarning() << "Resource not found:" << name;
            return {};
        }
        loader = m_resourceLoaders.value(name);
    }

    if (loader.isNull()) {
        qWarning() << "Loader not found for resource:" << name;
        return {};
    }
    return loader->load({});
}

void Resources::resolveLoaderForResource(const QString& name, const QVariant& value) {
    if (!value.canConvert<QString>()) {
        m_resourceLoaders.remove(name);
        return;
    }

    const QString source = value.toString();
    const QString protocol = extractProtocol(source);
    const QString suffix = extractSuffix(source);

    PropertyMap properties;
    properties["source"] = source;

    QSharedPointer<Loader> loader = Registration::getInstance().createLoader(protocol, suffix, properties);
    if (loader.isNull()) {
        qWarning() << "Unable to resolve loader for resource:" << name << source;
        m_resourceLoaders.remove(name);
        return;
    }
    loader->setSourceUrl(source);
    m_resourceLoaders[name] = loader;
}

QString Resources::extractProtocol(const QString& value) {
    if (value.startsWith("qrc:/") || value.startsWith(":/")) {
        return "qrc";
    }
    const int separatorIndex = value.indexOf("://");
    if (separatorIndex >= 0) {
        return value.left(separatorIndex);
    }
    return "file";
}

QString Resources::extractSuffix(const QString& value) {
    const int dotIndex = value.lastIndexOf('.');
    if (dotIndex < 0 || dotIndex + 1 >= value.size()) {
        return {};
    }
    return value.mid(dotIndex + 1).toLower();
}
