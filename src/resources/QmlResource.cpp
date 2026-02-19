#include "codingstyle.h" // include/codingstyle.h
#include "resources/QmlResource.h"

#include <QReadLocker>
#include <QWriteLocker>

QmlResource::QmlResource(const QString& url)
    : Resource(url)
    , m_dataSize(0)
{
}

size_t QmlResource::getSize() const {
    QReadLocker lock(&m_lock);
    return m_dataSize;
}

void QmlResource::setDataSize(size_t dataSize) {
    QWriteLocker lock(&m_lock);
    m_dataSize = dataSize;
}
