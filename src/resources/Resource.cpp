#include "codingstyle.h" // include/codingstyle.h
#include "resources/Resource.h"

#include <QReadLocker>
#include <QWriteLocker>

Resource::Resource(const QString& url)
    : m_url(url)
    , m_state(State::Unloaded)
    , m_object()
{
}

Resource::Resource(const Resource& other)
    : m_url()
    , m_state(State::Unloaded)
    , m_object()
{
    QWriteLocker lock(&other.m_lock);
    m_url = other.m_url;
    m_state = other.m_state;
    m_object = other.m_object;
}

Resource& Resource::operator=(const Resource& other) {
    if (this == &other) {
        return *this;
    }

    Resource* first = this < &other ? this : const_cast<Resource*>(&other);
    Resource* second = this < &other ? const_cast<Resource*>(&other) : this;
    first->m_lock.lockForWrite();
    second->m_lock.lockForWrite();

    m_url = other.m_url;
    m_state = other.m_state;
    m_object = other.m_object;

    second->m_lock.unlock();
    first->m_lock.unlock();
    return *this;
}

Resource::Resource(Resource&& other) noexcept
    : m_url()
    , m_state(State::Unloaded)
    , m_object()
{
    // Destination object is still under construction and not yet published to other threads.
    QWriteLocker lock(&other.m_lock);
    m_url = other.m_url;
    m_state = other.m_state;
    m_object = other.m_object;
}

Resource& Resource::operator=(Resource&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    Resource* first = this < &other ? this : &other;
    Resource* second = this < &other ? &other : this;
    first->m_lock.lockForWrite();
    second->m_lock.lockForWrite();

    m_url = other.m_url;
    m_state = other.m_state;
    m_object = other.m_object;

    second->m_lock.unlock();
    first->m_lock.unlock();
    return *this;
}

Resource::~Resource() {
    unload();
}

QString Resource::getUrl() const {
    QReadLocker lock(&m_lock);
    return m_url;
}

Resource::State Resource::getState() const {
    QReadLocker lock(&m_lock);
    return m_state;
}

bool Resource::isLoaded() const {
    QReadLocker lock(&m_lock);
    return m_state == State::Loaded;
}

void Resource::unload() {
    QWriteLocker lock(&m_lock);
    m_state = State::Unloaded;
    m_object.clear();
}

void Resource::setState(State state) {
    QWriteLocker lock(&m_lock);
    m_state = state;
}

QObject* Resource::get() const {
    QReadLocker lock(&m_lock);
    return m_object.data();
}

void Resource::set(QObject* object) {
    QWriteLocker lock(&m_lock);
    m_object.reset(object);
}

TextureResource::TextureResource(const QString& url)
    : Resource(url)
    , m_width(0)
    , m_height(0)
{
}

size_t TextureResource::getSize() const {
    QReadLocker lock(&m_lock);
    if (m_state != State::Loaded) {
        return 0;
    }
    return static_cast<size_t>(m_width * m_height * 4);
}

int TextureResource::getWidth() const {
    QReadLocker lock(&m_lock);
    return m_width;
}

int TextureResource::getHeight() const {
    QReadLocker lock(&m_lock);
    return m_height;
}

void TextureResource::setDimensions(int width, int height) {
    QWriteLocker lock(&m_lock);
    m_width = width;
    m_height = height;
}

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

ChatSessionResource::ChatSessionResource(const QString& url)
    : Resource(url)
    , m_dataSize(0)
{
}

size_t ChatSessionResource::getSize() const {
    QReadLocker lock(&m_lock);
    return m_dataSize;
}

void ChatSessionResource::setDataSize(size_t dataSize) {
    QWriteLocker lock(&m_lock);
    m_dataSize = dataSize;
}
