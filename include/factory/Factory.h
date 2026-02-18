#ifndef FACTORY_H
#define FACTORY_H

#include <memory>
#include <string>
#include <map>
#include <variant>

// Forward declaration
class Item;

/**
 * @brief Type alias for property values that can be passed from JSON/QML
 * 
 * Properties can be strings, integers, floats, or booleans.
 * When implementing a Factory, parse these variants to create Item instances.
 */
using PropertyValue = std::variant<std::string, int, float, bool>;
using PropertyMap = std::map<std::string, PropertyValue>;

/**
 * @brief Abstract Factory base class for creating Items
 * 
 * Factories are used to create Item instances from configuration data
 * (JSON or QML). Each Factory implementation handles specific Item types.
 * 
 * When creating a new Item type:
 * 1. Inherit from Item
 * 2. Create a Factory implementation
 * 3. Register the factory with Registration singleton
 * 
 * Example:
 * @code
 * class ImageItemFactory : public Factory {
 * public:
 *     std::shared_ptr<Item> create(const PropertyMap& properties) override {
 *         auto item = std::make_shared<ImageItem>();
 *         // Parse properties and configure item
 *         if (properties.count("source")) {
 *             std::string source = std::get<std::string>(properties.at("source"));
 *             item->setSource(source);
 *         }
 *         return item;
 *     }
 * };
 * @endcode
 */
class Factory {
public:
    virtual ~Factory() = default;

    /**
     * @brief Create an Item from properties
     * @param properties Dictionary of property name-value pairs from JSON/QML
     * @return Shared pointer to the created Item
     * @throws std::runtime_error if properties are invalid or incompatible
     */
    virtual std::shared_ptr<Item> create(const PropertyMap& properties) = 0;

    /**
     * @brief Get the type name this factory creates
     * @return Type name (e.g., "Image", "Text", "Character")
     */
    virtual std::string getTypeName() const = 0;
};

#endif // FACTORY_H
