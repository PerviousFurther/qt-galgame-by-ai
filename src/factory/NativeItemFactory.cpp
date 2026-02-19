#include "codingstyle.h" // include/codingstyle.h
#include "factory/NativeItemFactory.h"
#include "resources/FormatSupport.h"
#include "resources/Loader.h"
#include "scene/AudioItem.h"
#include "scene/CharacterItem.h"
#include "scene/Item.h"
#include "scene/VideoItem.h"

#include <QDebug>
#include <stdexcept>

NativeItemFactory::NativeItemFactory() {
}

QObject* NativeItemFactory::create(const PropertyMap& properties) {
    QString type;
    if (properties.contains("type")) {
        if (!properties["type"].canConvert<QString>()) {
            throw std::runtime_error("Property 'type' must be a string");
        }
        type = properties["type"].toString();
    } else if (properties.contains("protocol") || properties.contains("suffix")) {
        if (!properties.contains("protocol") || !properties.contains("suffix")) {
            throw std::runtime_error("Both 'protocol' and 'suffix' are required for loader inference");
        }
        if (!properties["protocol"].canConvert<QString>()) {
            throw std::runtime_error("Property 'protocol' must be a string");
        }
        if (!properties["suffix"].canConvert<QString>()) {
            throw std::runtime_error("Property 'suffix' must be a string");
        }
        const QString protocol = properties["protocol"].toString();
        const QString suffix = properties["suffix"].toString().toLower();
        if (protocol != "file" && protocol != "qrc" && protocol != "http" && protocol != "https") {
            throw std::runtime_error("Unsupported protocol: " + protocol.toStdString());
        }
        if (supportedImageSuffixes().contains(suffix)) {
            type = "BitmapLoader";
        } else if (suffix == "json") {
            type = "JsonLoader";
        } else if (suffix == "qml") {
            type = "QmlLoader";
        } else {
            qWarning() << "Unrecognized file extension; defaulting to VideoLoader for media playback:" << suffix;
            type = "VideoLoader";
        }
    } else {
        throw std::runtime_error("Property 'type' is required unless loader protocol/suffix are provided");
    }

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
        return new BitmapLoader();
    }

    if (type == "VideoLoader") {
        return new VideoLoader();
    }

    if (type == "JsonLoader") {
        return new JsonLoader();
    }

    if (type == "QmlLoader") {
        return new QmlLoader();
    }

    qWarning() << "Unknown native create type:" << type;
    return nullptr;
}

QString NativeItemFactory::getTypeName() const {
    return "Native";
}
