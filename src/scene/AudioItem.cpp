#include "scene/AudioItem.h"

AudioItem::AudioItem(QObject* parent)
    : PlayableItem(parent)
{
}

QString AudioItem::getType() const {
    return QStringLiteral("Audio");
}
