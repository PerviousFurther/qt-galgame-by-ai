#ifndef INCLUDE_RESOURCES_JSONRESOURCE_H
#define INCLUDE_RESOURCES_JSONRESOURCE_H
#include "codingstyle.h" // include/codingstyle.h

#include "resources/Resource.h"

class JsonResource : public Resource {
public:
    explicit JsonResource(const QString& url);
    ~JsonResource() override = default;

    size_t getSize() const override;
    void setDataSize(size_t dataSize);

private:
    size_t m_dataSize;
};

#endif // INCLUDE_RESOURCES_JSONRESOURCE_H
