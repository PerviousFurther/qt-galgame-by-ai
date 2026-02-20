#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include "scene/Scene.h"
#include <QHash>
#include <QColor>
#include <QPointer>
#include <QSharedPointer>
#include <QQuickWindow>
#include <QString>
#include <QVariant>

/**
 * @brief GameManager singleton – central controller for game logic and flow.
 *
 * Responsibilities:
 * - Scene management (loading, switching, unloading)
 * - Game-state lifecycle (Stopped / Running / Paused)
 * - Story-step tracking and persistence
 * - Screen navigation
 *
 * Public interface is intentionally minimal: callers read/write Q_PROPERTYs
 * and connect to signals; they do not need to know implementation details.
 */
class GameManager : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString gameState READ getGameState NOTIFY gameStateChanged)
    Q_PROPERTY(QString activeScene READ getActiveSceneName NOTIFY activeSceneChanged)
    Q_PROPERTY(int currentStoryStep READ getCurrentStoryStep WRITE setCurrentStoryStep NOTIFY currentStoryStepChanged)
    Q_PROPERTY(int savedStep READ getSavedStep NOTIFY savedStepChanged)
    Q_PROPERTY(QString currentScreen READ getCurrentScreen WRITE setCurrentScreen NOTIFY currentScreenChanged)
    Q_PROPERTY(QString currentScreenUrl READ getCurrentScreenUrl NOTIFY currentScreenChanged)

public:
    enum class State { Stopped, Running, Paused };
    Q_ENUM(State)

    static GameManager& getInstance();
    static void setInstance(GameManager* instance);
    explicit GameManager(QObject* parent = nullptr);
    ~GameManager() = default;

    void initialize();
    void attachRenderWindow(QQuickWindow* window);
    void handleApplicationStateChange(Qt::ApplicationState state);

    // Scene management
    void addScene(const QString& name, QSharedPointer<Scene> scene);
    bool removeScene(const QString& name);
    QSharedPointer<Scene> getScene(const QString& name) const;
    bool setActiveScene(const QString& name);
    QSharedPointer<Scene> getActiveScene() const;
    const QString& getActiveSceneName() const;

    // State
    State getState() const;
    QString getGameState() const;

    // Q_PROPERTY accessors
    int getCurrentStoryStep() const;
    void setCurrentStoryStep(int step);
    int getSavedStep() const;
    QString getCurrentScreen() const;
    QString getCurrentScreenUrl() const;
    void setCurrentScreen(const QString& screen);

    // Invokable actions exposed to QML
    Q_INVOKABLE void setState(State newState);
    Q_INVOKABLE void startGame(int fromStep = 0);
    Q_INVOKABLE bool hasSaves() const;
    Q_INVOKABLE bool save();
    Q_INVOKABLE void finishOpening();
    Q_INVOKABLE QVariantMap advanceStory(const QVariantList& storyData, const QVariantList& visitedShots);
    Q_INVOKABLE QVariantList buildRouteShots(const QVariantList& storyData) const;
    Q_INVOKABLE QString emotionEmoji(const QString& emotion) const;
    Q_INVOKABLE QColor emotionColor(const QString& emotion, const QColor& baseColor) const;
    Q_INVOKABLE QVariantMap getGameConstants() const;

public slots:
    void processFrame();

signals:
    void gameStateChanged();
    void activeSceneChanged();
    void currentStoryStepChanged();
    void savedStepChanged();
    void currentScreenChanged();

private:
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    void loadScenesFromResources();
    void update();
    void fixedUpdate();

    State m_state;
    QHash<QString, QSharedPointer<Scene>> m_scenes;
    QSharedPointer<Scene> m_activeScene;
    QString m_activeSceneName;
    bool m_frameUpdateInProgress;
    QPointer<QQuickWindow> m_renderWindow;
    int m_currentStoryStep;
    QString m_currentScreen;
    mutable QVariantMap m_cachedGameConstants;
};

#endif // GAMEMANAGER_H
