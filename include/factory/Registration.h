#ifndef REGISTRATION_H
#define REGISTRATION_H

#include "Factory.h"
#include <QSharedPointer>
#include <QHash>
#include <QString>
#include <QStringList>

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
 * auto factory = QSharedPointer<ImageItemFactory>::create();
 * Registration::getInstance().registerFactory(factory);
 * 
 * // Create an item
 * PropertyMap props;
 * props["source"] = "image.png";
 * props["x"] = 100;
 * props["y"] = 200;
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
    bool registerFactory(QSharedPointer<Factory> factory);

    /**
     * @brief Unregister a factory by type name
     * @param typeName The type name of the factory to remove
     * @return true if successful, false if no such factory exists
     */
    bool unregisterFactory(const QString& typeName);

    /**
     * @brief Create an Item using the registered factory
     * @param typeName The type of Item to create
     * @param properties Dictionary of properties from JSON/QML
     * @return Shared pointer to the created Item
     * @throws runtime_error if no factory is registered for the type
     */
    QSharedPointer<Item> createItem(const QString& typeName, const PropertyMap& properties);

    /**
     * @brief Check if a factory is registered for a type
     * @param typeName The type name to check
     * @return true if a factory is registered, false otherwise
     */
    bool hasFactory(const QString& typeName) const;

    /**
     * @brief Get all registered type names
     * @return List of registered type names
     */
    QStringList getRegisteredTypes() const;

private:
    Registration();
    ~Registration() = default;
    Registration(const Registration&) = delete;
    Registration& operator=(const Registration&) = delete;

    QHash<QString, QSharedPointer<Factory>> m_factories;
};

#endif // REGISTRATION_H
