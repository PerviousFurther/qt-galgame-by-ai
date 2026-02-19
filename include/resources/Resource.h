#ifndef RESOURCE_H
#define RESOURCE_H

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
 * NOTE: Repository coding constraints are documented in BEFORECODING.md
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

Q_DECLARE_METATYPE(QSharedPointer<Resource>)

#endif // RESOURCE_H
