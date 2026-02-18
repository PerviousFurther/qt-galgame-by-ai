#include "factory/Registration.h"
#include <stdexcept>

Registration::Registration() {
}

Registration& Registration::getInstance() {
    static Registration instance;
    return instance;
}

bool Registration::registerFactory(QSharedPointer<Factory> factory) {
    if (!factory) {
        return false;
    }

    QString typeName = factory->getTypeName();
    
    // Check if factory already exists
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
        throw std::runtime_error(("No factory registered for type: " + typeName).toStdString());
    }

    return m_factories[typeName]->create(properties);
}

bool Registration::hasFactory(const QString& typeName) const {
    return m_factories.contains(typeName);
}

QStringList Registration::getRegisteredTypes() const {
    return m_factories.keys();
}
