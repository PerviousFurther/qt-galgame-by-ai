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
#include <QTimer>
#include <QUrl>

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

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

    // Step 6: Start game loop in Qt event loop
    qDebug() << "=== Starting Game Loop ===";
    gameManager.start();
    auto* executionSingleton = &Execution::getInstance();
    auto* gameManagerSingleton = &GameManager::getInstance();

    QTimer gameLoopTimer;
    QObject::connect(&gameLoopTimer, &QTimer::timeout, [executionSingleton, gameManagerSingleton]() {
        executionSingleton->update();
        gameManagerSingleton->update();
        if (executionSingleton->shouldFixedUpdate()) {
            gameManagerSingleton->fixedUpdate();
        }
    });
    gameLoopTimer.start(16);  // ~60 FPS

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    const QStringList qmlSceneUrls = resources.getResourceUrlsBySuffix("qml");
    const QString startupSceneUrl = qmlSceneUrls.contains("qrc:/scene.qml")
                                        ? QStringLiteral("qrc:/scene.qml")
                                        : (qmlSceneUrls.isEmpty() ? QString() : qmlSceneUrls.first());
    if (startupSceneUrl.isEmpty()) {
        qWarning() << "No QML scene found in resources";
        return -1;
    }
    engine.load(QUrl(startupSceneUrl));
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "Failed to load QML scene:" << startupSceneUrl;
        return -1;
    }

    QObject::connect(&app, &QCoreApplication::aboutToQuit, [executionSingleton, gameManagerSingleton]() {
        qDebug() << "=== Engine Statistics ===";
        qDebug() << "Total frames:" << executionSingleton->getFrameCount();
        qDebug() << "Total runtime:" << executionSingleton->getRuntime() << "s";
        qDebug() << "Active scene:" << gameManagerSingleton->getActiveSceneName();

        gameManagerSingleton->stop();

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
