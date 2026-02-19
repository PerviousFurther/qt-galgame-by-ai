#ifndef RESOURCE_H
#define RESOURCE_H
#include "codingstyle.h" // include/codingstyle.h

#include <QString>
#include <QReadWriteLock>
#include <QObject>
#include <QSharedPointer>

/**
 * @brief Base class for loaded resources
 * 
 * Resource represents data that has been loaded into memory and is ready to use.
 * Different resource types (Texture, Audio, etc.) inherit from this class.
 * 
 * Resources emit signals when they are loaded or unloaded, allowing
 * Items to respond to resource state changes.
 * 
 * NOTE: Uses Qt containers - see CodingStandards.h
 */
class Resource {
public:
    enum class State {
        Unloaded,   // Not yet loaded
        Loading,    // Currently being loaded (async)
        Loaded,     // Successfully loaded and ready
        Failed      // Loading failed
    };

    Resource(const QString& url);
    Resource(const Resource& other);
    Resource& operator=(const Resource& other);
    Resource(Resource&& other) noexcept;
    Resource& operator=(Resource&& other) noexcept;
    virtual ~Resource();

    /**
     * @brief Get the resource URL
     * @return The URL/path of the resource
     * @note Returns by value to keep thread-safe copy semantics.
     */
    QString getUrl() const;

    /**
     * @brief Get the current state of the resource
     * @return Current resource state
     */
    State getState() const;

    /**
     * @brief Check if the resource is loaded and ready to use
     * @return true if loaded, false otherwise
     */
    bool isLoaded() const;

    /**
     * @brief Unload the resource from memory
     */
    virtual void unload();

    /**
     * @brief Get the size of the resource in bytes
     * @return Resource size, or 0 if not loaded
     */
    virtual size_t getSize() const = 0;

    /**
     * @brief Set the resource state (used by loaders)
     * @param state New state
     */
    void setState(State state);

    QObject* get() const;
    void set(QObject* object);

protected:
    QString m_url;
    State m_state;
    QSharedPointer<QObject> m_object;
    mutable QReadWriteLock m_lock;
};

/**
 * @brief Texture resource for images
 */
class TextureResource : public Resource {
public:
    TextureResource(const QString& url);
    virtual ~TextureResource() = default;

    size_t getSize() const override;

    int getWidth() const;
    int getHeight() const;
    void setDimensions(int width, int height);

    // TODO: Add actual texture data storage (e.g., OpenGL texture ID, raw pixel data)

private:
    int m_width;
    int m_height;
};

/**
 * @brief Audio resource for sound effects, music, and voice
 */
class AudioResource : public Resource {
public:
    AudioResource(const QString& url);
    virtual ~AudioResource() = default;

    size_t getSize() const override;

    float getDuration() const;
    void setDuration(float duration);

    // TODO: Add actual audio data storage

private:
    float m_duration;  // Duration in seconds
};

/**
 * @brief Chat/Dialog session resource
 * 
 * Represents dialog scripts, conversation trees, or chat sessions
 * typically loaded from JSON or other structured formats.
 */
class ChatSessionResource : public Resource {
public:
    ChatSessionResource(const QString& url);
    virtual ~ChatSessionResource() = default;

    size_t getSize() const override;
    void setDataSize(size_t dataSize);

    // TODO: Add dialog tree/chat session data structures

private:
    size_t m_dataSize;
};

Q_DECLARE_METATYPE(QSharedPointer<Resource>)

#endif // RESOURCE_H
