#include "core/Configuration.h"
#include <QDebug>
#include <QThread>

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

    // Execution defaults
    setInt("execution.max_threads", QThread::idealThreadCount());

    // Application bootstrap defaults
    setApplicationName("qt-galgame-by-ai");
    setConfigResourceUrl("qrc:/config.json");
    setStartupSceneUrl("qrc:/scene.qml");
    setGameLoopIntervalMs(16);  // ~60 FPS (1000ms / 60 ≈ 16.67ms)
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
                setValue(key, intValue);
            } else {
                const float floatValue = value.toFloat(&converted);
                if (converted) {
                    setValue(key, floatValue);
                }
            }

            if (!converted) {
                if (value == "true" || value == "false") {
                    setValue(key, value == "true");
                } else {
                    setValue(key, value);
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

QString Configuration::getApplicationName() const {
    return getString("app.name", "qt-galgame-by-ai");
}

void Configuration::setApplicationName(const QString& appName) {
    setString("app.name", appName);
}

QString Configuration::getConfigResourceUrl() const {
    return getString("app.config_resource_url", "qrc:/config.json");
}

void Configuration::setConfigResourceUrl(const QString& resourceUrl) {
    setString("app.config_resource_url", resourceUrl);
}

QString Configuration::getStartupSceneUrl() const {
    return getString("app.startup_scene_url", "qrc:/scene.qml");
}

void Configuration::setStartupSceneUrl(const QString& sceneUrl) {
    setString("app.startup_scene_url", sceneUrl);
}

int Configuration::getGameLoopIntervalMs() const {
    return getInt("app.game_loop_interval_ms", 16);
}

void Configuration::setGameLoopIntervalMs(int intervalMs) {
    setInt("app.game_loop_interval_ms", intervalMs);
}

// Generic configuration access
QVariant Configuration::getValue(const QString& key, const QVariant& defaultValue) const {
    return m_values.value(key, defaultValue);
}

void Configuration::setValue(const QString& key, const QVariant& value) {
    m_values[key] = value;
}

QString Configuration::getString(const QString& key, const QString& defaultValue) const {
    const QVariant raw = m_values.value(key);
    if (!raw.isValid()) {
        return defaultValue;
    }
    return raw.toString();
}

void Configuration::setString(const QString& key, const QString& value) {
    setValue(key, value);
}

int Configuration::getInt(const QString& key, int defaultValue) const {
    const QVariant raw = m_values.value(key);
    if (!raw.isValid() || !raw.canConvert<int>()) {
        return defaultValue;
    }
    return raw.toInt();
}

void Configuration::setInt(const QString& key, int value) {
    setValue(key, value);
}

float Configuration::getFloat(const QString& key, float defaultValue) const {
    const QVariant raw = m_values.value(key);
    if (!raw.isValid() || !raw.canConvert<float>()) {
        return defaultValue;
    }
    return raw.toFloat();
}

void Configuration::setFloat(const QString& key, float value) {
    setValue(key, value);
}

bool Configuration::getBool(const QString& key, bool defaultValue) const {
    const QVariant raw = m_values.value(key);
    if (!raw.isValid() || !raw.canConvert<bool>()) {
        return defaultValue;
    }
    return raw.toBool();
}

void Configuration::setBool(const QString& key, bool value) {
    setValue(key, value);
}
