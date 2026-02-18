#ifndef FACTORY_H
#define FACTORY_H

#include <QSharedPointer>
#include <QString>
#include <QHash>
#include <QVariant>

// Forward declaration
class Item;

/**
 * @brief Type alias for property map that can be passed from JSON/QML
 * 
 * Properties are stored as QVariant which can hold strings, integers, floats, or booleans.
 * When implementing a Factory, parse these variants to create Item instances.
 */
using PropertyMap = QHash<QString, QVariant>;

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
 *     QSharedPointer<Item> create(const PropertyMap& properties) override {
 *         auto item = QSharedPointer<ImageItem>::create();
 *         // Parse properties and configure item
 *         if (properties.contains("source")) {
 *             QString source = properties.value("source").toString();
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
     * @throws runtime_error if properties are invalid or incompatible
     */
    virtual QSharedPointer<Item> create(const PropertyMap& properties) = 0;

    /**
     * @brief Get the type name this factory creates
     * @return Type name (e.g., "Image", "Text", "Character")
     */
    virtual QString getTypeName() const = 0;
};

#endif // FACTORY_H
