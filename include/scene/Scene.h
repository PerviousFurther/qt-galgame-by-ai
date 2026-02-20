#ifndef INCLUDE_SCENE_SCENE_H
#define INCLUDE_SCENE_SCENE_H

#include "Item.h"
#include <QList>
#include <QHash>
#include <QSharedPointer>
#include <QString>

/**
 * @brief Container for Items with support for loading from QML or JSON.
 * 
 * Scene manages a collection of Items and handles the connections between them.
 * Items don't need to directly interact with each other; the Scene manages
 * their relationships and communications.
 * 
 * Note: This class is not thread-safe. All operations should be performed
 * from a single thread or externally synchronized.
 */
class Scene : public Item {
public:
    explicit Scene(QObject* parent = nullptr);
    ~Scene() override;

    /**
     * @brief Add an item to the scene
     * @param item The item to add
     * @return true if successful, false otherwise
     */
    bool addItem(QSharedPointer<Item> item);

    /**
     * @brief Remove an item from the scene by ID
     * @param itemId The ID of the item to remove
     * @return true if successful, false otherwise
     */
    bool removeItem(const QString& itemId);

    /**
     * @brief Get an item by its ID
     * @param itemId The ID of the item to find
     * @return Shared pointer to the item, or nullptr if not found
     */
    QSharedPointer<Item> getItem(const QString& itemId) const;

    /**
     * @brief Get all items in the scene
     * @return List of all items
     */
    const QList<QSharedPointer<Item>>& getItems() const;

    /**
     * @brief Load scene from a file; format is inferred from the URL suffix.
     * @param url Path or qrc URL to a .json or .qml file
     * @return true if successful, false otherwise
     */
    bool load(const QString& url);

    /**
     * @brief Initialize all items in the scene
     */
    void initialize() override;

    /**
     * @brief Update all items in the scene
     * Called every frame
     */
    void update() override;

    /**
     * @brief Fixed update for all items in the scene
     * Called at fixed intervals
     */
    void fixedUpdate() override;

    /**
     * @brief Clear all items from the scene
     */
    void clear();

    QString getType() const override;

private:
    bool loadFromJson(const QString& filePath);
    bool loadFromQml(const QString& filePath);

    QList<QSharedPointer<Item>> m_items;
    QHash<QString, QSharedPointer<Item>> m_itemMap;
};

#endif // INCLUDE_SCENE_SCENE_H
