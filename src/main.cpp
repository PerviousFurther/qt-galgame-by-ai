#include "codingstyle.h" // include/codingstyle.h
#include "scene/Scene.h"
#include "scene/Item.h"
#include "core/Execution.h"
#include "core/Configuration.h"
#include "core/GameManager.h"
#include "factory/Registration.h"
#include "factory/NativeItemFactory.h"
#include "resources/Loader.h"
#include "resources/Resources.h"
#include <QDebug>
#include <QObject>
#include <QThread>

int main(int argc, char *argv[]) {
    qDebug() << "Qt Galgame Engine - Visual Novel Development Framework";
    qDebug() << "======================================================";

    // Step 1: Initialize Configuration (first step in any application)
    qDebug() << "=== Initializing Configuration ===";
    Configuration& config = Configuration::getInstance();
    config.parseCommandLine(argc, argv);
    config.loadFromFile("resources/config.json");
    
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

    // Step 6: Create scenes
    qDebug() << "=== Creating Scenes ===";
    
    // Create a dialog scene
    auto dialogScene = QSharedPointer<Scene>::create();
    dialogScene->setId("dialog_scene");
    
    // Create items using factory
    PropertyMap bgProps = {{"type", "Item"}, {"id", "background"}, {"name", "Background"}};
    auto bgObject = registration.create("Native", bgProps);
    if (bgObject.isNull()) {
        qWarning() << "Failed to create background object";
        return 1;
    }
    auto bg = bgObject.dynamicCast<Item>();
    if (bg.isNull()) {
        qWarning() << "Background object is not Item";
        return 1;
    }
    
    PropertyMap charProps = {
        {"type", "Character"},
        {"id", "character"},
        {"name", "Main Character"},
        {"source", "resources/character.png"},
        {"expression", "neutral"}
    };
    auto characterObject = registration.create("Native", charProps);
    if (characterObject.isNull()) {
        qWarning() << "Failed to create character object";
        return 1;
    }
    auto character = characterObject.dynamicCast<Item>();
    if (character.isNull()) {
        qWarning() << "Character object is not Item";
        return 1;
    }

    PropertyMap bgmProps = {
        {"type", "Audio"},
        {"id", "bgm"},
        {"name", "Background Music"},
        {"source", "resources/audio/opening.mp3"},
        {"loop", true}
    };
    auto bgmObject = registration.create("Native", bgmProps);
    if (bgmObject.isNull()) {
        qWarning() << "Failed to create bgm object";
        return 1;
    }
    auto bgm = bgmObject.dynamicCast<Item>();
    if (bgm.isNull()) {
        qWarning() << "BGM object is not Item";
        return 1;
    }

    PropertyMap videoProps = {
        {"type", "Video"},
        {"id", "opening_video"},
        {"name", "Opening Video"},
        {"source", "resources/video/opening.mp4"},
        {"loop", false}
    };
    auto openingVideoObject = registration.create("Native", videoProps);
    if (openingVideoObject.isNull()) {
        qWarning() << "Failed to create opening video object";
        return 1;
    }
    auto openingVideo = openingVideoObject.dynamicCast<Item>();
    if (openingVideo.isNull()) {
        qWarning() << "Opening video object is not Item";
        return 1;
    }
    
    dialogScene->addItem(bg);
    dialogScene->addItem(character);
    dialogScene->addItem(bgm);
    dialogScene->addItem(openingVideo);

    if (auto audioItem = bgm.dynamicCast<AudioItem>()) {
        QObject::connect(audioItem.data(), &AudioItem::playRequested, []() {
            qDebug() << "BGM play requested";
        });
        audioItem->play();
        qDebug() << "Audio item ready:" << audioItem->getSource() << "playing=" << audioItem->isPlaying();
    }
    if (auto videoItem = openingVideo.dynamicCast<VideoItem>()) {
        QObject::connect(videoItem.data(), &VideoItem::playRequested, []() {
            qDebug() << "Video play requested";
        });
        videoItem->play();
        qDebug() << "Video item ready:" << videoItem->getSource() << "playing=" << videoItem->isPlaying();
    }
    
    gameManager.addScene("dialog", dialogScene);
    gameManager.setActiveScene("dialog");

    // Step 7: Simulate game loop
    qDebug() << "=== Starting Game Loop ===";
    gameManager.start();
    
    // Simulate a few frames
    for (int frame = 0; frame < 5; ++frame) {
        execution.update();
        
        qDebug() << "Frame" << frame << ":" << "deltaTime=" << execution.getDeltaTime() << "runtime=" << execution.getRuntime();
        
        // Regular update
        gameManager.update();
        
        // Fixed update (if needed)
        if (execution.shouldFixedUpdate()) {
            qDebug() << "  -> Fixed update triggered";
            gameManager.fixedUpdate();
        }
        
        // Simulate frame delay
        QThread::msleep(16);  // ~60 FPS
    }

    // Step 8: Test resource loading
    qDebug() << "=== Testing Resource Loading ===";
    resources.addResource("opening_bitmap", "resources/background.png");
    resources.addResource("opening_video", "resources/video/opening.mp4");
    resources.addResource("scene_descriptor", "resources/scene.json");
    QSharedPointer<Loader> bitmapLoader = resources.getLoader("opening_bitmap");
    if (!bitmapLoader.isNull()) {
        QObject::connect(bitmapLoader.data(), &Loader::loadFinished, [](Loader* loader) {
            qDebug() << "Loaded bitmap resource via loader. initialized=" << loader->isInitialized()
                     << "payload=" << loader->get();
        });
        bitmapLoader->load({}, false);
        qDebug() << "Resolved named bitmap loader successfully, load requested";
    }
    QSharedPointer<Loader> videoLoader = resources.getLoader("opening_video");
    if (!videoLoader.isNull()) {
        QObject::connect(videoLoader.data(), &Loader::loadFinished, [](Loader* loader) {
            qDebug() << "Loaded video resource via loader. initialized=" << loader->isInitialized()
                     << "payload=" << loader->get();
        });
        videoLoader->load({}, true);
        qDebug() << "Resolved named video loader successfully, load requested";
    }
    if (!resources.getLoader("opening_bitmap").isNull()) {
        qDebug() << "Opening bitmap loader initialized:" << resources.getLoader("opening_bitmap")->isInitialized();
    }

    // Step 9: Show stats
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

    return 0;
}
