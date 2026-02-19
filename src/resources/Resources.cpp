#include "codingstyle.h" // include/codingstyle.h
#include "resources/Resources.h"

#include "factory/Registration.h"
#include "resources/Loader.h"

#include <QDebug>

Resources::Resources() {
    registerDefaultLoaders();
}

Resources& Resources::getInstance() {
    static Resources instance;
    return instance;
}

void Resources::registerDefaultLoaders() {
    // Unified creation flow: loader selection comes from PropertyMap (protocol/suffix/type/source).
}

void Resources::addResource(const QString& name, const QVariant& value) {
    m_resources[name] = value;
    resolveLoaderForResource(name, value);
}

QSharedPointer<Loader> Resources::getLoader(const QString& name) const {
    return m_resourceLoaders.value(name);
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

    properties["protocol"] = protocol;
    properties["suffix"] = suffix;
    QSharedPointer<QObject> object = Registration::getInstance().create("Native", properties);
    if (object.isNull()) {
        qWarning() << "Unable to create object for resource loader:" << name << source;
        m_resourceLoaders.remove(name);
        return;
    }
    QSharedPointer<Loader> loader = object.dynamicCast<Loader>();
    if (loader.isNull()) {
        qWarning() << "Resolved object is not Loader for resource:" << name << source;
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
