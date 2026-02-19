#include "codingstyle.h" // include/codingstyle.h
#include "core/Configuration.h"
#include <QDebug>

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

bool Configuration::loadFromFile(const QString& filePath) {
    // TODO: Implement JSON parsing
    // For now, this is a stub that will be implemented when JSON library is added
    qDebug() << "Loading configuration from:" << filePath << "(JSON parsing not yet implemented)";
    return false;
}

void Configuration::parseCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        const QString arg = QString::fromUtf8(argv[i]);
        
        // Parse --key=value format
        const int pos = arg.indexOf('=');
        if (pos >= 0 && arg.startsWith("--")) {
            const QString key = arg.mid(2, pos - 2);
            const QString value = arg.mid(pos + 1);
            
            // Try to parse as different types
            bool converted = false;
            const int intValue = value.toInt(&converted);
            if (converted) {
                setInt(key, intValue);
            } else {
                const float floatValue = value.toFloat(&converted);
                if (converted) {
                    setFloat(key, floatValue);
                }
            }

            if (!converted) {
                if (value == "true" || value == "false") {
                    setBool(key, value == "true");
                } else {
                    setString(key, value);
                }
            }
        }
    }
}

bool Configuration::saveToFile(const QString& filePath) const {
    // TODO: Implement JSON serialization
    qDebug() << "Saving configuration to:" << filePath << "(JSON serialization not yet implemented)";
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
QString Configuration::getString(const QString& key, const QString& defaultValue) const {
    return m_stringValues.value(key, defaultValue);
}

void Configuration::setString(const QString& key, const QString& value) {
    m_stringValues[key] = value;
}

int Configuration::getInt(const QString& key, int defaultValue) const {
    return m_intValues.value(key, defaultValue);
}

void Configuration::setInt(const QString& key, int value) {
    m_intValues[key] = value;
}

float Configuration::getFloat(const QString& key, float defaultValue) const {
    return m_floatValues.value(key, defaultValue);
}

void Configuration::setFloat(const QString& key, float value) {
    m_floatValues[key] = value;
}

bool Configuration::getBool(const QString& key, bool defaultValue) const {
    return m_boolValues.value(key, defaultValue);
}

void Configuration::setBool(const QString& key, bool value) {
    m_boolValues[key] = value;
}
