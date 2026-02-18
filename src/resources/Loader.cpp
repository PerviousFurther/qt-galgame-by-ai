#include "resources/Loader.h"
#include <algorithm>
#include <fstream>
#include <thread>
#include <iostream>

// Loader base class
std::string Loader::getProtocol(const std::string& url) {
    size_t pos = url.find("://");
    if (pos != std::string::npos) {
        return url.substr(0, pos);
    }
    return "file";  // Default to file protocol
}

std::string Loader::getExtension(const std::string& url) {
    size_t pos = url.find_last_of('.');
    if (pos != std::string::npos && pos < url.length() - 1) {
        std::string ext = url.substr(pos + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }
    return "";
}

// FileLoader
bool FileLoader::canLoad(const std::string& url) const {
    std::string protocol = getProtocol(url);
    return protocol == "file" || url.find("://") == std::string::npos;
}

bool FileLoader::readFile(const std::string& path, std::vector<char>& outData) const {
    // Strip file:// protocol if present
    std::string filePath = path;
    if (filePath.substr(0, 7) == "file://") {
        filePath = filePath.substr(7);
    }

    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    outData.resize(static_cast<size_t>(size));
    if (!file.read(outData.data(), size)) {
        return false;
    }

    return true;
}

// QrcLoader
bool QrcLoader::canLoad(const std::string& url) const {
    return getProtocol(url) == "qrc";
}

bool QrcLoader::readQrcResource(const std::string& path, std::vector<char>& outData) const {
    // TODO: Implement Qt resource system reading when Qt is fully integrated
    std::cout << "QRC loading not yet implemented: " << path << std::endl;
    return false;
}

// FileImageLoader
bool FileImageLoader::isImageExtension(const std::string& ext) const {
    return ext == "png" || ext == "jpg" || ext == "jpeg" || 
           ext == "bmp" || ext == "gif" || ext == "webp";
}

bool FileImageLoader::canLoad(const std::string& url) const {
    if (!FileLoader::canLoad(url)) {
        return false;
    }
    return isImageExtension(getExtension(url));
}

std::shared_ptr<Resource> FileImageLoader::loadSync(const std::string& url) {
    auto resource = std::make_shared<TextureResource>(url);
    resource->setState(Resource::State::Loading);

    std::vector<char> data;
    if (readFile(url, data)) {
        // TODO: Actual image decoding (using stb_image or similar)
        // For now, just mark as loaded with dummy dimensions
        resource->setDimensions(100, 100);
        resource->setState(Resource::State::Loaded);
        std::cout << "Loaded image: " << url << " (" << data.size() << " bytes)" << std::endl;
    } else {
        resource->setState(Resource::State::Failed);
        std::cerr << "Failed to load image: " << url << std::endl;
    }

    return resource;
}

void FileImageLoader::loadAsync(const std::string& url, LoadCallback callback) {
    // Launch async loading in a separate thread
    std::thread([this, url, callback]() {
        auto resource = loadSync(url);
        bool success = resource && resource->isLoaded();
        if (callback) {
            callback(resource, success);
        }
    }).detach();
}

// QrcImageLoader
bool QrcImageLoader::isImageExtension(const std::string& ext) const {
    return ext == "png" || ext == "jpg" || ext == "jpeg" || 
           ext == "bmp" || ext == "gif" || ext == "webp";
}

bool QrcImageLoader::canLoad(const std::string& url) const {
    if (!QrcLoader::canLoad(url)) {
        return false;
    }
    return isImageExtension(getExtension(url));
}

std::shared_ptr<Resource> QrcImageLoader::loadSync(const std::string& url) {
    auto resource = std::make_shared<TextureResource>(url);
    resource->setState(Resource::State::Loading);

    std::vector<char> data;
    if (readQrcResource(url, data)) {
        // TODO: Actual image decoding
        resource->setDimensions(100, 100);
        resource->setState(Resource::State::Loaded);
    } else {
        resource->setState(Resource::State::Failed);
        std::cerr << "Failed to load QRC image: " << url << std::endl;
    }

    return resource;
}

void QrcImageLoader::loadAsync(const std::string& url, LoadCallback callback) {
    std::thread([this, url, callback]() {
        auto resource = loadSync(url);
        bool success = resource && resource->isLoaded();
        if (callback) {
            callback(resource, success);
        }
    }).detach();
}
