#include "codingstyle.h" // include/codingstyle.h
#include "core/Execution.h"
#include "core/Configuration.h"
#include "core/GameManager.h"
#include "factory/Registration.h"
#include "factory/NativeItemFactory.h"
#include "resources/Resources.h"
#include <QGuiApplication>
#include <QDebug>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QString>
#include <QTimer>
#include <QUrl>

class GameManagerUiBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString gameState READ gameState NOTIFY gameStateChanged)
    Q_PROPERTY(QString activeScene READ activeScene NOTIFY activeSceneChanged)
public:
    explicit GameManagerUiBridge(GameManager* manager, QObject* parent = nullptr)
        : QObject(parent)
        , m_manager(manager)
        , m_lastGameState(gameState())
        , m_lastActiveScene(activeScene())
    {
        Q_ASSERT(m_manager != nullptr);
        QObject::connect(m_manager, &GameManager::gameEventTriggered, this, [this](GameEvent, const QVariant&) {
            const QString currentState = gameState();
            const QString currentScene = activeScene();
            if (currentState != m_lastGameState) {
                m_lastGameState = currentState;
                emit gameStateChanged();
            }
            if (currentScene != m_lastActiveScene) {
                m_lastActiveScene = currentScene;
                emit activeSceneChanged();
            }
        });
    }

    Q_INVOKABLE void startGame() { m_manager->start(); }
    Q_INVOKABLE void pauseGame() { m_manager->pause(); }
    Q_INVOKABLE void resumeGame() { m_manager->resume(); }
    Q_INVOKABLE void stopGame() { m_manager->stop(); }

    QString gameState() const {
        switch (m_manager->getState()) {
        case GameManager::State::Running:
            return QStringLiteral("Running");
        case GameManager::State::Paused:
            return QStringLiteral("Paused");
        case GameManager::State::Stopped:
        default:
            return QStringLiteral("Stopped");
        }
    }

    QString activeScene() const {
        return m_manager->getActiveSceneName();
    }

signals:
    void gameStateChanged();
    void activeSceneChanged();

private:
    GameManager* m_manager;
    QString m_lastGameState;
    QString m_lastActiveScene;
};

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setApplicationName("qt-galgame-by-ai");

    qDebug() << "Qt Galgame Engine - Visual Novel Development Framework";
    qDebug() << "======================================================";

    // Step 1: Initialize Configuration (first step in any application)
    qDebug() << "=== Initializing Configuration ===";
    Configuration& config = Configuration::getInstance();
    config.parseCommandLine(argc, argv);
    config.loadFromFile("qrc:/config.json");
    
    qDebug() << "Configuration loaded:";
    qDebug() << "  Window:" << config.getWindowWidth() << "x" << config.getWindowHeight();
    qDebug() << "  Target FPS:" << config.getTargetFPS();
    qDebug() << "  Master Volume:" << config.getMasterVolume();

    // Step 2: Initialize Execution
    qDebug() << "=== Initializing Execution ===";
    Execution& execution = Execution::getInstance();
    execution.initialize();
    execution.setFixedUpdateInterval(1.0f / 60.0f);  // 60 FPS for fixed updates
    qDebug() << "Execution initialized (fixed update:" << execution.getFixedUpdateInterval()
             << "s, max threads:" << execution.getMaxThreadCount() << ")";

    // Step 3: Register factories for Item creation
    qDebug() << "=== Registering Factories ===";
    Registration& registration = Registration::getInstance();
    auto nativeFactory = QSharedPointer<NativeItemFactory>::create();
    registration.registerFactory(nativeFactory);
    qDebug() << "Native factory registered";
    for (const auto& type : registration.getRegisteredTypes()) {
        qDebug() << "Registered type:" << type;
    }

    // Step 4: Initialize Resources
    qDebug() << "=== Initializing Resources ===";
    Resources& resources = Resources::getInstance();
    qDebug() << "Resources system initialized with default loaders";

    // Step 5: Initialize GameManager
    qDebug() << "=== Initializing GameManager ===";
    GameManager& gameManager = GameManager::getInstance();
    gameManager.initialize();

    // Step 6: Bind GameManager to application lifecycle and UI bridge
    auto* gameBridge = new GameManagerUiBridge(&gameManager, &app);
    QObject::connect(&app, &QGuiApplication::applicationStateChanged, [&gameManager](Qt::ApplicationState state) {
        if (state == Qt::ApplicationActive && gameManager.getState() == GameManager::State::Paused) {
            gameManager.resume();
        } else if (state != Qt::ApplicationActive && gameManager.getState() == GameManager::State::Running) {
            gameManager.pause();
        }
    });

    // Step 7: Start game loop in Qt event loop
    qDebug() << "=== Starting Game Loop ===";
    QTimer gameLoopTimer;
    QObject::connect(&gameLoopTimer, &QTimer::timeout, [&execution, &gameManager]() {
        execution.update();
        gameManager.update();
        if (execution.shouldFixedUpdate()) {
            gameManager.fixedUpdate();
        }
    });
    gameLoopTimer.start(16);  // ~60 FPS

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("gameBridge", gameBridge);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    const QString defaultQmlScene = QStringLiteral("qrc:/scene.qml");
    const QStringList qmlSceneUrls = resources.getResourceUrlsBySuffix("qml");
    QString startupSceneUrl;
    if (qmlSceneUrls.contains(defaultQmlScene)) {
        startupSceneUrl = defaultQmlScene;
    } else if (!qmlSceneUrls.isEmpty()) {
        startupSceneUrl = qmlSceneUrls.first();
    }
    if (startupSceneUrl.isEmpty()) {
        qWarning() << "No QML scene found in resources";
        return -1;
    }
    engine.load(QUrl(startupSceneUrl));
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "Failed to load QML scene:" << startupSceneUrl;
        return -1;
    }

    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&execution, &gameManager]() {
        qDebug() << "=== Engine Statistics ===";
        qDebug() << "Total frames:" << execution.getFrameCount();
        qDebug() << "Total runtime:" << execution.getRuntime() << "s";
        qDebug() << "Active scene:" << gameManager.getActiveSceneName();

        gameManager.stop();

        qDebug() << "=== Demonstration Completed Successfully! ===";
        qDebug() << "Architecture summary:";
        qDebug() << "  ✓ Execution singleton for timing and task dispatch";
        qDebug() << "  ✓ Configuration singleton for settings";
        qDebug() << "  ✓ Registration singleton for Item factories";
        qDebug() << "  ✓ Resources singleton for named loader asset management";
        qDebug() << "  ✓ GameManager singleton for game flow";
        qDebug() << "  ✓ Scene inheritance support for specialized scenes";
        qDebug() << "  ✓ Item update() and fixedUpdate() methods";
    });

    return app.exec();
}
