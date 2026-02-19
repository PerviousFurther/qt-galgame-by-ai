#include "core/Configuration.h"
#include "core/Execution.h"
#include "core/GameManager.h"
#include "factory/NativeItemFactory.h"
#include "factory/Registration.h"
#include "resources/Resources.h"

#include <QDebug>
#include <QGuiApplication>
#include <QPointer>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include <QSharedPointer>
#include <QUrl>
#include <atomic>
#include <qqml.h>

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    qDebug() << "Qt Galgame Engine - Visual Novel Development Framework";
    qDebug() << "======================================================";

    qDebug() << "=== Initializing Configuration ===";
    Configuration& config = Configuration::getInstance();
    config.parseCommandLine(argc, argv);
    app.setApplicationName(config.getApplicationName());
    config.loadFromFile(config.getConfigResourceUrl());

    qDebug() << "=== Initializing Execution ===";
    Execution& execution = Execution::getInstance();
    execution.initialize();
    const int targetFps = config.getTargetFPS() > 0 ? config.getTargetFPS() : 60;
    execution.setFixedUpdateInterval(1.0f / static_cast<float>(targetFps));

    qDebug() << "=== Registering Factories ===";
    Registration& registration = Registration::getInstance();
    registration.registerFactory(QSharedPointer<NativeItemFactory>::create());

    qDebug() << "=== Initializing Resources ===";
    Resources::getInstance();

    qDebug() << "=== Initializing GameManager ===";
    GameManager& gameManager = GameManager::getInstance();
    gameManager.initialize();

    QObject::connect(&app, &QGuiApplication::applicationStateChanged, [&gameManager](Qt::ApplicationState state) {
        if (state == Qt::ApplicationActive && gameManager.getState() == GameManager::State::Paused) {
            gameManager.resume();
        } else if (state != Qt::ApplicationActive && gameManager.getState() == GameManager::State::Running) {
            gameManager.pause();
        }
    });

    qmlRegisterSingletonInstance("Galgame", 1, 0, "Configuration", &config);
    qmlRegisterSingletonInstance("Galgame", 1, 0, "GameManager", &gameManager);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    const QString startupSceneUrl = config.getStartupSceneUrl();
    if (startupSceneUrl.isEmpty()) {
        qWarning() << "Startup scene URL is empty";
        return -1;
    }

    engine.load(QUrl(startupSceneUrl));
    if (engine.rootObjects().isEmpty()) {
        qWarning() << "Failed to load QML scene:" << startupSceneUrl;
        return -1;
    }

    qDebug() << "=== Starting Frame-Driven Game Loop ===";
    QQuickWindow* rootWindow = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
    if (rootWindow != nullptr) {
        auto updateInProgress = QSharedPointer<std::atomic_bool>::create(false);
        QObject::connect(rootWindow, &QQuickWindow::afterAnimating, &app,
                         [&execution, &gameManager, updateInProgress,
                          trackedWindow = QPointer<QQuickWindow>(rootWindow)]() {
            if (updateInProgress->exchange(true)) {
                return;
            }
            execution.update();
            gameManager.update();
            if (execution.shouldFixedUpdate()) {
                gameManager.fixedUpdate();
            }
            updateInProgress->store(false);
            if (!trackedWindow.isNull()) {
                trackedWindow->requestUpdate();
            }
        });
        rootWindow->requestUpdate();
    } else {
        qWarning() << "Root QML object is not a QQuickWindow; frame loop not attached";
    }

    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&execution, &gameManager]() {
        qDebug() << "=== Engine Statistics ===";
        qDebug() << "Total frames:" << execution.getFrameCount();
        qDebug() << "Total runtime:" << execution.getRuntime() << "s";
        qDebug() << "Active scene:" << gameManager.getActiveSceneName();
        gameManager.stop();
    });

    return app.exec();
}
