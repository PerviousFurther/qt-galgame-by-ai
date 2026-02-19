#ifndef INCLUDE_SCENE_VIDEOITEM_H
#define INCLUDE_SCENE_VIDEOITEM_H
#include "codingstyle.h" // include/codingstyle.h

#include "scene/Item.h"

class VideoItem : public Item {
    Q_OBJECT
public:
    explicit VideoItem(QObject* parent = nullptr);

    void setSource(const QString& source);
    const QString& getSource() const;

    void setLoop(bool loop);
    bool isLoop() const;

    void play();
    void stop();
    bool isPlaying() const;

    QString getType() const override;

signals:
    void playRequested();
    void stopRequested();

private:
    QString m_source;
    bool m_loop;
    bool m_playing;
};

#endif // INCLUDE_SCENE_VIDEOITEM_H
