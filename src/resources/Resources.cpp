#include "resources/Resources.h"
#include <iostream>

Resources::Resources() {
    registerDefaultLoaders();
}

Resources& Resources::getInstance() {
    static Resources instance;
    return instance;
}

void Resources::registerDefaultLoaders() {
    // Register default loaders for common protocols and formats
    registerLoader(std::make_shared<FileImageLoader>());
    registerLoader(std::make_shared<QrcImageLoader>());
    
    // TODO: Add more loaders as they are implemented
    // registerLoader(std::make_shared<FileAudioLoader>());
    // registerLoader(std::make_shared<FileVideoLoader>());
    // etc.
}

void Resources::registerLoader(std::shared_ptr<Loader> loader) {
    if (loader) {
        m_loaders.push_back(loader);
    }
}

std::shared_ptr<Loader> Resources::findLoader(const std::string& url) const {
    for (const auto& loader : m_loaders) {
        if (loader->canLoad(url)) {
            return loader;
        }
    }
    return nullptr;
}

std::shared_ptr<Resource> Resources::load(const std::string& url) {
    // Check if already cached
    auto it = m_cache.find(url);
    if (it != m_cache.end()) {
        return it->second;
    }

    // Find appropriate loader
    auto loader = findLoader(url);
    if (!loader) {
        std::cerr << "No loader found for: " << url << std::endl;
        return nullptr;
    }

    // Load the resource
    auto resource = loader->loadSync(url);
    
    // Cache if successful
    if (resource && resource->isLoaded()) {
        m_cache[url] = resource;
    }

    return resource;
}

void Resources::loadAsync(const std::string& url, LoadCallback callback) {
    // Check if already cached
    auto it = m_cache.find(url);
    if (it != m_cache.end()) {
        if (callback) {
            callback(it->second, true);
        }
        return;
    }

    // Find appropriate loader
    auto loader = findLoader(url);
    if (!loader) {
        std::cerr << "No loader found for: " << url << std::endl;
        if (callback) {
            callback(nullptr, false);
        }
        return;
    }

    // Load asynchronously
    loader->loadAsync(url, [this, url, callback](std::shared_ptr<Resource> resource, bool success) {
        // Cache if successful
        if (success && resource && resource->isLoaded()) {
            m_cache[url] = resource;
        }
        
        // Call user callback
        if (callback) {
            callback(resource, success);
        }
    });
}

std::shared_ptr<Resource> Resources::get(const std::string& url) const {
    auto it = m_cache.find(url);
    if (it != m_cache.end()) {
        return it->second;
    }
    return nullptr;
}

bool Resources::isCached(const std::string& url) const {
    return m_cache.find(url) != m_cache.end();
}

bool Resources::unload(const std::string& url) {
    auto it = m_cache.find(url);
    if (it == m_cache.end()) {
        return false;
    }

    // Unload the resource
    if (it->second) {
        it->second->unload();
    }

    m_cache.erase(it);
    return true;
}

void Resources::clearCache() {
    // Unload all resources
    for (auto& pair : m_cache) {
        if (pair.second) {
            pair.second->unload();
        }
    }
    m_cache.clear();
}

size_t Resources::getCacheSize() const {
    size_t totalSize = 0;
    for (const auto& pair : m_cache) {
        if (pair.second) {
            totalSize += pair.second->getSize();
        }
    }
    return totalSize;
}
