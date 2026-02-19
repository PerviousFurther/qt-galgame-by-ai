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
