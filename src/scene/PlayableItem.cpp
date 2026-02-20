#include "scene/PlayableItem.h"

PlayableItem::PlayableItem(QObject* parent)
    : Item(parent)
    , m_loop(false)
    , m_playing(false)
{
}

QString PlayableItem::getSource() const {
    return m_source;
}

void PlayableItem::setSource(const QString& source) {
    if (m_source == source) {
        return;
    }
    m_source = source;
    emit sourceChanged();
}

bool PlayableItem::isLoop() const {
    return m_loop;
}

void PlayableItem::setLoop(bool loop) {
    if (m_loop == loop) {
        return;
    }
    m_loop = loop;
    emit loopChanged();
}

bool PlayableItem::isPlaying() const {
    return m_playing;
}

void PlayableItem::play() {
    if (m_playing) {
        return;
    }
    m_playing = true;
    emit playingChanged();
    emit playRequested();
}

void PlayableItem::stop() {
    if (!m_playing) {
        return;
    }
    m_playing = false;
    emit playingChanged();
    emit stopRequested();
}
