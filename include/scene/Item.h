#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <memory>

/**
 * @brief Base class for all items that can be placed in a scene.
 * 
 * Item is the fundamental building block of the galgame engine.
 * Most objects in the scene inherit from this class.
 */
class Item {
public:
    Item();
    virtual ~Item();

    /**
     * @brief Get the unique identifier of this item
     * @return The item's ID
     */
    const std::string& getId() const;

    /**
     * @brief Set the unique identifier of this item
     * @param id The new ID
     */
    void setId(const std::string& id);

    /**
     * @brief Get the name of this item
     * @return The item's name
     */
    const std::string& getName() const;

    /**
     * @brief Set the name of this item
     * @param name The new name
     */
    void setName(const std::string& name);

    /**
     * @brief Initialize the item
     * Called when the item is added to a scene
     */
    virtual void initialize();

    /**
     * @brief Update the item state
     * Called every frame. Use Timer::getInstance().getDeltaTime() to get delta time.
     */
    virtual void update();

    /**
     * @brief Fixed update for physics and time-critical operations
     * Called at fixed intervals (e.g., for mini-games like Snake).
     * Use Timer::getInstance().getFixedUpdateInterval() to get the interval.
     */
    virtual void fixedUpdate();

    /**
     * @brief Clean up resources when item is removed
     */
    virtual void cleanup();

protected:
    std::string m_id;
    std::string m_name;
    bool m_initialized;
};

#endif // ITEM_H
