#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <map>
#include <memory>

/**
 * @brief Configuration singleton for managing application settings
 * 
 * Configuration handles loading and managing settings from:
 * - JSON configuration files
 * - Command line arguments
 * 
 * Settings include:
 * - Audio volume levels
 * - Window dimensions
 * - Render frame rate
 * - Other game-specific configurations
 * 
 * Configuration can be modified through UI (e.g., menu.qml)
 * Note: This class is not thread-safe. Settings modifications from UI
 * should occur on the main thread.
 */
class Configuration {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the Configuration singleton
     */
    static Configuration& getInstance();

    /**
     * @brief Load configuration from a JSON file
     * @param filePath Path to the JSON configuration file
     * @return true if successful, false otherwise
     */
    bool loadFromFile(const std::string& filePath);

    /**
     * @brief Parse command line arguments
     * @param argc Argument count
     * @param argv Argument values
     */
    void parseCommandLine(int argc, char* argv[]);

    /**
     * @brief Save current configuration to a JSON file
     * @param filePath Path to save the configuration
     * @return true if successful, false otherwise
     */
    bool saveToFile(const std::string& filePath) const;

    // Audio settings
    float getMasterVolume() const;
    void setMasterVolume(float volume);

    float getMusicVolume() const;
    void setMusicVolume(float volume);

    float getSoundEffectVolume() const;
    void setSoundEffectVolume(float volume);

    float getVoiceVolume() const;
    void setVoiceVolume(float volume);

    // Window settings
    int getWindowWidth() const;
    void setWindowWidth(int width);

    int getWindowHeight() const;
    void setWindowHeight(int height);

    bool isFullscreen() const;
    void setFullscreen(bool fullscreen);

    // Render settings
    int getTargetFPS() const;
    void setTargetFPS(int fps);

    bool isVSyncEnabled() const;
    void setVSyncEnabled(bool enabled);

    // Generic configuration access
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    void setString(const std::string& key, const std::string& value);

    int getInt(const std::string& key, int defaultValue = 0) const;
    void setInt(const std::string& key, int value);

    float getFloat(const std::string& key, float defaultValue = 0.0f) const;
    void setFloat(const std::string& key, float value);

    bool getBool(const std::string& key, bool defaultValue = false) const;
    void setBool(const std::string& key, bool value);

private:
    Configuration();
    ~Configuration() = default;
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;

    // Default values
    void setDefaults();

    std::map<std::string, std::string> m_stringValues;
    std::map<std::string, int> m_intValues;
    std::map<std::string, float> m_floatValues;
    std::map<std::string, bool> m_boolValues;
};

#endif // CONFIGURATION_H
