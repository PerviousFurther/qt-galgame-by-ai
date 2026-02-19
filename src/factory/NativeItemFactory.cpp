#include "codingstyle.h" // include/codingstyle.h
#include "factory/NativeItemFactory.h"
#include "resources/Loader.h"
#include "scene/Item.h"

#include <QDebug>

NativeItemFactory::NativeItemFactory() {
}

QObject* NativeItemFactory::create(const PropertyMap& properties) {
    if (!properties.contains("type") || !properties["type"].canConvert<QString>()) {
        qWarning() << "Property 'type' is required and must be a string";
        return nullptr;
    }

    const QString type = properties["type"].toString();

    auto setCommonProperties = [&properties](Item* item) {
        if (properties.contains("id") && properties["id"].canConvert<QString>()) {
            item->setId(properties["id"].toString());
        }
        if (properties.contains("name") && properties["name"].canConvert<QString>()) {
            item->setName(properties["name"].toString());
        }
    };

    if (type == "Item" || type == "Base") {
        auto* item = new Item();
        setCommonProperties(item);
        return item;
    }

    if (type == "Audio" || type == "AudioPlayer") {
        auto* item = new AudioItem();
        setCommonProperties(item);
        if (properties.contains("source") && properties["source"].canConvert<QString>()) {
            item->setSource(properties["source"].toString());
        }
        if (properties.contains("loop") && properties["loop"].canConvert<bool>()) {
            item->setLoop(properties["loop"].toBool());
        }
        return item;
    }

    if (type == "Video" || type == "VideoPlayer") {
        auto* item = new VideoItem();
        setCommonProperties(item);
        if (properties.contains("source") && properties["source"].canConvert<QString>()) {
            item->setSource(properties["source"].toString());
        }
        if (properties.contains("loop") && properties["loop"].canConvert<bool>()) {
            item->setLoop(properties["loop"].toBool());
        }
        return item;
    }

    if (type == "Character" || type == "Sprite") {
        auto* item = new CharacterItem();
        setCommonProperties(item);
        if (properties.contains("source") && properties["source"].canConvert<QString>()) {
            item->setPortrait(properties["source"].toString());
        }
        if (properties.contains("expression") && properties["expression"].canConvert<QString>()) {
            item->setExpression(properties["expression"].toString());
        }
        if (properties.contains("visible") && properties["visible"].canConvert<bool>()) {
            item->setVisible(properties["visible"].toBool());
        }
        return item;
    }

    if (type == "BitmapLoader") {
        QString suffix = "bmp";
        if (properties.contains("suffix") && properties["suffix"].canConvert<QString>()) {
            suffix = properties["suffix"].toString();
        }
        return new BitmapLoader(suffix);
    }

    if (type == "VideoLoader") {
        return new VideoLoader();
    }

    qWarning() << "Unknown native create type:" << type;
    return nullptr;
}

QString NativeItemFactory::getTypeName() const {
    return "Native";
}
