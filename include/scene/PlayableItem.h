#ifndef INCLUDE_SCENE_PLAYABLEITEM_H
#define INCLUDE_SCENE_PLAYABLEITEM_H

#include "scene/Item.h"

/**
 * @brief Shared base for items that have a playable media source (audio or video).
 *
 * AudioItem and VideoItem inherit from this class and only need to override
 * getType().  All common state and signals live here so the two concrete
 * subclasses remain trivial.
 */
class PlayableItem : public Item {
    Q_OBJECT
    Q_PROPERTY(QString source READ getSource WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool loop READ isLoop WRITE setLoop NOTIFY loopChanged)
    // playing is read-only from outside: callers drive it through play()/stop()
    // so that signal emission and state update stay consistent.
    Q_PROPERTY(bool playing READ isPlaying NOTIFY playingChanged)
public:
    explicit PlayableItem(QObject* parent = nullptr);

    QString getSource() const;
    void setSource(const QString& source);

    bool isLoop() const;
    void setLoop(bool loop);

    bool isPlaying() const;

    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();

signals:
    void sourceChanged();
    void loopChanged();
    void playingChanged();
    void playRequested();
    void stopRequested();

protected:
    QString m_source;
    bool m_loop;
    bool m_playing;
};

#endif // INCLUDE_SCENE_PLAYABLEITEM_H
