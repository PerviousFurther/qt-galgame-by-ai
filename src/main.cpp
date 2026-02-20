#include "core/Configuration.h"
#include "core/Execution.h"
#include "core/GameManager.h"
#include "factory/NativeItemFactory.h"
#include "factory/Registration.h"
#include "resources/Resources.h"

#include <QDebug>
#include <QGuiApplication>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <qqml.h>

namespace {

void initializeCoreSystems(int argc, char* argv[], QGuiApplication& app) {
    Configuration& config = Configuration::getInstance();
    config.parseCommandLine(argc, argv);
    app.setApplicationName(config.getApplicationName());

    // Load from filesystem config; create with defaults if not found
    if (!config.loadFromFile(config.getConfigFilePath())) {
        qDebug() << "Creating default config file:" << config.getConfigFilePath();
        config.saveToFile(config.getConfigFilePath());
    }

    Execution& execution = Execution::getInstance();
    execution.initialize();
    const int targetFps = config.getTargetFPS() > 0 ? config.getTargetFPS() : 60;
    execution.setFixedUpdateInterval(1.0f / static_cast<float>(targetFps));

    Registration::getInstance().registerFactory(QSharedPointer<NativeItemFactory>::create());
    Resources::getInstance();
    GameManager::getInstance().initialize();
}

bool loadStartupQml(QQmlApplicationEngine& engine, const QString& startupSceneUrl) {
    if (startupSceneUrl.isEmpty()) {
        qWarning() << "Startup scene URL is empty";
        return false;
    }
    engine.load(QUrl(startupSceneUrl));
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "Failed to load QML scene:" << startupSceneUrl;
        return false;
    }
    return true;
}

void shutdownAndLogStats() {
    Execution& execution = Execution::getInstance();
    GameManager& gameManager = GameManager::getInstance();
    qDebug() << "=== Engine Statistics ===";
    qDebug() << "Total frames:" << execution.getFrameCount();
    qDebug() << "Total runtime:" << execution.getRuntime() << "s";
    qDebug() << "Active scene:" << gameManager.getActiveSceneName();
    gameManager.setState(GameManager::State::Stopped);
}

}

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    Configuration configInstance;
    GameManager gameManagerInstance;
    Configuration::setInstance(&configInstance);
    GameManager::setInstance(&gameManagerInstance);

    qDebug() << "Qt Galgame Engine - Visual Novel Development Framework";
    qDebug() << "======================================================";
    initializeCoreSystems(argc, argv, app);

    Configuration& config = Configuration::getInstance();
    GameManager& gameManager = GameManager::getInstance();
    QObject::connect(&app, &QGuiApplication::applicationStateChanged, &gameManager, &GameManager::handleApplicationStateChange);

    qmlRegisterSingletonInstance("Galgame", 1, 0, "Configuration", &config);
    qmlRegisterSingletonInstance("Galgame", 1, 0, "GameManager", &gameManager);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    if (!loadStartupQml(engine, config.getStartupSceneUrl())) {
        return -1;
    }

    QQuickWindow* rootWindow = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
    if (rootWindow != nullptr) {
        gameManager.attachRenderWindow(rootWindow);
    } else {
        qWarning() << "Root QML object is not a QQuickWindow; frame loop not attached";
    }

    QObject::connect(&app, &QCoreApplication::aboutToQuit, &shutdownAndLogStats);
    return app.exec();
}

