#include "core/GameManager.h"
#include "core/Timer.h"
#include <iostream>

GameManager::GameManager()
    : m_state(State::Stopped)
    , m_nextEventHandle(1)
{
}

GameManager& GameManager::getInstance() {
    static GameManager instance;
    return instance;
}

void GameManager::initialize() {
    std::cout << "GameManager initialized" << std::endl;
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
        std::cout << "Game started" << std::endl;
    }
}

void GameManager::pause() {
    if (m_state == State::Running) {
        m_state = State::Paused;
        emitEvent(GameEvent::GamePaused);
        std::cout << "Game paused" << std::endl;
    }
}

void GameManager::resume() {
    if (m_state == State::Paused) {
        m_state = State::Running;
        emitEvent(GameEvent::GameResumed);
        std::cout << "Game resumed" << std::endl;
    }
}

void GameManager::stop() {
    if (m_state != State::Stopped) {
        m_state = State::Stopped;
        emitEvent(GameEvent::GameEnded);
        std::cout << "Game stopped" << std::endl;
    }
}

GameManager::State GameManager::getState() const {
    return m_state;
}

void GameManager::addScene(const std::string& name, std::shared_ptr<Scene> scene) {
    if (!scene) {
        return;
    }

    m_scenes[name] = scene;
    std::cout << "Added scene: " << name << std::endl;
}

bool GameManager::removeScene(const std::string& name) {
    auto it = m_scenes.find(name);
    if (it == m_scenes.end()) {
        return false;
    }

    // Don't remove if it's the active scene
    if (it->second == m_activeScene) {
        std::cerr << "Cannot remove active scene: " << name << std::endl;
        return false;
    }

    m_scenes.erase(it);
    emitEvent(GameEvent::SceneUnloaded, name);
    return true;
}

std::shared_ptr<Scene> GameManager::getScene(const std::string& name) const {
    auto it = m_scenes.find(name);
    if (it != m_scenes.end()) {
        return it->second;
    }
    return nullptr;
}

bool GameManager::setActiveScene(const std::string& name) {
    auto it = m_scenes.find(name);
    if (it == m_scenes.end()) {
        std::cerr << "Scene not found: " << name << std::endl;
        return false;
    }

    // Cleanup old scene if needed
    if (m_activeScene) {
        std::cout << "Switching from scene: " << m_activeSceneName << std::endl;
    }

    m_activeScene = it->second;
    m_activeSceneName = name;
    
    // Initialize new scene
    m_activeScene->initialize();
    
    emitEvent(GameEvent::SceneChanged, name);
    std::cout << "Active scene set to: " << name << std::endl;
    
    return true;
}

std::shared_ptr<Scene> GameManager::getActiveScene() const {
    return m_activeScene;
}

const std::string& GameManager::getActiveSceneName() const {
    return m_activeSceneName;
}

int GameManager::registerEventCallback(GameEventCallback callback) {
    int handle = m_nextEventHandle++;
    m_eventCallbacks[handle] = callback;
    return handle;
}

void GameManager::unregisterEventCallback(int handle) {
    m_eventCallbacks.erase(handle);
}

void GameManager::emitEvent(GameEvent event, const std::string& data) {
    for (const auto& pair : m_eventCallbacks) {
        if (pair.second) {
            pair.second(event, data);
        }
    }
}
