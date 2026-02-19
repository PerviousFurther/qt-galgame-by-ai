#ifndef INCLUDE_RESOURCES_QMLRESOURCE_H
#define INCLUDE_RESOURCES_QMLRESOURCE_H

#include "resources/Resource.h"

class QmlResource : public Resource {
public:
    explicit QmlResource(const QString& url);
    ~QmlResource() override = default;

    size_t getSize() const override;
    void setDataSize(size_t dataSize);

private:
    size_t m_dataSize;
};

#endif // INCLUDE_RESOURCES_QMLRESOURCE_H
