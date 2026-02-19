#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include "scene/Scene.h"
#include <QHash>
#include <QPointer>
#include <QSharedPointer>
#include <QQuickWindow>
#include <QString>
#include <QVariant>

/**
 * @brief Game event types
 */
enum class GameEvent {
    SceneLoaded,
    SceneUnloaded,
    SceneChanged,
    GameStarted,
    GamePaused,
    GameResumed,
    GameEnded,
    SaveRequested,
    LoadRequested,
    SettingsChanged,
    // Add more events as needed
};

/**
 * @brief Event callback type
 * @param event The event type
 * @param data Optional event data (scene name, save slot, etc.)
 */
/**
 * @brief GameManager singleton for managing game logic and flow
 * 
 * GameManager is the central controller for the game, handling:
 * - Scene management (loading, switching, unloading)
 * - Game state (running, paused, etc.)
 * - Game events and callbacks
 * - Coordination between different systems
 * 
 * Different Scene types can be used:
 * - DialogScene for conversations
 * - MiniGameScene for gameplay segments
 * - SettingsScene for configuration (operates on Configuration singleton)
 * 
 * Note: Scenes that modify singletons (like SettingsScene modifying Configuration)
 * are assumed to run on the main thread without async conflicts.
 */
class GameManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString gameState READ getGameState NOTIFY gameStateChanged)
    Q_PROPERTY(QString activeScene READ getActiveSceneName NOTIFY activeSceneChanged)
public:
    enum class State {
        Stopped,
        Running,
        Paused
    };

    /**
     * @brief Get the singleton instance
     * @return Reference to the GameManager singleton
     */
    static GameManager& getInstance();

    /**
     * @brief Initialize the game manager
     * Should be called once at application start, after Configuration is loaded
     */
    void initialize();
    void loadScenesFromResources();

    /**
     * @brief Update game logic
     * Should be called every frame
     */
    void update();

    /**
     * @brief Fixed update for game logic
     * Should be called at fixed intervals
     */
    void fixedUpdate();

    /**
     * @brief Start the game
     */
    Q_INVOKABLE void start();

    /**
     * @brief Pause the game
     */
    Q_INVOKABLE void pause();

    /**
     * @brief Resume the game
     */
    Q_INVOKABLE void resume();

    /**
     * @brief Stop the game
     */
    Q_INVOKABLE void stop();
    void handleApplicationStateChange(Qt::ApplicationState state);

    /**
     * @brief Get current game state
     * @return Current state
     */
    State getState() const;
    QString getGameState() const;

    // Scene management
    
    /**
     * @brief Add a scene to the manager
     * @param name Scene name/identifier
     * @param scene Shared pointer to the scene
     */
    void addScene(const QString& name, QSharedPointer<Scene> scene);

    /**
     * @brief Remove a scene from the manager
     * @param name Scene name
     * @return true if successful, false if not found
     */
    bool removeScene(const QString& name);

    /**
     * @brief Get a scene by name
     * @param name Scene name
     * @return Shared pointer to the scene, or nullptr if not found
     */
    QSharedPointer<Scene> getScene(const QString& name) const;

    /**
     * @brief Set the active scene
     * @param name Scene name
     * @return true if successful, false if scene not found
     */
    bool setActiveScene(const QString& name);

    /**
     * @brief Get the currently active scene
     * @return Active scene, or nullptr if none
     */
    QSharedPointer<Scene> getActiveScene() const;

    /**
     * @brief Get the name of the active scene
     * @return Active scene name, or empty string if none
     */
    const QString& getActiveSceneName() const;

    // Event system

    /**
     * @brief Emit a game event
     * @param event Event type
     * @param data Optional event data payload
     */
    void emitEvent(GameEvent event, const QVariant& data = {});
    void attachRenderWindow(QQuickWindow* window);

public slots:
    void processFrame();

signals:
    void gameEventTriggered(GameEvent event, const QVariant& data);
    void gameStateChanged();
    void activeSceneChanged();

private:
    GameManager();
    ~GameManager() = default;
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    State m_state;
    QHash<QString, QSharedPointer<Scene>> m_scenes;
    QSharedPointer<Scene> m_activeScene;
    QString m_activeSceneName;
    bool m_frameUpdateInProgress;
    QPointer<QQuickWindow> m_renderWindow;
};

Q_DECLARE_METATYPE(GameEvent)

#endif // GAMEMANAGER_H
