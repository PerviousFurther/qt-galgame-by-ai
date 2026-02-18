#include "factory/NativeItemFactory.h"
#include "scene/Item.h"
#include <stdexcept>

NativeItemFactory::NativeItemFactory() {
}

QSharedPointer<Item> NativeItemFactory::create(const PropertyMap& properties) {
    // Get the type property to determine which Item to create
    QString type;
    if (properties.contains("type")) {
        if (properties["type"].canConvert<QString>()) {
            type = properties["type"].toString();
        } else {
            throw std::runtime_error("Property 'type' must be a string");
        }
    } else {
        throw std::runtime_error("Property 'type' is required to create an Item");
    }

    // ==================== ADD NEW NATIVE ITEM TYPES HERE ====================
    // Create the appropriate Item based on type
    
    if (type == "Item" || type == "Base") {
        // Create a basic Item
        auto item = QSharedPointer<Item>::create();
        
        // Set common properties
        if (properties.contains("id")) {
            if (properties["id"].canConvert<QString>()) {
                item->setId(properties["id"].toString());
            } else {
                throw std::runtime_error("Property 'id' must be a string");
            }
        }
        
        if (properties.contains("name")) {
            if (properties["name"].canConvert<QString>()) {
                item->setName(properties["name"].toString());
            } else {
                throw std::runtime_error("Property 'name' must be a string");
            }
        }
        
        return item;
    }
    
    // TODO: Add more native Item types here as they are implemented
    // Example template:
    //
    // if (type == "Image") {
    //     auto item = QSharedPointer<ImageItem>::create();
    //     
    //     if (properties.contains("source")) {
    //         if (properties["source"].canConvert<QString>()) {
    //             item->setSource(properties["source"].toString());
    //         } else {
    //             throw std::runtime_error("Property 'source' must be a string");
    //         }
    //     }
    //     
    //     if (properties.contains("x")) {
    //         if (properties["x"].canConvert<int>()) {
    //             item->setX(properties["x"].toInt());
    //         } else {
    //             throw std::runtime_error("Property 'x' must be an integer");
    //         }
    //     }
    //     
    //     // Set common properties (id, name)
    //     if (properties.contains("id")) {
    //         item->setId(properties["id"].toString());
    //     }
    //     
    //     return item;
    // }
    
    // =========================================================================

    // If we get here, the type is not recognized
    throw std::runtime_error(("Unknown native Item type: " + type).toStdString());
}

QString NativeItemFactory::getTypeName() const {
    return "Native";
}
