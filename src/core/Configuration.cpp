#include "core/Configuration.h"
#include <QFile>
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
        QString arg = QString::fromLocal8Bit(argv[i]);
        
        // Parse --key=value format
        int pos = arg.indexOf(QLatin1Char('='));
        if (pos != -1 && arg.startsWith(QLatin1String("--"))) {
            QString key = arg.mid(2, pos - 2);
            QString value = arg.mid(pos + 1);
            
            // Try to parse as different types
            bool ok = false;
            if (value.contains(QLatin1Char('.'))) {
                float fval = value.toFloat(&ok);
                if (ok) {
                    setFloat(key, fval);
                    continue;
                }
            } else {
                int ival = value.toInt(&ok);
                if (ok) {
                    setInt(key, ival);
                    continue;
                }
            }
            
            // If not a number, treat as string or bool
            if (value == QLatin1String("true") || value == QLatin1String("false")) {
                setBool(key, value == QLatin1String("true"));
            } else {
                setString(key, value);
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
    return getFloat(QStringLiteral("audio.master_volume"), 1.0f);
}

void Configuration::setMasterVolume(float volume) {
    setFloat(QStringLiteral("audio.master_volume"), volume);
}

float Configuration::getMusicVolume() const {
    return getFloat(QStringLiteral("audio.music_volume"), 0.8f);
}

void Configuration::setMusicVolume(float volume) {
    setFloat(QStringLiteral("audio.music_volume"), volume);
}

float Configuration::getSoundEffectVolume() const {
    return getFloat(QStringLiteral("audio.sound_effect_volume"), 0.8f);
}

void Configuration::setSoundEffectVolume(float volume) {
    setFloat(QStringLiteral("audio.sound_effect_volume"), volume);
}

float Configuration::getVoiceVolume() const {
    return getFloat(QStringLiteral("audio.voice_volume"), 1.0f);
}

void Configuration::setVoiceVolume(float volume) {
    setFloat(QStringLiteral("audio.voice_volume"), volume);
}

// Window settings
int Configuration::getWindowWidth() const {
    return getInt(QStringLiteral("window.width"), 1280);
}

void Configuration::setWindowWidth(int width) {
    setInt(QStringLiteral("window.width"), width);
}

int Configuration::getWindowHeight() const {
    return getInt(QStringLiteral("window.height"), 720);
}

void Configuration::setWindowHeight(int height) {
    setInt(QStringLiteral("window.height"), height);
}

bool Configuration::isFullscreen() const {
    return getBool(QStringLiteral("window.fullscreen"), false);
}

void Configuration::setFullscreen(bool fullscreen) {
    setBool(QStringLiteral("window.fullscreen"), fullscreen);
}

// Render settings
int Configuration::getTargetFPS() const {
    return getInt(QStringLiteral("render.target_fps"), 60);
}

void Configuration::setTargetFPS(int fps) {
    setInt(QStringLiteral("render.target_fps"), fps);
}

bool Configuration::isVSyncEnabled() const {
    return getBool(QStringLiteral("render.vsync"), true);
}

void Configuration::setVSyncEnabled(bool enabled) {
    setBool(QStringLiteral("render.vsync"), enabled);
}

// Generic configuration access
QString Configuration::getString(const QString& key, const QString& defaultValue) const {
    auto it = m_stringValues.find(key);
    return (it != m_stringValues.end()) ? it.value() : defaultValue;
}

void Configuration::setString(const QString& key, const QString& value) {
    m_stringValues[key] = value;
}

int Configuration::getInt(const QString& key, int defaultValue) const {
    auto it = m_intValues.find(key);
    return (it != m_intValues.end()) ? it.value() : defaultValue;
}

void Configuration::setInt(const QString& key, int value) {
    m_intValues[key] = value;
}

float Configuration::getFloat(const QString& key, float defaultValue) const {
    auto it = m_floatValues.find(key);
    return (it != m_floatValues.end()) ? it.value() : defaultValue;
}

void Configuration::setFloat(const QString& key, float value) {
    m_floatValues[key] = value;
}

bool Configuration::getBool(const QString& key, bool defaultValue) const {
    auto it = m_boolValues.find(key);
    return (it != m_boolValues.end()) ? it.value() : defaultValue;
}

void Configuration::setBool(const QString& key, bool value) {
    m_boolValues[key] = value;
}
