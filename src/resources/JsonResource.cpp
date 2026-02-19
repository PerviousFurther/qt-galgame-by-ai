#include "resources/JsonResource.h"

#include <QReadLocker>
#include <QWriteLocker>

JsonResource::JsonResource(const QString& url)
    : Resource(url)
    , m_dataSize(0)
{
}

size_t JsonResource::getSize() const {
    QReadLocker lock(&m_lock);
    return m_dataSize;
}

void JsonResource::setDataSize(size_t dataSize) {
    QWriteLocker lock(&m_lock);
    m_dataSize = dataSize;
}
