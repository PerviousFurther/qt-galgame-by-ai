#ifndef INCLUDE_SCENE_VIDEOITEM_H
#define INCLUDE_SCENE_VIDEOITEM_H

#include "scene/PlayableItem.h"

class VideoItem : public PlayableItem {
    Q_OBJECT
public:
    explicit VideoItem(QObject* parent = nullptr);
    QString getType() const override;
};

#endif // INCLUDE_SCENE_VIDEOITEM_H
