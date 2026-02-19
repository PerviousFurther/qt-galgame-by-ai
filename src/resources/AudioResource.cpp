#include "codingstyle.h" // include/codingstyle.h
#include "resources/AudioResource.h"

#include <QReadLocker>
#include <QWriteLocker>

AudioResource::AudioResource(const QString& url)
    : Resource(url)
    , m_duration(0.0f)
{
}

size_t AudioResource::getSize() const {
    QReadLocker lock(&m_lock);
    if (m_state != State::Loaded) {
        return 0;
    }
    return static_cast<size_t>(44100 * 2 * 2 * m_duration);
}

float AudioResource::getDuration() const {
    QReadLocker lock(&m_lock);
    return m_duration;
}

void AudioResource::setDuration(float duration) {
    QWriteLocker lock(&m_lock);
    m_duration = duration;
}
