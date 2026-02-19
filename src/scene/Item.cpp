#include "scene/Item.h"

Item::Item() : m_initialized(false) {
}

Item::~Item() {
    cleanup();
}

const QString& Item::getId() const {
    return m_id;
}

void Item::setId(const QString& id) {
    m_id = id;
}

const QString& Item::getName() const {
    return m_name;
}

void Item::setName(const QString& name) {
    m_name = name;
}

void Item::setProperty(const QString& key, const QVariant& value) {
    if (!key.isEmpty()) {
        m_properties[key] = value;
    }
}

QVariant Item::getProperty(const QString& key) const {
    return m_properties.value(key);
}

const QHash<QString, QVariant>& Item::getProperties() const {
    return m_properties;
}

void Item::initialize() {
    m_initialized = true;
}

void Item::update() {
    // Base implementation does nothing
    // Derived classes can override to implement specific behavior
    // Use Timer::getInstance().getDeltaTime() to get delta time in seconds
}

void Item::fixedUpdate() {
    // Base implementation does nothing
    // Derived classes can override for fixed-interval updates (physics, mini-games)
    // Use Timer::getInstance().getFixedUpdateInterval() to get the interval
}

void Item::cleanup() {
    m_properties.clear();
    m_initialized = false;
}
