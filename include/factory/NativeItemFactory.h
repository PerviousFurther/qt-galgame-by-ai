#ifndef NATIVEITEMFACTORY_H
#define NATIVEITEMFACTORY_H

#include "factory/Factory.h"

/**
 * @brief Factory for creating native (built-in) Item types
 * 
 * This factory handles creation of all native Item types provided by the engine.
 * 
 * ==================== CODING STANDARDS ====================
 * ⚠️ IMPORTANT: Use Qt containers, NOT STL containers! ⚠️
 * 
 * - Use QString instead of std::string
 * - Use QList instead of std::vector
 * - Use QHash instead of std::map
 * - Use QVariant instead of std::variant
 * - Use QSharedPointer instead of std::shared_ptr
 * 
 * This keeps the codebase consistent with Qt conventions.
 * ==========================================================
 * 
 * ==================== ADDING NEW NATIVE ITEMS ====================
 * When adding a new native Item type to the engine:
 * 
 * 1. Create your Item subclass (e.g., ImageItem, TextItem, etc.)
 * 2. Add a case for your type in the create() method below
 * 3. Parse the properties from PropertyMap to configure your Item
 * 4. Handle property type mismatches with proper error messages
 * 
 * Example:
 * @code
 * if (type == "YourNewType") {
 *     auto item = QSharedPointer<YourNewItem>::create();
 *     
 *     // Parse properties
 *     if (properties.contains("someProperty")) {
 *         if (properties["someProperty"].canConvert<QString>()) {
 *             QString value = properties["someProperty"].toString();
 *             item->setSomeProperty(value);
 *         } else {
 *             throw std::runtime_error("Property 'someProperty' must be a string");
 *         }
 *     }
 *     
 *     return item;
 * }
 * @endcode
 * 
 * ==================== PROPERTY PARSING ====================
 * Properties from JSON/QML are stored as QVariant.
 * Use QVariant methods to extract the value with type checking:
 * 
 * - properties["key"].toString()   // For strings
 * - properties["key"].toInt()      // For integers
 * - properties["key"].toDouble()   // For floats
 * - properties["key"].toBool()     // For booleans
 * - properties["key"].canConvert<T>()  // Check if convertible to type T
 * 
 * Always check canConvert() or handle conversion failures gracefully!
 * ================================================================
 */
class NativeItemFactory : public Factory {
public:
    NativeItemFactory();
    virtual ~NativeItemFactory() = default;

    /**
     * @brief Create a native Item from properties
     * @param properties Dictionary of property name-value pairs from JSON/QML
     * @return QObject pointer to a created native Item/Loader, or nullptr on invalid data
     */
    QObject* create(const PropertyMap& properties) override;

    /**
     * @brief Get the factory type name
     * @return "Native" to indicate this handles native types
     */
    QString getTypeName() const override;
};

#endif // NATIVEITEMFACTORY_H
