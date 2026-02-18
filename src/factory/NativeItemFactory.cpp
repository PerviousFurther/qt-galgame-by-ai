#include "factory/NativeItemFactory.h"
#include "scene/Item.h"
#include <stdexcept>

NativeItemFactory::NativeItemFactory() {
}

std::shared_ptr<Item> NativeItemFactory::create(const PropertyMap& properties) {
    // Get the type property to determine which Item to create
    std::string type;
    try {
        if (properties.count("type")) {
            type = std::get<std::string>(properties.at("type"));
        } else {
            throw std::runtime_error("Property 'type' is required to create an Item");
        }
    } catch (const std::bad_variant_access&) {
        throw std::runtime_error("Property 'type' must be a string");
    }

    // ==================== ADD NEW NATIVE ITEM TYPES HERE ====================
    // Create the appropriate Item based on type
    
    if (type == "Item" || type == "Base") {
        // Create a basic Item
        auto item = std::make_shared<Item>();
        
        // Set common properties
        if (properties.count("id")) {
            try {
                item->setId(std::get<std::string>(properties.at("id")));
            } catch (const std::bad_variant_access&) {
                throw std::runtime_error("Property 'id' must be a string");
            }
        }
        
        if (properties.count("name")) {
            try {
                item->setName(std::get<std::string>(properties.at("name")));
            } catch (const std::bad_variant_access&) {
                throw std::runtime_error("Property 'name' must be a string");
            }
        }
        
        return item;
    }
    
    // TODO: Add more native Item types here as they are implemented
    // Example template:
    //
    // if (type == "Image") {
    //     auto item = std::make_shared<ImageItem>();
    //     
    //     if (properties.count("source")) {
    //         try {
    //             item->setSource(std::get<std::string>(properties.at("source")));
    //         } catch (const std::bad_variant_access&) {
    //             throw std::runtime_error("Property 'source' must be a string");
    //         }
    //     }
    //     
    //     if (properties.count("x")) {
    //         try {
    //             item->setX(std::get<int>(properties.at("x")));
    //         } catch (const std::bad_variant_access&) {
    //             throw std::runtime_error("Property 'x' must be an integer");
    //         }
    //     }
    //     
    //     // Set common properties (id, name)
    //     if (properties.count("id")) {
    //         item->setId(std::get<std::string>(properties.at("id")));
    //     }
    //     
    //     return item;
    // }
    
    // =========================================================================

    // If we get here, the type is not recognized
    throw std::runtime_error("Unknown native Item type: " + type);
}

std::string NativeItemFactory::getTypeName() const {
    return "Native";
}
