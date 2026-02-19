#ifndef INCLUDE_RESOURCES_MEDIARESOURCE_H
#define INCLUDE_RESOURCES_MEDIARESOURCE_H

#include "resources/Resource.h"

class MediaResource : public Resource {
public:
    explicit MediaResource(const QString& url);
    ~MediaResource() override = default;

    size_t getSize() const override;
    void setDataSize(size_t dataSize);

private:
    size_t m_dataSize;
};

#endif // INCLUDE_RESOURCES_MEDIARESOURCE_H
