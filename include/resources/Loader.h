#ifndef LOADER_H
#define LOADER_H

#include "Resource.h"
#include <memory>
#include <string>
#include <functional>

/**
 * @brief Callback type for async loading completion
 * @param resource The loaded resource (may be nullptr if loading failed)
 * @param success Whether loading was successful
 */
using LoadCallback = std::function<void(std::shared_ptr<Resource>, bool)>;

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
    virtual bool canLoad(const std::string& url) const = 0;

    /**
     * @brief Load a resource synchronously
     * @param url The resource URL
     * @return Loaded resource, or nullptr if loading failed
     */
    virtual std::shared_ptr<Resource> loadSync(const std::string& url) = 0;

    /**
     * @brief Load a resource asynchronously
     * @param url The resource URL
     * @param callback Called when loading completes (or fails)
     */
    virtual void loadAsync(const std::string& url, LoadCallback callback) = 0;

protected:
    /**
     * @brief Extract protocol from URL (e.g., "file", "qrc")
     * @param url The URL to parse
     * @return Protocol string, or "file" if no protocol specified
     */
    static std::string getProtocol(const std::string& url);

    /**
     * @brief Extract file extension from URL
     * @param url The URL to parse
     * @return File extension (lowercase, without dot)
     */
    static std::string getExtension(const std::string& url);
};

/**
 * @brief Base class for loaders that support the file:// protocol
 */
class FileLoader : public virtual Loader {
public:
    bool canLoad(const std::string& url) const override;

protected:
    /**
     * @brief Open a file and read its contents
     * @param path File path (protocol already stripped)
     * @param outData Output buffer for file data
     * @param outSize Output variable for data size
     * @return true if successful
     */
    bool readFile(const std::string& path, std::vector<char>& outData) const;
};

/**
 * @brief Base class for loaders that support the qrc:// protocol (Qt resources)
 */
class QrcLoader : public virtual Loader {
public:
    bool canLoad(const std::string& url) const override;

protected:
    /**
     * @brief Read data from Qt resource system
     * @param path Resource path (protocol already stripped)
     * @param outData Output buffer for resource data
     * @return true if successful
     */
    bool readQrcResource(const std::string& path, std::vector<char>& outData) const;
};

/**
 * @brief Loader for image files from file:// protocol
 */
class FileImageLoader : public FileLoader {
public:
    bool canLoad(const std::string& url) const override;
    std::shared_ptr<Resource> loadSync(const std::string& url) override;
    void loadAsync(const std::string& url, LoadCallback callback) override;

private:
    bool isImageExtension(const std::string& ext) const;
};

/**
 * @brief Loader for image files from qrc:// protocol
 */
class QrcImageLoader : public QrcLoader {
public:
    bool canLoad(const std::string& url) const override;
    std::shared_ptr<Resource> loadSync(const std::string& url) override;
    void loadAsync(const std::string& url, LoadCallback callback) override;

private:
    bool isImageExtension(const std::string& ext) const;
};

#endif // LOADER_H
