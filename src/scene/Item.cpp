#include "scene/Item.h"

Item::Item(QObject* parent)
    : QObject(parent)
    , m_initialized(false) {
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

void Item::initialize() {
    m_initialized = true;
}

void Item::update() {
    // Base implementation does nothing
    // Derived classes can override to implement specific behavior
    // Use Execution::getInstance().getDeltaTime() to get delta time in seconds
}

void Item::fixedUpdate() {
    // Base implementation does nothing
    // Derived classes can override for fixed-interval updates (physics, mini-games)
    // Use Execution::getInstance().getFixedUpdateInterval() to get the interval
}

void Item::cleanup() {
    m_initialized = false;
}

QString Item::getType() const {
    return "Item";
}
