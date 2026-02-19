#include "codingstyle.h" // include/codingstyle.h
#include "scene/AudioItem.h"

AudioItem::AudioItem(QObject* parent)
    : Item(parent)
    , m_loop(false)
    , m_playing(false)
{
}

void AudioItem::setSource(const QString& source) {
    m_source = source;
}

const QString& AudioItem::getSource() const {
    return m_source;
}

void AudioItem::setLoop(bool loop) {
    m_loop = loop;
}

bool AudioItem::isLoop() const {
    return m_loop;
}

void AudioItem::play() {
    m_playing = true;
    emit playRequested();
}

void AudioItem::stop() {
    m_playing = false;
    emit stopRequested();
}

bool AudioItem::isPlaying() const {
    return m_playing;
}

QString AudioItem::getType() const {
    return "Audio";
}
