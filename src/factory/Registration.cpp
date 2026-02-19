#include "factory/Registration.h"

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

QSharedPointer<QObject> Registration::create(const QString& typeName, const PropertyMap& properties) {
    if (!m_factories.contains(typeName)) {
        qWarning() << "No factory registered for type:" << typeName;
        return {};
    }

    QObject* object = m_factories[typeName]->create(properties);
    if (!object) {
        return {};
    }

    return QSharedPointer<QObject>(object, [](QObject* ptr) { delete ptr; });
}

bool Registration::hasFactory(const QString& typeName) const {
    return m_factories.contains(typeName);
}

QStringList Registration::getRegisteredTypes() const {
    return m_factories.keys();
}
