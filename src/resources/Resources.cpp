#include "codingstyle.h" // include/codingstyle.h
#include "resources/Resources.h"

#include "factory/Registration.h"
#include "resources/Loader.h"

#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include <QFile>
#include <QFileInfo>

namespace {
bool supportsLoaderSuffix(const QString& suffix) {
    return suffix == "bmp" || suffix == "png" || suffix == "jpg" || suffix == "jpeg" ||
           suffix == "webp" || suffix == "mp4" || suffix == "json";
}
}

Resources::Resources() {
    registerDefaultLoaders();
    registerResourcesFromQrc();
}

Resources& Resources::getInstance() {
    static Resources instance;
    return instance;
}

void Resources::registerDefaultLoaders() {
    // Unified creation flow: loader selection comes from PropertyMap (protocol/suffix/type/source).
}

void Resources::addResource(const QString& name, const QVariant& value) {
    if (value.canConvert<QString>() && !resourceExists(value.toString())) {
        qWarning() << "Resource path does not exist:" << value.toString();
        m_resources.remove(name);
        m_resourceLoaders.remove(name);
        return;
    }
    m_resources[name] = value;
    resolveLoaderForResource(name, value);
}

QSharedPointer<Loader> Resources::getLoader(const QString& name) const {
    return m_resourceLoaders.value(name);
}

QVariant Resources::getResource(const QString& name) const {
    return m_resources.value(name);
}

QStringList Resources::getResourceUrlsBySuffix(const QString& suffix) const {
    QStringList urls;
    const QString suffixLower = suffix.toLower();
    for (auto it = m_resources.constBegin(); it != m_resources.constEnd(); ++it) {
        if (!it.value().canConvert<QString>()) {
            continue;
        }
        const QString url = it.value().toString();
        if (extractSuffix(url) == suffixLower) {
            urls.append(url);
        }
    }
    return urls;
}

void Resources::registerResourcesFromQrc() {
    QDirIterator it(":/", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QString qrcPath = it.next();
        const QString qrcUrl = normalizeResourcePath(qrcPath);
        addResource(qrcUrl, qrcUrl);
    }
}

void Resources::resolveLoaderForResource(const QString& name, const QVariant& value) {
    if (!value.canConvert<QString>()) {
        m_resourceLoaders.remove(name);
        return;
    }

    const QString source = value.toString();
    const QString protocol = extractProtocol(source);
    const QString suffix = extractSuffix(source);
    if (!supportsLoaderSuffix(suffix)) {
        m_resourceLoaders.remove(name);
        return;
    }

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

QString Resources::normalizeResourcePath(const QString& value) {
    if (value.startsWith("qrc:/")) {
        return value;
    }
    if (value.startsWith(":/")) {
        return "qrc" + value;
    }
    return value;
}

bool Resources::resourceExists(const QString& value) {
    if (value.startsWith("qrc:/")) {
        return QFile::exists(":" + value.mid(4));
    }
    if (value.startsWith(":/")) {
        return QFile::exists(value);
    }
    return QFileInfo::exists(value);
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
