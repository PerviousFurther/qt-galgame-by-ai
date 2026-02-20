#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QVariant>

/**
 * @brief Configuration singleton for managing application settings.
 *
 * Settings are stored in a flat key-value map and exposed to QML via
 * Q_PROPERTYs.  For non-QML C++ code use getValue() / setValue().
 * Typed helpers (getInt, etc.) are private implementation details.
 */
class Configuration : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString applicationName READ getApplicationName WRITE setApplicationName NOTIFY applicationNameChanged)
    Q_PROPERTY(int targetFps READ getTargetFPS WRITE setTargetFPS NOTIFY targetFpsChanged)
    Q_PROPERTY(int gameLoopIntervalMs READ getGameLoopIntervalMs WRITE setGameLoopIntervalMs NOTIFY gameLoopIntervalMsChanged)
    Q_PROPERTY(QString startupSceneUrl READ getStartupSceneUrl WRITE setStartupSceneUrl NOTIFY startupSceneUrlChanged)
    Q_PROPERTY(bool openingAnimationPlayed READ isOpeningAnimationPlayed WRITE setOpeningAnimationPlayed NOTIFY openingAnimationPlayedChanged)
    Q_PROPERTY(QString configFilePath READ getConfigFilePath WRITE setConfigFilePath NOTIFY configFilePathChanged)
    Q_PROPERTY(QString savesPath READ getSavesPath WRITE setSavesPath NOTIFY savesPathChanged)
    Q_PROPERTY(float masterVolume READ getMasterVolume WRITE setMasterVolume NOTIFY masterVolumeChanged)
public:
    static Configuration& getInstance();

    bool loadFromFile(const QString& filePath);
    void parseCommandLine(int argc, char* argv[]);
    bool saveToFile(const QString& filePath) const;

    // Generic key-value access (preferred for non-QML C++ callers)
    QVariant getValue(const QString& key, const QVariant& defaultValue = {}) const;
    void setValue(const QString& key, const QVariant& value);

    // Q_PROPERTY accessors (public because Qt requires it)
    QString getApplicationName() const;
    void setApplicationName(const QString& appName);

    int getTargetFPS() const;
    void setTargetFPS(int fps);

    int getGameLoopIntervalMs() const;
    void setGameLoopIntervalMs(int intervalMs);

    QString getStartupSceneUrl() const;
    void setStartupSceneUrl(const QString& sceneUrl);

    bool isOpeningAnimationPlayed() const;
    void setOpeningAnimationPlayed(bool played);

    QString getConfigFilePath() const;
    void setConfigFilePath(const QString& path);

    QString getSavesPath() const;
    void setSavesPath(const QString& path);

    float getMasterVolume() const;
    void setMasterVolume(float volume);

    Q_INVOKABLE bool saveConfig() const;

signals:
    void applicationNameChanged();
    void targetFpsChanged();
    void gameLoopIntervalMsChanged();
    void startupSceneUrlChanged();
    void openingAnimationPlayedChanged();
    void configFilePathChanged();
    void savesPathChanged();
    void masterVolumeChanged();

private:
    Configuration();
    ~Configuration() = default;
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;

    void setDefaults();

    // Typed helpers — private; external code uses getValue/setValue
    QString getString(const QString& key, const QString& defaultValue = {}) const;
    void setString(const QString& key, const QString& value);
    int getInt(const QString& key, int defaultValue = 0) const;
    void setInt(const QString& key, int value);
    float getFloat(const QString& key, float defaultValue = 0.0f) const;
    void setFloat(const QString& key, float value);
    bool getBool(const QString& key, bool defaultValue = false) const;
    void setBool(const QString& key, bool value);

    // Internal audio/window/render accessors (only used in load/save/defaults)
    float getMusicVolume() const;
    void setMusicVolume(float volume);
    float getSoundEffectVolume() const;
    void setSoundEffectVolume(float volume);
    float getVoiceVolume() const;
    void setVoiceVolume(float volume);
    int getWindowWidth() const;
    void setWindowWidth(int width);
    int getWindowHeight() const;
    void setWindowHeight(int height);
    bool isFullscreen() const;
    void setFullscreen(bool fullscreen);
    bool isVSyncEnabled() const;
    void setVSyncEnabled(bool enabled);

    QHash<QString, QVariant> m_values;
};

#endif // CONFIGURATION_H
