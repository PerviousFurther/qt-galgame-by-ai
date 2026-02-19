#include "resources/TextureResource.h"

#include <QReadLocker>
#include <QWriteLocker>

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
