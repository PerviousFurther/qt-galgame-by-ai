#include "scene/Item.h"

Item::Item() : m_initialized(false) {
}

Item::~Item() {
    cleanup();
}

const std::string& Item::getId() const {
    return m_id;
}

void Item::setId(const std::string& id) {
    m_id = id;
}

const std::string& Item::getName() const {
    return m_name;
}

void Item::setName(const std::string& name) {
    m_name = name;
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
    m_initialized = false;
}
