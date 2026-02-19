#include "codingstyle.h" // include/codingstyle.h
#include "core/GameManager.h"
#include <QDebug>

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
        qDebug() << "Game started";
    }
}

void GameManager::pause() {
    if (m_state == State::Running) {
        m_state = State::Paused;
        emitEvent(GameEvent::GamePaused);
        qDebug() << "Game paused";
    }
}

void GameManager::resume() {
    if (m_state == State::Paused) {
        m_state = State::Running;
        emitEvent(GameEvent::GameResumed);
        qDebug() << "Game resumed";
    }
}

void GameManager::stop() {
    if (m_state != State::Stopped) {
        m_state = State::Stopped;
        emitEvent(GameEvent::GameEnded);
        qDebug() << "Game stopped";
    }
}

GameManager::State GameManager::getState() const {
    return m_state;
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
