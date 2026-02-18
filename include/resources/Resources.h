#ifndef RESOURCES_H
#define RESOURCES_H

#include "Resource.h"
#include "Loader.h"
#include <QSharedPointer>
#include <QList>
#include <QMap>
#include <QString>
#include <QMutex>

/**
 * @brief Resources singleton for managing resource loading
 * 
 * Resources maintains a registry of Loaders and handles loading resources
 * from various sources (file://, qrc://, etc.) in various formats
 * (images, audio, dialogs, etc.).
 * 
 * Resources are cached after loading to avoid redundant loads.
 * Supports both synchronous and asynchronous loading.
 * 
 * Usage:
 * @code
 * // Load synchronously
 * auto texture = Resources::getInstance().load("file://textures/character.png");
 * 
 * // Load asynchronously
 * Resources::getInstance().loadAsync("file://audio/bgm.mp3", 
 *     [](auto resource, bool success) {
 *         if (success) {
 *             // Use the loaded resource
 *         }
 *     });
 * 
 * // Get cached resource
 * auto cached = Resources::getInstance().get("file://textures/character.png");
 * @endcode
 */
class Resources {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the Resources singleton
     */
    static Resources& getInstance();

    /**
     * @brief Register a loader for handling specific resource types
     * @param loader Shared pointer to the Loader
     */
    void registerLoader(QSharedPointer<Loader> loader);

    /**
     * @brief Load a resource synchronously
     * @param url The resource URL (e.g., "file://image.png", "qrc:/audio/bgm.mp3")
     * @return Loaded resource, or cached resource if already loaded
     */
    QSharedPointer<Resource> load(const QString& url);

    /**
     * @brief Load a resource asynchronously
     * @param url The resource URL
     * @param callback Called when loading completes
     */
    void loadAsync(const QString& url, LoadCallback callback);

    /**
     * @brief Get a cached resource if it exists
     * @param url The resource URL
     * @return Cached resource, or nullptr if not found
     */
    QSharedPointer<Resource> get(const QString& url) const;

    /**
     * @brief Check if a resource is cached
     * @param url The resource URL
     * @return true if cached, false otherwise
     */
    bool isCached(const QString& url) const;

    /**
     * @brief Unload a resource from cache
     * @param url The resource URL
     * @return true if successful, false if not found
     */
    bool unload(const QString& url);

    /**
     * @brief Clear all cached resources
     */
    void clearCache();

    /**
     * @brief Get total memory used by cached resources
     * @return Total size in bytes
     */
    size_t getCacheSize() const;

private:
    Resources();
    ~Resources() = default;
    Resources(const Resources&) = delete;
    Resources& operator=(const Resources&) = delete;

    // Register default loaders
    void registerDefaultLoaders();

    // Find a loader that can handle the URL
    QSharedPointer<Loader> findLoader(const QString& url) const;

    QList<QSharedPointer<Loader>> m_loaders;
    QMap<QString, QSharedPointer<Resource>> m_cache;
    mutable QMutex m_cacheMutex;  // Protects cache access from multiple threads
};

#endif // RESOURCES_H
