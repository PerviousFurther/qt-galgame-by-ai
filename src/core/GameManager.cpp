#include "core/GameManager.h"
#include "resources/Resources.h"

#include <QDebug>
#include <QFileInfo>

GameManager::GameManager()
    : m_state(State::Stopped)
{
}

GameManager& GameManager::getInstance() {
    static GameManager instance;
    return instance;
}

void GameManager::initialize() {
    qDebug() << "GameManager initialized";
    m_state = State::Stopped;
    loadScenesFromResources();
    if (m_activeScene.isNull() && !m_scenes.isEmpty()) {
        setActiveScene(m_scenes.constBegin().key());
    }
}

void GameManager::loadScenesFromResources() {
    Resources& resources = Resources::getInstance();
    const QStringList sceneUrls = resources.getResourceUrlsBySuffix("json") +
                                  resources.getResourceUrlsBySuffix("qml");
    for (const QString& sceneUrl : sceneUrls) {
        const QFileInfo sceneInfo(sceneUrl);
        const QString sceneName = sceneInfo.completeBaseName();
        const QString suffix = sceneInfo.suffix().toLower();
        if (sceneName.isEmpty()) {
            continue;
        }
        if (suffix != "json" && suffix != "qml") {
            continue;
        }
        QString sceneKey = sceneName + "_" + suffix;
        int suffixIndex = 1;
        while (m_scenes.contains(sceneKey)) {
            sceneKey = sceneName + "_" + suffix + "_" + QString::number(suffixIndex++);
        }
        QSharedPointer<Scene> scene = QSharedPointer<Scene>::create();
        scene->setId(sceneName);
        const bool loaded = (suffix == "json") ? scene->loadFromJson(sceneUrl)
                                               : scene->loadFromQml(sceneUrl);
        if (!loaded) {
            qWarning() << "Failed to load scene from resource:" << sceneUrl;
            continue;
        }
        addScene(sceneKey, scene);
        emitEvent(GameEvent::SceneLoaded, sceneKey);
    }
}

void GameManager::update() {
    if (m_state != State::Running) {
        return;
    }

    // Update active scene
    if (m_activeScene) {
        m_activeScene->update();
    }
}

void GameManager::fixedUpdate() {
    if (m_state != State::Running) {
        return;
    }

    // Fixed update for active scene
    if (m_activeScene) {
        m_activeScene->fixedUpdate();
    }
}

void GameManager::start() {
    if (m_state == State::Stopped) {
        m_state = State::Running;
        emitEvent(GameEvent::GameStarted);
        emit gameStateChanged();
        qDebug() << "Game started";
    }
}

void GameManager::pause() {
    if (m_state == State::Running) {
        m_state = State::Paused;
        emitEvent(GameEvent::GamePaused);
        emit gameStateChanged();
        qDebug() << "Game paused";
    }
}

void GameManager::resume() {
    if (m_state == State::Paused) {
        m_state = State::Running;
        emitEvent(GameEvent::GameResumed);
        emit gameStateChanged();
        qDebug() << "Game resumed";
    }
}

void GameManager::stop() {
    if (m_state != State::Stopped) {
        m_state = State::Stopped;
        emitEvent(GameEvent::GameEnded);
        emit gameStateChanged();
        qDebug() << "Game stopped";
    }
}

GameManager::State GameManager::getState() const {
    return m_state;
}

QString GameManager::getGameState() const {
    switch (m_state) {
    case State::Running:
        return QStringLiteral("Running");
    case State::Paused:
        return QStringLiteral("Paused");
    case State::Stopped:
    default:
        return QStringLiteral("Stopped");
    }
}

void GameManager::addScene(const QString& name, QSharedPointer<Scene> scene) {
    if (scene.isNull()) {
        return;
    }

    m_scenes[name] = scene;
    qDebug() << "Added scene:" << name;
}

bool GameManager::removeScene(const QString& name) {
    if (!m_scenes.contains(name)) {
        return false;
    }

    // Don't remove if it's the active scene
    if (m_scenes.value(name) == m_activeScene) {
        qWarning() << "Cannot remove active scene:" << name;
        return false;
    }

    m_scenes.remove(name);
    emitEvent(GameEvent::SceneUnloaded, name);
    return true;
}

QSharedPointer<Scene> GameManager::getScene(const QString& name) const {
    return m_scenes.value(name);
}

bool GameManager::setActiveScene(const QString& name) {
    if (!m_scenes.contains(name)) {
        qWarning() << "Scene not found:" << name;
        return false;
    }

    // Cleanup old scene if needed
    if (m_activeScene) {
        qDebug() << "Switching from scene:" << m_activeSceneName;
    }

    m_activeScene = m_scenes.value(name);
    m_activeSceneName = name;
    
    // Initialize new scene
    m_activeScene->initialize();
    
    emitEvent(GameEvent::SceneChanged, name);
    emit activeSceneChanged();
    qDebug() << "Active scene set to:" << name;
    
    return true;
}

QSharedPointer<Scene> GameManager::getActiveScene() const {
    return m_activeScene;
}

const QString& GameManager::getActiveSceneName() const {
    return m_activeSceneName;
}

void GameManager::emitEvent(GameEvent event, const QVariant& data) {
    emit gameEventTriggered(event, data);
}
