#include "codingstyle.h" // include/codingstyle.h
#include "scene/VideoItem.h"

VideoItem::VideoItem(QObject* parent)
    : Item(parent)
    , m_loop(false)
    , m_playing(false)
{
}

void VideoItem::setSource(const QString& source) {
    m_source = source;
}

const QString& VideoItem::getSource() const {
    return m_source;
}

void VideoItem::setLoop(bool loop) {
    m_loop = loop;
}

bool VideoItem::isLoop() const {
    return m_loop;
}

void VideoItem::play() {
    m_playing = true;
    emit playRequested();
}

void VideoItem::stop() {
    m_playing = false;
    emit stopRequested();
}

bool VideoItem::isPlaying() const {
    return m_playing;
}

QString VideoItem::getType() const {
    return "Video";
}
