#ifndef RESOURCES_H
#define RESOURCES_H

#include "Resource.h"
#include "Loader.h"
#include <memory>
#include <vector>
#include <map>
#include <string>

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
    void registerLoader(std::shared_ptr<Loader> loader);

    /**
     * @brief Load a resource synchronously
     * @param url The resource URL (e.g., "file://image.png", "qrc:/audio/bgm.mp3")
     * @return Loaded resource, or cached resource if already loaded
     */
    std::shared_ptr<Resource> load(const std::string& url);

    /**
     * @brief Load a resource asynchronously
     * @param url The resource URL
     * @param callback Called when loading completes
     */
    void loadAsync(const std::string& url, LoadCallback callback);

    /**
     * @brief Get a cached resource if it exists
     * @param url The resource URL
     * @return Cached resource, or nullptr if not found
     */
    std::shared_ptr<Resource> get(const std::string& url) const;

    /**
     * @brief Check if a resource is cached
     * @param url The resource URL
     * @return true if cached, false otherwise
     */
    bool isCached(const std::string& url) const;

    /**
     * @brief Unload a resource from cache
     * @param url The resource URL
     * @return true if successful, false if not found
     */
    bool unload(const std::string& url);

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
    std::shared_ptr<Loader> findLoader(const std::string& url) const;

    std::vector<std::shared_ptr<Loader>> m_loaders;
    std::map<std::string, std::shared_ptr<Resource>> m_cache;
};

#endif // RESOURCES_H
