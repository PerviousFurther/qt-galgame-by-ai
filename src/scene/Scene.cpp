#include "scene/Scene.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQueue>
#include <QQuickItem>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QScopedPointer>
#include <QUrl>

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
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open JSON scene file:" << filePath;
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "Failed to parse JSON scene file:" << filePath << parseError.errorString();
        return false;
    }

    clear();

    const QJsonObject rootObj = doc.object();
    const QJsonObject sceneObj = rootObj.value("scene").isObject() ? rootObj.value("scene").toObject() : rootObj;

    const QString sceneId = sceneObj.value("id").toString(sceneObj.value("sceneId").toString());
    if (!sceneId.isEmpty()) {
        setId(sceneId);
    }

    const QJsonArray itemsArray = sceneObj.value("items").toArray();
    for (const QJsonValue& itemValue : itemsArray) {
        if (!itemValue.isObject()) {
            continue;
        }

        const QJsonObject itemObj = itemValue.toObject();
        auto item = QSharedPointer<Item>::create();
        item->setId(itemObj.value("id").toString());
        item->setName(itemObj.value("name").toString());

        if (itemObj.contains("type")) {
            item->setProperty("type", itemObj.value("type").toVariant());
        }
        if (itemObj.contains("className")) {
            item->setProperty("className", itemObj.value("className").toVariant());
        }

        const QJsonObject propertiesObj = itemObj.value("properties").toObject();
        for (auto it = propertiesObj.begin(); it != propertiesObj.end(); ++it) {
            item->setProperty(it.key(), it.value().toVariant());
        }

        addItem(item);
    }

    return true;
}

bool Scene::loadFromQml(const QString& filePath) {
    QQmlEngine engine;
    QQmlComponent component(&engine, QUrl::fromLocalFile(filePath));
    if (component.isError()) {
        qWarning() << "Failed to parse QML scene file:" << filePath << component.errors();
        return false;
    }

    QScopedPointer<QObject> root(component.create());
    if (!root) {
        qWarning() << "Failed to create QML scene root object:" << filePath << component.errors();
        return false;
    }

    clear();

    setId(QFileInfo(filePath).baseName());

    QList<QQuickItem*> qmlItems;
    if (auto* rootItem = qobject_cast<QQuickItem*>(root.data())) {
        QQueue<QQuickItem*> pendingItems;
        for (QQuickItem* child : rootItem->childItems()) {
            pendingItems.enqueue(child);
        }

        while (!pendingItems.isEmpty()) {
            QQuickItem* currentItem = pendingItems.dequeue();
            qmlItems.append(currentItem);
            const QList<QQuickItem*> childItems = currentItem->childItems();
            for (QQuickItem* child : childItems) {
                pendingItems.enqueue(child);
            }
        }
    }

    QHash<QString, int> typeCounters;
    for (QQuickItem* qmlItem : qmlItems) {
        auto item = QSharedPointer<Item>::create();
        const QString qmlType = QString::fromUtf8(qmlItem->metaObject()->className());
        QString objectId = qmlItem->objectName();
        if (objectId.isEmpty()) {
            const int index = typeCounters.value(qmlType) + 1;
            typeCounters[qmlType] = index;
            objectId = qmlType + "_" + QString::number(index);
        }
        item->setId(objectId);
        item->setName(qmlType);
        item->setProperty("qmlType", item->getName());

        const QVariant x = qmlItem->property("x");
        const QVariant y = qmlItem->property("y");
        const QVariant width = qmlItem->property("width");
        const QVariant height = qmlItem->property("height");
        const QVariant visible = qmlItem->property("visible");
        const QVariant source = qmlItem->property("source");
        if (x.isValid()) item->setProperty("x", x);
        if (y.isValid()) item->setProperty("y", y);
        if (width.isValid()) item->setProperty("width", width);
        if (height.isValid()) item->setProperty("height", height);
        if (visible.isValid()) item->setProperty("visible", visible);
        if (source.isValid()) item->setProperty("source", source);

        addItem(item);
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
