#ifndef LOADER_H
#define LOADER_H

#include "Resource.h"
#include <QSharedPointer>
#include <QString>
#include <QByteArray>
#include <functional>

/**
 * @brief Callback type for async loading completion
 * @param resource The loaded resource (may be nullptr if loading failed)
 * @param success Whether loading was successful
 */
using LoadCallback = std::function<void(QSharedPointer<Resource>, bool)>;

/**
 * @brief Abstract base class for resource loaders
 * 
 * Loaders handle loading resources from different protocols (file://, qrc://, etc.)
 * and file formats (images, audio, etc.). They support both synchronous and
 * asynchronous loading.
 * 
 * Loaders can be specialized through multiple inheritance to handle combinations
 * of protocols and formats:
 * - FileImageLoader (file:// + images)
 * - QrcImageLoader (qrc:// + images)
 * - FileAudioLoader (file:// + audio)
 * - etc.
 */
class Loader {
public:
    virtual ~Loader() = default;

    /**
     * @brief Check if this loader can handle the given URL
     * @param url The resource URL
     * @return true if this loader supports the URL's protocol and format
     */
    virtual bool canLoad(const QString& url) const = 0;

    /**
     * @brief Load a resource synchronously
     * @param url The resource URL
     * @return Loaded resource, or nullptr if loading failed
     */
    virtual QSharedPointer<Resource> loadSync(const QString& url) = 0;

    /**
     * @brief Load a resource asynchronously
     * @param url The resource URL
     * @param callback Called when loading completes (or fails)
     */
    virtual void loadAsync(const QString& url, LoadCallback callback) = 0;

protected:
    /**
     * @brief Extract protocol from URL (e.g., "file", "qrc")
     * @param url The URL to parse
     * @return Protocol string, or "file" if no protocol specified
     */
    static QString getProtocol(const QString& url);

    /**
     * @brief Extract file extension from URL
     * @param url The URL to parse
     * @return File extension (lowercase, without dot)
     */
    static QString getExtension(const QString& url);
};

/**
 * @brief Base class for loaders that support the file:// protocol
 */
class FileLoader : public virtual Loader {
public:
    bool canLoad(const QString& url) const override;

protected:
    /**
     * @brief Open a file and read its contents
     * @param path File path (protocol already stripped)
     * @param outData Output buffer for file data
     * @return true if successful
     */
    bool readFile(const QString& path, QByteArray& outData) const;
};

/**
 * @brief Base class for loaders that support the qrc:// protocol (Qt resources)
 */
class QrcLoader : public virtual Loader {
public:
    bool canLoad(const QString& url) const override;

protected:
    /**
     * @brief Read data from Qt resource system
     * @param path Resource path (protocol already stripped)
     * @param outData Output buffer for resource data
     * @return true if successful
     */
    bool readQrcResource(const QString& path, QByteArray& outData) const;
};

/**
 * @brief Loader for image files from file:// protocol
 */
class FileImageLoader : public FileLoader {
public:
    bool canLoad(const QString& url) const override;
    QSharedPointer<Resource> loadSync(const QString& url) override;
    void loadAsync(const QString& url, LoadCallback callback) override;

private:
    bool isImageExtension(const QString& ext) const;
};

/**
 * @brief Loader for image files from qrc:// protocol
 */
class QrcImageLoader : public QrcLoader {
public:
    bool canLoad(const QString& url) const override;
    QSharedPointer<Resource> loadSync(const QString& url) override;
    void loadAsync(const QString& url, LoadCallback callback) override;

private:
    bool isImageExtension(const QString& ext) const;
};

#endif // LOADER_H
