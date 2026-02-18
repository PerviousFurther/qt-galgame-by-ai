#ifndef SCENE_H
#define SCENE_H

#include <QObject>
#include <QList>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>

class Item;

class Scene : public QObject
{
    Q_OBJECT

public:
    explicit Scene(const QString &id, QObject *parent = nullptr);
    ~Scene() override = default;

    QString id() const;

    void addItem(Item *item);
    void removeItem(Item *item);
    Item *findItem(const QString &itemId) const;
    QList<Item *> items() const;

    void loadFromJson(const QJsonObject &json);

signals:
    void itemAdded(Item *item);
    void itemRemoved(Item *item);

private:
    QString m_id;
};

#endif // SCENE_H
