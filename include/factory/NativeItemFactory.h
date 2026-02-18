#ifndef NATIVEITEMFACTORY_H
#define NATIVEITEMFACTORY_H

#include "factory/Factory.h"

/**
 * @brief Factory for creating native (built-in) Item types
 * 
 * This factory handles creation of all native Item types provided by the engine.
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
 *     auto item = std::make_shared<YourNewItem>();
 *     
 *     // Parse properties
 *     if (properties.count("someProperty")) {
 *         try {
 *             std::string value = std::get<std::string>(properties.at("someProperty"));
 *             item->setSomeProperty(value);
 *         } catch (const std::bad_variant_access&) {
 *             throw std::runtime_error("Property 'someProperty' must be a string");
 *         }
 *     }
 *     
 *     return item;
 * }
 * @endcode
 * 
 * ==================== PROPERTY PARSING ====================
 * Properties from JSON/QML are stored as std::variant<string, int, float, bool>.
 * Use std::get<T>() to extract the value with type checking:
 * 
 * - std::get<std::string>(value)  // For strings
 * - std::get<int>(value)          // For integers
 * - std::get<float>(value)        // For floats
 * - std::get<bool>(value)         // For booleans
 * 
 * Always wrap in try-catch to handle type mismatches gracefully!
 * ================================================================
 */
class NativeItemFactory : public Factory {
public:
    NativeItemFactory();
    virtual ~NativeItemFactory() = default;

    /**
     * @brief Create a native Item from properties
     * @param properties Dictionary of property name-value pairs from JSON/QML
     * @return Shared pointer to the created Item
     * @throws std::runtime_error if properties are invalid or incompatible
     */
    std::shared_ptr<Item> create(const PropertyMap& properties) override;

    /**
     * @brief Get the factory type name
     * @return "Native" to indicate this handles native types
     */
    std::string getTypeName() const override;

private:
    // Helper to get a property value with type checking
    template<typename T>
    T getProperty(const PropertyMap& properties, const std::string& key, const T& defaultValue) const;
};

#endif // NATIVEITEMFACTORY_H
