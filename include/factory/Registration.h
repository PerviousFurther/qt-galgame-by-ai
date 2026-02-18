#ifndef REGISTRATION_H
#define REGISTRATION_H

#include "Factory.h"
#include <memory>
#include <map>
#include <string>
#include <vector>

/**
 * @brief Registration singleton for managing Item factories
 * 
 * Registration maintains a registry of Factory objects that can create
 * different types of Items from configuration data. This enables dynamic
 * Item creation from JSON or QML files.
 * 
 * Usage:
 * @code
 * // Register a factory
 * auto factory = std::make_shared<ImageItemFactory>();
 * Registration::getInstance().registerFactory(factory);
 * 
 * // Create an item
 * PropertyMap props = {{"source", "image.png"}, {"x", 100}, {"y", 200}};
 * auto item = Registration::getInstance().createItem("Image", props);
 * 
 * // Unregister when no longer needed
 * Registration::getInstance().unregisterFactory("Image");
 * @endcode
 */
class Registration {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the Registration singleton
     */
    static Registration& getInstance();

    /**
     * @brief Register a factory for creating Items
     * @param factory Shared pointer to the Factory
     * @return true if successful, false if a factory with this type already exists
     */
    bool registerFactory(std::shared_ptr<Factory> factory);

    /**
     * @brief Unregister a factory by type name
     * @param typeName The type name of the factory to remove
     * @return true if successful, false if no such factory exists
     */
    bool unregisterFactory(const std::string& typeName);

    /**
     * @brief Create an Item using the registered factory
     * @param typeName The type of Item to create
     * @param properties Dictionary of properties from JSON/QML
     * @return Shared pointer to the created Item
     * @throws std::runtime_error if no factory is registered for the type
     */
    std::shared_ptr<Item> createItem(const std::string& typeName, const PropertyMap& properties);

    /**
     * @brief Check if a factory is registered for a type
     * @param typeName The type name to check
     * @return true if a factory is registered, false otherwise
     */
    bool hasFactory(const std::string& typeName) const;

    /**
     * @brief Get all registered type names
     * @return Vector of registered type names
     */
    std::vector<std::string> getRegisteredTypes() const;

private:
    Registration();
    ~Registration() = default;
    Registration(const Registration&) = delete;
    Registration& operator=(const Registration&) = delete;

    std::map<std::string, std::shared_ptr<Factory>> m_factories;
};

#endif // REGISTRATION_H
