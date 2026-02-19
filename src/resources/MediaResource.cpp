#include "resources/MediaResource.h"

#include <QReadLocker>
#include <QWriteLocker>

MediaResource::MediaResource(const QString& url)
    : Resource(url)
    , m_dataSize(0)
{
}

size_t MediaResource::getSize() const {
    QReadLocker lock(&m_lock);
    return m_dataSize;
}

void MediaResource::setDataSize(size_t dataSize) {
    QWriteLocker lock(&m_lock);
    m_dataSize = dataSize;
}
