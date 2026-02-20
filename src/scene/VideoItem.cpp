#include "scene/VideoItem.h"

VideoItem::VideoItem(QObject* parent)
    : PlayableItem(parent)
{
}

QString VideoItem::getType() const {
    return QStringLiteral("Video");
}
