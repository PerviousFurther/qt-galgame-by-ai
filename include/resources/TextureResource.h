#ifndef INCLUDE_RESOURCES_TEXTURERESOURCE_H
#define INCLUDE_RESOURCES_TEXTURERESOURCE_H

#include "resources/Resource.h"

class TextureResource : public Resource {
public:
    explicit TextureResource(const QString& url);
    ~TextureResource() override = default;

    size_t getSize() const override;

    int getWidth() const;
    int getHeight() const;
    void setDimensions(int width, int height);

private:
    int m_width;
    int m_height;
};

#endif // INCLUDE_RESOURCES_TEXTURERESOURCE_H
