#include "scene.h"
#include "item.h"

Scene::Scene(const QString &id, QObject *parent)
    : QObject(parent)
    , m_id(id)
{
}

QString Scene::id() const
{
    return m_id;
}

void Scene::addItem(Item *item)
{
    item->setParent(this);
    emit itemAdded(item);
}

void Scene::removeItem(Item *item)
{
    if (item->parent() == this) {
        emit itemRemoved(item);
        item->setParent(nullptr);
    }
}

Item *Scene::findItem(const QString &itemId) const
{
    const auto children = findChildren<Item *>(Qt::FindDirectChildrenOnly);
    for (auto *child : children) {
        if (child->id() == itemId)
            return child;
    }
    return nullptr;
}

QList<Item *> Scene::items() const
{
    return findChildren<Item *>(Qt::FindDirectChildrenOnly);
}

void Scene::loadFromJson(const QJsonObject &json)
{
    const QJsonArray itemsArray = json[QStringLiteral("items")].toArray();
    for (const auto &val : itemsArray) {
        const QJsonObject obj = val.toObject();
        const QString className = obj[QStringLiteral("className")].toString();
        const QString id = obj[QStringLiteral("id")].toString();

        auto *item = new Item(id, className, this);
        emit itemAdded(item);

        const QJsonObject props = obj[QStringLiteral("properties")].toObject();
        for (auto it = props.begin(); it != props.end(); ++it) {
            item->setProperty(it.key().toUtf8().constData(), it.value().toVariant());
        }
    }
}
