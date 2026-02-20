#ifndef INCLUDE_SCENE_AUDIOITEM_H
#define INCLUDE_SCENE_AUDIOITEM_H

#include "scene/PlayableItem.h"

class AudioItem : public PlayableItem {
    Q_OBJECT
public:
    explicit AudioItem(QObject* parent = nullptr);
    QString getType() const override;
};

#endif // INCLUDE_SCENE_AUDIOITEM_H
