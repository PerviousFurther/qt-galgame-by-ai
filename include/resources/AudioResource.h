#ifndef INCLUDE_RESOURCES_AUDIORESOURCE_H
#define INCLUDE_RESOURCES_AUDIORESOURCE_H

#include "resources/Resource.h"

class AudioResource : public Resource {
public:
    explicit AudioResource(const QString& url);
    ~AudioResource() override = default;

    size_t getSize() const override;

    float getDuration() const;
    void setDuration(float duration);

private:
    float m_duration;
};

#endif // INCLUDE_RESOURCES_AUDIORESOURCE_H
