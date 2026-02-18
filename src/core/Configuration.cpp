#include "core/Configuration.h"
#include <fstream>
#include <iostream>
#include <sstream>

Configuration::Configuration() {
    setDefaults();
}

Configuration& Configuration::getInstance() {
    static Configuration instance;
    return instance;
}

void Configuration::setDefaults() {
    // Audio defaults
    setMasterVolume(1.0f);
    setMusicVolume(0.8f);
    setSoundEffectVolume(0.8f);
    setVoiceVolume(1.0f);

    // Window defaults
    setWindowWidth(1280);
    setWindowHeight(720);
    setFullscreen(false);

    // Render defaults
    setTargetFPS(60);
    setVSyncEnabled(true);
}

bool Configuration::loadFromFile(const std::string& filePath) {
    // TODO: Implement JSON parsing
    // For now, this is a stub that will be implemented when JSON library is added
    std::cout << "Loading configuration from: " << filePath << " (JSON parsing not yet implemented)" << std::endl;
    return false;
}

void Configuration::parseCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Parse --key=value format
        size_t pos = arg.find('=');
        if (pos != std::string::npos && arg.substr(0, 2) == "--") {
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            
            // Try to parse as different types
            try {
                // Check if it's a number
                if (value.find('.') != std::string::npos) {
                    setFloat(key, std::stof(value));
                } else {
                    setInt(key, std::stoi(value));
                }
            } catch (...) {
                // If not a number, treat as string
                if (value == "true" || value == "false") {
                    setBool(key, value == "true");
                } else {
                    setString(key, value);
                }
            }
        }
    }
}

bool Configuration::saveToFile(const std::string& filePath) const {
    // TODO: Implement JSON serialization
    std::cout << "Saving configuration to: " << filePath << " (JSON serialization not yet implemented)" << std::endl;
    return false;
}

// Audio settings
float Configuration::getMasterVolume() const {
    return getFloat("audio.master_volume", 1.0f);
}

void Configuration::setMasterVolume(float volume) {
    setFloat("audio.master_volume", volume);
}

float Configuration::getMusicVolume() const {
    return getFloat("audio.music_volume", 0.8f);
}

void Configuration::setMusicVolume(float volume) {
    setFloat("audio.music_volume", volume);
}

float Configuration::getSoundEffectVolume() const {
    return getFloat("audio.sound_effect_volume", 0.8f);
}

void Configuration::setSoundEffectVolume(float volume) {
    setFloat("audio.sound_effect_volume", volume);
}

float Configuration::getVoiceVolume() const {
    return getFloat("audio.voice_volume", 1.0f);
}

void Configuration::setVoiceVolume(float volume) {
    setFloat("audio.voice_volume", volume);
}

// Window settings
int Configuration::getWindowWidth() const {
    return getInt("window.width", 1280);
}

void Configuration::setWindowWidth(int width) {
    setInt("window.width", width);
}

int Configuration::getWindowHeight() const {
    return getInt("window.height", 720);
}

void Configuration::setWindowHeight(int height) {
    setInt("window.height", height);
}

bool Configuration::isFullscreen() const {
    return getBool("window.fullscreen", false);
}

void Configuration::setFullscreen(bool fullscreen) {
    setBool("window.fullscreen", fullscreen);
}

// Render settings
int Configuration::getTargetFPS() const {
    return getInt("render.target_fps", 60);
}

void Configuration::setTargetFPS(int fps) {
    setInt("render.target_fps", fps);
}

bool Configuration::isVSyncEnabled() const {
    return getBool("render.vsync", true);
}

void Configuration::setVSyncEnabled(bool enabled) {
    setBool("render.vsync", enabled);
}

// Generic configuration access
std::string Configuration::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = m_stringValues.find(key);
    return (it != m_stringValues.end()) ? it->second : defaultValue;
}

void Configuration::setString(const std::string& key, const std::string& value) {
    m_stringValues[key] = value;
}

int Configuration::getInt(const std::string& key, int defaultValue) const {
    auto it = m_intValues.find(key);
    return (it != m_intValues.end()) ? it->second : defaultValue;
}

void Configuration::setInt(const std::string& key, int value) {
    m_intValues[key] = value;
}

float Configuration::getFloat(const std::string& key, float defaultValue) const {
    auto it = m_floatValues.find(key);
    return (it != m_floatValues.end()) ? it->second : defaultValue;
}

void Configuration::setFloat(const std::string& key, float value) {
    m_floatValues[key] = value;
}

bool Configuration::getBool(const std::string& key, bool defaultValue) const {
    auto it = m_boolValues.find(key);
    return (it != m_boolValues.end()) ? it->second : defaultValue;
}

void Configuration::setBool(const std::string& key, bool value) {
    m_boolValues[key] = value;
}
