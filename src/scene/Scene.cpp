#include "codingstyle.h" // include/codingstyle.h
#include "scene/Scene.h"
#include <QDebug>

Scene::Scene() {
}

Scene::~Scene() {
    clear();
}

const QString& Scene::getId() const {
    return m_id;
}

void Scene::setId(const QString& id) {
    m_id = id;
}

bool Scene::addItem(QSharedPointer<Item> item) {
    if (!item) {
        return false;
    }

    const QString& itemId = item->getId();
    
    // Check if item with this ID already exists
    if (!itemId.isEmpty() && m_itemMap.contains(itemId)) {
        qWarning() << "Item with ID '" << itemId << "' already exists in scene";
        return false;
    }

    m_items.append(item);
    
    // Add to map if item has an ID
    if (!itemId.isEmpty()) {
        m_itemMap[itemId] = item;
    }

    return true;
}

bool Scene::removeItem(const QString& itemId) {
    if (itemId.isEmpty()) {
        return false;
    }

    auto mapIt = m_itemMap.find(itemId);
    if (mapIt == m_itemMap.end()) {
        return false;
    }

    auto item = mapIt.value();
    
    // Remove from list
    // Note: O(n) linear search. For better performance with large item counts,
    // consider maintaining indices or using a different data structure.
    int index = m_items.indexOf(item);
    if (index >= 0) {
        m_items[index]->cleanup();
        m_items.removeAt(index);
    }

    // Remove from map
    m_itemMap.erase(mapIt);

    return true;
}

QSharedPointer<Item> Scene::getItem(const QString& itemId) const {
    auto it = m_itemMap.find(itemId);
    if (it != m_itemMap.end()) {
        return it.value();
    }
    return QSharedPointer<Item>();
}

const QList<QSharedPointer<Item>>& Scene::getItems() const {
    return m_items;
}

bool Scene::loadFromJson(const QString& filePath) {
    // TODO: Implement JSON loading
    // This is a stub for future implementation
    qDebug() << "Loading scene from JSON:" << filePath << "(not yet implemented)";
    return false;
}

bool Scene::loadFromQml(const QString& filePath) {
    // TODO: Implement QML loading
    // This is a stub for future implementation
    qDebug() << "Loading scene from QML:" << filePath << "(not yet implemented)";
    return false;
}

void Scene::initialize() {
    for (auto& item : m_items) {
        if (item) {
            item->initialize();
        }
    }
}

void Scene::update() {
    for (auto& item : m_items) {
        if (item) {
            item->update();
        }
    }
}

void Scene::fixedUpdate() {
    for (auto& item : m_items) {
        if (item) {
            item->fixedUpdate();
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
