#include "core/Configuration.h"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
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
    setStartupSceneUrl("qrc:/main.qml");
    setGameLoopIntervalMs(16);  // ~60 FPS (1000ms / 60 ≈ 16.67ms)

    // Game state defaults
    setOpeningAnimationPlayed(false);
    setConfigFilePath("galgame_config.json");
    setSavesPath("galgame_saves.json");
}

bool Configuration::loadFromFile(const QString& filePath) {
    QString normalizedPath = filePath;
    if (filePath.startsWith("qrc:/")) {
        normalizedPath = ":" + filePath.mid(4);
    }

    QFile file(normalizedPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Config file not found:" << filePath;
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse config JSON:" << filePath << parseError.errorString();
        return false;
    }
    if (!doc.isObject()) {
        qWarning() << "Config JSON root is not an object:" << filePath;
        return false;
    }

    const QJsonObject root = doc.object();

    if (root.contains("audio")) {
        const QJsonObject audio = root["audio"].toObject();
        if (audio.contains("master_volume"))      setMasterVolume(static_cast<float>(audio["master_volume"].toDouble()));
        if (audio.contains("music_volume"))       setMusicVolume(static_cast<float>(audio["music_volume"].toDouble()));
        if (audio.contains("sound_effect_volume")) setSoundEffectVolume(static_cast<float>(audio["sound_effect_volume"].toDouble()));
        if (audio.contains("voice_volume"))       setVoiceVolume(static_cast<float>(audio["voice_volume"].toDouble()));
    }

    if (root.contains("window")) {
        const QJsonObject window = root["window"].toObject();
        if (window.contains("width"))      setWindowWidth(window["width"].toInt());
        if (window.contains("height"))     setWindowHeight(window["height"].toInt());
        if (window.contains("fullscreen")) setFullscreen(window["fullscreen"].toBool());
    }

    if (root.contains("render")) {
        const QJsonObject render = root["render"].toObject();
        if (render.contains("target_fps")) setTargetFPS(render["target_fps"].toInt());
        if (render.contains("vsync"))      setVSyncEnabled(render["vsync"].toBool());
    }

    if (root.contains("game")) {
        const QJsonObject game = root["game"].toObject();
        if (game.contains("opening_animation_played"))
            setOpeningAnimationPlayed(game["opening_animation_played"].toBool());
        if (game.contains("saves_path"))
            setSavesPath(game["saves_path"].toString());
    }

    qDebug() << "Configuration loaded from:" << filePath;
    return true;
}

void Configuration::parseCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        const QString arg = QString::fromUtf8(argv[i]);

        // Handle --config=<path> specifically
        if (arg.startsWith("--config=")) {
            setConfigFilePath(arg.mid(9));
            continue;
        }

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
    QJsonObject audio;
    audio["master_volume"]       = static_cast<double>(getMasterVolume());
    audio["music_volume"]        = static_cast<double>(getMusicVolume());
    audio["sound_effect_volume"] = static_cast<double>(getSoundEffectVolume());
    audio["voice_volume"]        = static_cast<double>(getVoiceVolume());

    QJsonObject window;
    window["width"]      = getWindowWidth();
    window["height"]     = getWindowHeight();
    window["fullscreen"] = isFullscreen();

    QJsonObject render;
    render["target_fps"] = getTargetFPS();
    render["vsync"]      = isVSyncEnabled();

    QJsonObject game;
    game["opening_animation_played"] = isOpeningAnimationPlayed();
    game["saves_path"]               = getSavesPath();

    QJsonObject root;
    root["audio"]  = audio;
    root["window"] = window;
    root["render"] = render;
    root["game"]   = game;

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to save config to:" << filePath;
        return false;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "Configuration saved to:" << filePath;
    return true;
}

// Audio settings
float Configuration::getMasterVolume() const {
    return getFloat("audio.master_volume", 1.0f);
}

void Configuration::setMasterVolume(float volume) {
    if (getMasterVolume() == volume) {
        return;
    }
    setFloat("audio.master_volume", volume);
    emit masterVolumeChanged();
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
    if (getTargetFPS() == fps) {
        return;
    }
    setInt("render.target_fps", fps);
    emit targetFpsChanged();
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
    if (getApplicationName() == appName) {
        return;
    }
    setString("app.name", appName);
    emit applicationNameChanged();
}

QString Configuration::getStartupSceneUrl() const {
    return getString("app.startup_scene_url", "qrc:/main.qml");
}

void Configuration::setStartupSceneUrl(const QString& sceneUrl) {
    if (getStartupSceneUrl() == sceneUrl) {
        return;
    }
    setString("app.startup_scene_url", sceneUrl);
    emit startupSceneUrlChanged();
}

int Configuration::getGameLoopIntervalMs() const {
    return getInt("app.game_loop_interval_ms", 16);
}

void Configuration::setGameLoopIntervalMs(int intervalMs) {
    if (getGameLoopIntervalMs() == intervalMs) {
        return;
    }
    setInt("app.game_loop_interval_ms", intervalMs);
    emit gameLoopIntervalMsChanged();
}

bool Configuration::isOpeningAnimationPlayed() const {
    return getBool("game.opening_animation_played", false);
}

void Configuration::setOpeningAnimationPlayed(bool played) {
    if (isOpeningAnimationPlayed() == played) {
        return;
    }
    setBool("game.opening_animation_played", played);
    emit openingAnimationPlayedChanged();
}

QString Configuration::getConfigFilePath() const {
    return getString("app.config_file_path", "galgame_config.json");
}

void Configuration::setConfigFilePath(const QString& path) {
    if (getConfigFilePath() == path) {
        return;
    }
    setString("app.config_file_path", path);
    emit configFilePathChanged();
}

QString Configuration::getSavesPath() const {
    return getString("game.saves_path", "galgame_saves.json");
}

void Configuration::setSavesPath(const QString& path) {
    if (getSavesPath() == path) {
        return;
    }
    setString("game.saves_path", path);
    emit savesPathChanged();
}

bool Configuration::saveConfig() const {
    return saveToFile(getConfigFilePath());
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
