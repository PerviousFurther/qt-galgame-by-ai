#include "scene/Scene.h"
#include "factory/Registration.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace {
QString normalizeScenePath(const QString& filePath) {
    if (filePath.startsWith("qrc:/")) {
        return ":" + filePath.mid(4);
    }
    return filePath;
}
}

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
    QFile file(normalizeScenePath(filePath));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open scene JSON:" << filePath;
        return false;
    }
    const QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        qWarning() << "Failed to parse scene JSON:" << filePath << parseError.errorString();
        return false;
    }

    const QJsonObject root = document.object();
    const QJsonObject sceneObject = root.value("scene").toObject();
    const QString parsedId = sceneObject.value("id").toString();
    if (!parsedId.isEmpty()) {
        setId(parsedId);
    } else if (m_id.isEmpty()) {
        setId(QFileInfo(filePath).completeBaseName());
    }

    const QJsonArray items = sceneObject.value("items").toArray();
    for (const QJsonValue& value : items) {
        const QJsonObject itemObject = value.toObject();
        const QString itemType = itemObject.value("type").toString();
        if (itemType.isEmpty()) {
            qWarning() << "Scene item missing type in scene:" << m_id << "- falling back to Item";
        }
        PropertyMap properties;
        properties["type"] = itemType.isEmpty() ? QStringLiteral("Item") : itemType;
        properties["id"] = itemObject.value("id").toString();
        properties["name"] = itemObject.value("name").toString();
        const QJsonObject itemProperties = itemObject.value("properties").toObject();
        for (auto it = itemProperties.begin(); it != itemProperties.end(); ++it) {
            properties[it.key()] = it.value().toVariant();
        }
        const QSharedPointer<QObject> object = Registration::getInstance().create("Native", properties);
        const QSharedPointer<Item> item = object.dynamicCast<Item>();
        if (!item.isNull()) {
            addItem(item);
        } else {
            qWarning() << "Failed to create scene item: scene=" << m_id
                       << ", itemId=" << itemObject.value("id").toString()
                       << ", type=" << properties["type"].toString();
        }
    }

    return true;
}

bool Scene::loadFromQml(const QString& filePath) {
    const QString normalizedPath = normalizeScenePath(filePath);
    if (!QFile::exists(normalizedPath)) {
        qWarning() << "Scene QML does not exist:" << filePath;
        return false;
    }
    if (m_id.isEmpty()) {
        setId(QFileInfo(filePath).completeBaseName());
    }
    return true;
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
