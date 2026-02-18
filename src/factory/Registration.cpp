#include "factory/Registration.h"
#include <stdexcept>
#include <algorithm>

Registration::Registration() {
}

Registration& Registration::getInstance() {
    static Registration instance;
    return instance;
}

bool Registration::registerFactory(std::shared_ptr<Factory> factory) {
    if (!factory) {
        return false;
    }

    std::string typeName = factory->getTypeName();
    
    // Check if factory already exists
    if (m_factories.find(typeName) != m_factories.end()) {
        return false;
    }

    m_factories[typeName] = factory;
    return true;
}

bool Registration::unregisterFactory(const std::string& typeName) {
    auto it = m_factories.find(typeName);
    if (it == m_factories.end()) {
        return false;
    }

    m_factories.erase(it);
    return true;
}

std::shared_ptr<Item> Registration::createItem(const std::string& typeName, const PropertyMap& properties) {
    auto it = m_factories.find(typeName);
    if (it == m_factories.end()) {
        throw std::runtime_error("No factory registered for type: " + typeName);
    }

    return it->second->create(properties);
}

bool Registration::hasFactory(const std::string& typeName) const {
    return m_factories.find(typeName) != m_factories.end();
}

std::vector<std::string> Registration::getRegisteredTypes() const {
    std::vector<std::string> types;
    types.reserve(m_factories.size());
    
    for (const auto& pair : m_factories) {
        types.push_back(pair.first);
    }
    
    return types;
}
