#include "factory/Registration.h"
#include "scene/Item.h"

#include <QDebug>

Registration::Registration() {
}

Registration& Registration::getInstance() {
    static Registration instance;
    return instance;
}

bool Registration::registerFactory(QSharedPointer<Factory> factory) {
    if (factory.isNull()) {
        return false;
    }

    const QString typeName = factory->getTypeName();
    if (m_factories.contains(typeName)) {
        return false;
    }

    m_factories[typeName] = factory;
    return true;
}

bool Registration::unregisterFactory(const QString& typeName) {
    if (!m_factories.contains(typeName)) {
        return false;
    }
    m_factories.remove(typeName);
    return true;
}

QSharedPointer<Item> Registration::createItem(const QString& typeName, const PropertyMap& properties) {
    if (!m_factories.contains(typeName)) {
        qWarning() << "No factory registered for type:" << typeName;
        return {};
    }

    QObject* object = m_factories[typeName]->create(properties);
    if (!object) {
        return {};
    }

    Item* item = qobject_cast<Item*>(object);
    if (!item) {
        qWarning() << "Factory returned non-Item object for type:" << typeName;
        delete object;
        return {};
    }
    return QSharedPointer<Item>(item);
}

QSharedPointer<Loader> Registration::createLoader(const QString& protocol, const QString& suffix, const PropertyMap& properties) {
    for (const LoaderRegistry& loaderRegistry : m_loaderRegistries) {
        if (loaderRegistry.protocol == protocol && loaderRegistry.suffix == suffix) {
            if (!m_factories.contains(loaderRegistry.factoryType)) {
                qWarning() << "Factory not found for loader registry:" << loaderRegistry.factoryType;
                return {};
            }

            PropertyMap finalProperties = properties;
            finalProperties["type"] = loaderRegistry.loaderType;

            QObject* object = m_factories[loaderRegistry.factoryType]->create(finalProperties);
            if (!object) {
                return {};
            }

            Loader* loader = qobject_cast<Loader*>(object);
            if (!loader) {
                qWarning() << "Factory returned non-Loader object:" << loaderRegistry.loaderType;
                delete object;
                return {};
            }
            return QSharedPointer<Loader>(loader);
        }
    }

    qWarning() << "No loader registry found for protocol/suffix:" << protocol << suffix;
    return {};
}

bool Registration::hasFactory(const QString& typeName) const {
    return m_factories.contains(typeName);
}

QStringList Registration::getRegisteredTypes() const {
    return m_factories.keys();
}

bool Registration::registerLoader(const LoaderRegistry& loaderRegistry) {
    if (loaderRegistry.protocol.isEmpty() || loaderRegistry.suffix.isEmpty() ||
        loaderRegistry.factoryType.isEmpty() || loaderRegistry.loaderType.isEmpty()) {
        return false;
    }

    for (const LoaderRegistry& existing : m_loaderRegistries) {
        if (existing.protocol == loaderRegistry.protocol &&
            existing.suffix == loaderRegistry.suffix) {
            return false;
        }
    }
    m_loaderRegistries.append(loaderRegistry);
    return true;
}

QList<Registration::LoaderRegistry> Registration::getRegisteredLoaders() const {
    return m_loaderRegistries;
}
