#include "resources/Resources.h"
#include <QDebug>

Resources::Resources() {
    registerDefaultLoaders();
}

Resources& Resources::getInstance() {
    static Resources instance;
    return instance;
}

void Resources::registerDefaultLoaders() {
    // Register default loaders for common protocols and formats
    registerLoader(QSharedPointer<FileImageLoader>::create());
    registerLoader(QSharedPointer<QrcImageLoader>::create());
    
    // TODO: Add more loaders as they are implemented
    // registerLoader(QSharedPointer<FileAudioLoader>::create());
    // registerLoader(QSharedPointer<FileVideoLoader>::create());
    // etc.
}

void Resources::registerLoader(QSharedPointer<Loader> loader) {
    if (loader) {
        m_loaders.append(loader);
    }
}

QSharedPointer<Loader> Resources::findLoader(const QString& url) const {
    for (const auto& loader : m_loaders) {
        if (loader->canLoad(url)) {
            return loader;
        }
    }
    return QSharedPointer<Loader>();
}

QSharedPointer<Resource> Resources::load(const QString& url) {
    // Check if already cached (thread-safe)
    {
        QMutexLocker lock(&m_cacheMutex);
        auto it = m_cache.find(url);
        if (it != m_cache.end()) {
            return it.value();
        }
    }

    // Find appropriate loader
    auto loader = findLoader(url);
    if (!loader) {
        qWarning() << "No loader found for:" << url;
        return QSharedPointer<Resource>();
    }

    // Load the resource
    auto resource = loader->loadSync(url);
    
    // Cache if successful (thread-safe)
    if (resource && resource->isLoaded()) {
        QMutexLocker lock(&m_cacheMutex);
        m_cache[url] = resource;
    }

    return resource;
}

void Resources::loadAsync(const QString& url, LoadCallback callback) {
    // Check if already cached (thread-safe)
    {
        QMutexLocker lock(&m_cacheMutex);
        auto it = m_cache.find(url);
        if (it != m_cache.end()) {
            if (callback) {
                callback(it.value(), true);
            }
            return;
        }
    }

    // Find appropriate loader
    auto loader = findLoader(url);
    if (!loader) {
        qWarning() << "No loader found for:" << url;
        if (callback) {
            callback(QSharedPointer<Resource>(), false);
        }
        return;
    }

    // Load asynchronously
    QMutex* cacheMutexPtr = &m_cacheMutex;
    QMap<QString, QSharedPointer<Resource>>* cachePtr = &m_cache;
    
    loader->loadAsync(url, [cachePtr, cacheMutexPtr, url, callback](QSharedPointer<Resource> resource, bool success) {
        // Cache if successful (thread-safe)
        if (success && resource && resource->isLoaded()) {
            QMutexLocker lock(cacheMutexPtr);
            (*cachePtr)[url] = resource;
        }
        
        // Call user callback
        if (callback) {
            callback(resource, success);
        }
    });
}

QSharedPointer<Resource> Resources::get(const QString& url) const {
    QMutexLocker lock(&m_cacheMutex);
    auto it = m_cache.find(url);
    if (it != m_cache.end()) {
        return it.value();
    }
    return QSharedPointer<Resource>();
}

bool Resources::isCached(const QString& url) const {
    QMutexLocker lock(&m_cacheMutex);
    return m_cache.contains(url);
}

bool Resources::unload(const QString& url) {
    QMutexLocker lock(&m_cacheMutex);
    auto it = m_cache.find(url);
    if (it == m_cache.end()) {
        return false;
    }

    // Unload the resource
    if (it.value()) {
        it.value()->unload();
    }

    m_cache.erase(it);
    return true;
}

void Resources::clearCache() {
    QMutexLocker lock(&m_cacheMutex);
    // Unload all resources
    for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
        if (it.value()) {
            it.value()->unload();
        }
    }
    m_cache.clear();
}

size_t Resources::getCacheSize() const {
    QMutexLocker lock(&m_cacheMutex);
    size_t totalSize = 0;
    for (auto it = m_cache.constBegin(); it != m_cache.constEnd(); ++it) {
        if (it.value()) {
            totalSize += it.value()->getSize();
        }
    }
    return totalSize;
}
