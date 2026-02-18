#ifndef SCENE_H
#define SCENE_H

#include "Item.h"
#include <vector>
#include <map>
#include <memory>
#include <string>

/**
 * @brief Container for Items with support for loading from QML or JSON.
 * 
 * Scene manages a collection of Items and handles the connections between them.
 * Items don't need to directly interact with each other; the Scene manages
 * their relationships and communications.
 */
class Scene {
public:
    Scene();
    virtual ~Scene();

    /**
     * @brief Get the unique identifier of this scene
     * @return The scene's ID
     */
    const std::string& getId() const;

    /**
     * @brief Set the unique identifier of this scene
     * @param id The new ID
     */
    void setId(const std::string& id);

    /**
     * @brief Add an item to the scene
     * @param item The item to add
     * @return true if successful, false otherwise
     */
    bool addItem(std::shared_ptr<Item> item);

    /**
     * @brief Remove an item from the scene by ID
     * @param itemId The ID of the item to remove
     * @return true if successful, false otherwise
     */
    bool removeItem(const std::string& itemId);

    /**
     * @brief Get an item by its ID
     * @param itemId The ID of the item to find
     * @return Shared pointer to the item, or nullptr if not found
     */
    std::shared_ptr<Item> getItem(const std::string& itemId) const;

    /**
     * @brief Get all items in the scene
     * @return Vector of all items
     */
    const std::vector<std::shared_ptr<Item>>& getItems() const;

    /**
     * @brief Load scene from a JSON file
     * @param filePath Path to the JSON file
     * @return true if successful, false otherwise
     */
    bool loadFromJson(const std::string& filePath);

    /**
     * @brief Load scene from a QML file
     * @param filePath Path to the QML file
     * @return true if successful, false otherwise
     */
    bool loadFromQml(const std::string& filePath);

    /**
     * @brief Initialize all items in the scene
     */
    void initialize();

    /**
     * @brief Update all items in the scene
     * @param deltaTime Time elapsed since last update in seconds
     */
    void update(float deltaTime);

    /**
     * @brief Clear all items from the scene
     */
    void clear();

private:
    std::string m_id;
    std::vector<std::shared_ptr<Item>> m_items;
    std::map<std::string, std::shared_ptr<Item>> m_itemMap;
};

#endif // SCENE_H
