#include "core/GameManager.h"
#include "core/Configuration.h"
#include "core/Execution.h"
#include "resources/Resources.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMetaObject>

namespace {
constexpr int MaxFixedUpdateStepsPerFrame = 8;
}

GameManager::GameManager()
    : m_state(State::Stopped)
    , m_frameUpdateInProgress(false)
    , m_currentStoryStep(0)
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
    // Determine initial screen based on config
    const bool played = Configuration::getInstance().isOpeningAnimationPlayed();
    m_currentScreen = played ? QStringLiteral("menu") : QStringLiteral("opening");
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
        if (!scene->load(sceneUrl)) {
            qWarning() << "Failed to load scene from resource:" << sceneUrl;
            continue;
        }
        addScene(sceneKey, scene);
    }
}

void GameManager::update() {
    if (m_state != State::Running) {
        return;
    }
    if (m_activeScene) {
        m_activeScene->update();
    }
}

void GameManager::fixedUpdate() {
    if (m_state != State::Running) {
        return;
    }
    if (m_activeScene) {
        m_activeScene->fixedUpdate();
    }
}

void GameManager::setState(State newState) {
    if (m_state == newState) {
        return;
    }
    // Only Paused→Running is allowed from a paused state (not Stopped→Paused)
    if (newState == State::Paused && m_state != State::Running) {
        return;
    }
    m_state = newState;
    emit gameStateChanged();
    qDebug() << "Game state:" << getGameState();
}

void GameManager::handleApplicationStateChange(Qt::ApplicationState state) {
    if (state == Qt::ApplicationActive && m_state == State::Paused) {
        setState(State::Running);
    } else if (state != Qt::ApplicationActive && m_state == State::Running) {
        setState(State::Paused);
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
    if (m_scenes.value(name) == m_activeScene) {
        qWarning() << "Cannot remove active scene:" << name;
        return false;
    }
    m_scenes.remove(name);
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
    if (m_activeScene) {
        qDebug() << "Switching from scene:" << m_activeSceneName;
    }
    m_activeScene = m_scenes.value(name);
    m_activeSceneName = name;
    m_activeScene->initialize();
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

void GameManager::attachRenderWindow(QQuickWindow* window) {
    if (window == nullptr) {
        return;
    }
    m_renderWindow = window;
    QObject::connect(m_renderWindow, &QQuickWindow::beforeRendering,
                     this, &GameManager::processFrame, Qt::DirectConnection);
    m_renderWindow->requestUpdate();
}

void GameManager::processFrame() {
    if (m_frameUpdateInProgress) {
        return;
    }
    m_frameUpdateInProgress = true;
    Execution& execution = Execution::getInstance();
    execution.update();
    update();
    int fixedStepCount = 0;
    while (execution.shouldFixedUpdate() && fixedStepCount < MaxFixedUpdateStepsPerFrame) {
        fixedUpdate();
        ++fixedStepCount;
    }
    m_frameUpdateInProgress = false;
    if (!m_renderWindow.isNull()) {
        QMetaObject::invokeMethod(m_renderWindow, "requestUpdate", Qt::QueuedConnection);
    }
}

// ── Game-flow invokables ───────────────────────────────────────────────────

void GameManager::startGame(int fromStep) {
    setCurrentStoryStep(fromStep);
    setState(State::Running);
    setCurrentScreen(QStringLiteral("game"));
    qDebug() << "Game started at step:" << fromStep;
}

bool GameManager::hasSaves() const {
    const QString savesPath = Configuration::getInstance().getSavesPath();
    if (savesPath.isEmpty()) {
        return false;
    }
    QFile file(savesPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Saves file parse error:" << parseError.errorString();
        return false;
    }
    return doc.isObject() && doc.object().contains("current_step");
}

bool GameManager::save() {
    const QString savesPath = Configuration::getInstance().getSavesPath();
    if (savesPath.isEmpty()) {
        return false;
    }
    QJsonObject root;
    root["current_step"] = m_currentStoryStep;
    root["timestamp"]    = QDateTime::currentDateTime().toString(Qt::ISODate);

    QFile file(savesPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to save game progress to:" << savesPath;
        return false;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();
    emit savedStepChanged();
    qDebug() << "Game saved at step:" << m_currentStoryStep;
    return true;
}

// ── Q_PROPERTY accessors ──────────────────────────────────────────────────

int GameManager::getCurrentStoryStep() const {
    return m_currentStoryStep;
}

void GameManager::setCurrentStoryStep(int step) {
    if (m_currentStoryStep == step) {
        return;
    }
    m_currentStoryStep = step;
    emit currentStoryStepChanged();
}

int GameManager::getSavedStep() const {
    const QString savesPath = Configuration::getInstance().getSavesPath();
    if (savesPath.isEmpty()) {
        return 0;
    }
    QFile file(savesPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return 0;
    }
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        qDebug() << "Saves file parse error:" << parseError.errorString();
        return 0;
    }
    return doc.object().value("current_step").toInt(0);
}

QString GameManager::getCurrentScreen() const {
    return m_currentScreen;
}

void GameManager::setCurrentScreen(const QString& screen) {
    if (m_currentScreen == screen) {
        return;
    }
    m_currentScreen = screen;
    emit currentScreenChanged();
}


