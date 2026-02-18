#include "scene/Scene.h"
#include "scene/Item.h"
#include "core/Timer.h"
#include "core/Configuration.h"
#include "core/GameManager.h"
#include "factory/Registration.h"
#include "factory/NativeItemFactory.h"
#include "resources/Resources.h"
#include <QDebug>
#include <QThread>

int main(int argc, char *argv[]) {
    qDebug() << "Qt Galgame Engine - Visual Novel Development Framework";
    qDebug() << "======================================================";
    qDebug() << "";

    // Step 1: Initialize Configuration (first step in any application)
    qDebug() << "=== Initializing Configuration ===";
    Configuration& config = Configuration::getInstance();
    config.parseCommandLine(argc, argv);
    config.loadFromFile(QStringLiteral("resources/config.json"));
    
    qDebug() << "Configuration loaded:";
    qDebug() << "  Window:" << config.getWindowWidth() << "x" << config.getWindowHeight();
    qDebug() << "  Target FPS:" << config.getTargetFPS();
    qDebug() << "  Master Volume:" << config.getMasterVolume();
    qDebug() << "";

    // Step 2: Initialize Timer
    qDebug() << "=== Initializing Timer ===";
    Timer& timer = Timer::getInstance();
    timer.initialize();
    timer.setFixedUpdateInterval(1.0f / 60.0f);  // 60 FPS for fixed updates
    qDebug() << "Timer initialized (fixed update:" << timer.getFixedUpdateInterval() << "s)";
    qDebug() << "";

    // Step 3: Register factories for Item creation
    qDebug() << "=== Registering Factories ===";
    Registration& registration = Registration::getInstance();
    auto nativeFactory = QSharedPointer<NativeItemFactory>::create();
    registration.registerFactory(nativeFactory);
    qDebug() << "Native Item factory registered";
    qDebug() << "Registered types:" << registration.getRegisteredTypes();
    qDebug() << "";

    // Step 4: Initialize Resources
    qDebug() << "=== Initializing Resources ===";
    Resources& resources = Resources::getInstance();
    qDebug() << "Resources system initialized with default loaders";
    qDebug() << "";

    // Step 5: Initialize GameManager
    qDebug() << "=== Initializing GameManager ===";
    GameManager& gameManager = GameManager::getInstance();
    gameManager.initialize();
    qDebug() << "";

    // Step 6: Create scenes
    qDebug() << "=== Creating Scenes ===";
    
    // Create a dialog scene
    auto dialogScene = QSharedPointer<Scene>::create();
    dialogScene->setId(QStringLiteral("dialog_scene"));
    
    // Create items using factory
    PropertyMap bgProps = {{QStringLiteral("type"), QStringLiteral("Item")}, {QStringLiteral("id"), QStringLiteral("background")}, {QStringLiteral("name"), QStringLiteral("Background")}};
    auto bg = registration.createItem(QStringLiteral("Native"), bgProps);
    
    PropertyMap charProps = {{QStringLiteral("type"), QStringLiteral("Item")}, {QStringLiteral("id"), QStringLiteral("character")}, {QStringLiteral("name"), QStringLiteral("Main Character")}};
    auto character = registration.createItem(QStringLiteral("Native"), charProps);
    
    dialogScene->addItem(bg);
    dialogScene->addItem(character);
    
    gameManager.addScene(QStringLiteral("dialog"), dialogScene);
    gameManager.setActiveScene(QStringLiteral("dialog"));
    qDebug() << "";

    // Step 7: Simulate game loop
    qDebug() << "=== Starting Game Loop ===";
    gameManager.start();
    
    // Simulate a few frames
    for (int frame = 0; frame < 5; ++frame) {
        timer.update();
        
        qDebug() << "Frame" << frame << ":"
                 << "deltaTime=" << timer.getDeltaTime() << "s,"
                 << "runtime=" << timer.getRuntime() << "s";
        
        // Regular update
        gameManager.update();
        
        // Fixed update (if needed)
        if (timer.shouldFixedUpdate()) {
            qDebug() << "  -> Fixed update triggered";
            gameManager.fixedUpdate();
        }
        
        // Simulate frame delay
        QThread::msleep(16);  // ~60 FPS
    }
    qDebug() << "";

    // Step 8: Test resource loading
    qDebug() << "=== Testing Resource Loading ===";
    qDebug() << "Note: Image decoding not yet implemented, showing stub behavior";
    
    // Sync loading
    auto texture = resources.load(QStringLiteral("file://resources/test_image.png"));
    if (texture) {
        qDebug() << "Loaded texture:" << texture->getUrl();
    }
    
    // Async loading demo
    qDebug() << "Starting async load...";
    resources.loadAsync(QStringLiteral("file://resources/test_image2.png"), 
        [](QSharedPointer<Resource> res, bool success) {
            if (success) {
                qDebug() << "Async load completed:" << res->getUrl();
            } else {
                qDebug() << "Async load failed";
            }
        });
    
    // Wait for async load
    QThread::msleep(100);
    qDebug() << "";

    // Step 9: Show stats
    qDebug() << "=== Engine Statistics ===";
    qDebug() << "Total frames:" << timer.getFrameCount();
    qDebug() << "Total runtime:" << timer.getRuntime() << "s";
    qDebug() << "Cached resources:" << resources.getCacheSize() << "bytes";
    qDebug() << "Active scene:" << gameManager.getActiveSceneName();
    qDebug() << "";

    gameManager.stop();
    
    qDebug() << "=== Demonstration Completed Successfully! ===";
    qDebug() << "";
    qDebug() << "Architecture summary:";
    qDebug() << "  ✓ Timer singleton for frame timing";
    qDebug() << "  ✓ Configuration singleton for settings";
    qDebug() << "  ✓ Registration singleton for Item factories";
    qDebug() << "  ✓ Resources singleton for asset management";
    qDebug() << "  ✓ GameManager singleton for game flow";
    qDebug() << "  ✓ Scene inheritance support for specialized scenes";
    qDebug() << "  ✓ Item update() and fixedUpdate() methods";
    qDebug() << "  ✓ Async resource loading with callbacks";

    return 0;
}

