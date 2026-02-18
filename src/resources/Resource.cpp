#include "resources/Resource.h"

// Resource base class
Resource::Resource(const std::string& url)
    : m_url(url)
    , m_state(State::Unloaded)
{
}

Resource::~Resource() {
    unload();
}

const std::string& Resource::getUrl() const {
    return m_url;
}

Resource::State Resource::getState() const {
    return m_state;
}

bool Resource::isLoaded() const {
    return m_state == State::Loaded;
}

void Resource::unload() {
    setState(State::Unloaded);
}

void Resource::setState(State state) {
    m_state = state;
    // TODO: Emit signal when Qt support is available
}

// TextureResource
TextureResource::TextureResource(const std::string& url)
    : Resource(url)
    , m_width(0)
    , m_height(0)
{
}

size_t TextureResource::getSize() const {
    if (!isLoaded()) return 0;
    // Assuming RGBA format (4 bytes per pixel)
    return m_width * m_height * 4;
}

void TextureResource::setDimensions(int width, int height) {
    m_width = width;
    m_height = height;
}

// AudioResource
AudioResource::AudioResource(const std::string& url)
    : Resource(url)
    , m_duration(0.0f)
{
}

size_t AudioResource::getSize() const {
    if (!isLoaded()) return 0;
    // Rough estimate: 44100 Hz * 2 channels * 2 bytes per sample * duration
    return static_cast<size_t>(44100 * 2 * 2 * m_duration);
}

void AudioResource::setDuration(float duration) {
    m_duration = duration;
}

// ChatSessionResource
ChatSessionResource::ChatSessionResource(const std::string& url)
    : Resource(url)
    , m_dataSize(0)
{
}

size_t ChatSessionResource::getSize() const {
    return m_dataSize;
}
