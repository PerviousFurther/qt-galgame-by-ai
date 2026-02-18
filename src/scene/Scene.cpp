#include "scene/Scene.h"
#include <algorithm>
#include <iostream>

Scene::Scene() {
}

Scene::~Scene() {
    clear();
}

const std::string& Scene::getId() const {
    return m_id;
}

void Scene::setId(const std::string& id) {
    m_id = id;
}

bool Scene::addItem(std::shared_ptr<Item> item) {
    if (!item) {
        return false;
    }

    const std::string& itemId = item->getId();
    
    // Check if item with this ID already exists
    if (!itemId.empty() && m_itemMap.find(itemId) != m_itemMap.end()) {
        std::cerr << "Item with ID '" << itemId << "' already exists in scene" << std::endl;
        return false;
    }

    m_items.push_back(item);
    
    // Add to map if item has an ID
    if (!itemId.empty()) {
        m_itemMap[itemId] = item;
    }

    return true;
}

bool Scene::removeItem(const std::string& itemId) {
    if (itemId.empty()) {
        return false;
    }

    auto mapIt = m_itemMap.find(itemId);
    if (mapIt == m_itemMap.end()) {
        return false;
    }

    auto item = mapIt->second;
    
    // Remove from vector
    // Note: O(n) linear search. For better performance with large item counts,
    // consider maintaining indices or using a different data structure.
    auto it = std::find(m_items.begin(), m_items.end(), item);
    if (it != m_items.end()) {
        (*it)->cleanup();
        m_items.erase(it);
    }

    // Remove from map
    m_itemMap.erase(mapIt);

    return true;
}

std::shared_ptr<Item> Scene::getItem(const std::string& itemId) const {
    auto it = m_itemMap.find(itemId);
    if (it != m_itemMap.end()) {
        return it->second;
    }
    return nullptr;
}

const std::vector<std::shared_ptr<Item>>& Scene::getItems() const {
    return m_items;
}

bool Scene::loadFromJson(const std::string& filePath) {
    // TODO: Implement JSON loading
    // This is a stub for future implementation
    std::cout << "Loading scene from JSON: " << filePath << " (not yet implemented)" << std::endl;
    return false;
}

bool Scene::loadFromQml(const std::string& filePath) {
    // TODO: Implement QML loading
    // This is a stub for future implementation
    std::cout << "Loading scene from QML: " << filePath << " (not yet implemented)" << std::endl;
    return false;
}

void Scene::initialize() {
    for (auto& item : m_items) {
        if (item) {
            item->initialize();
        }
    }
}

void Scene::update(float deltaTime) {
    for (auto& item : m_items) {
        if (item) {
            item->update(deltaTime);
        }
    }
}

void Scene::clear() {
    for (auto& item : m_items) {
        if (item) {
            item->cleanup();
        }
    }
    m_items.clear();
    m_itemMap.clear();
}
