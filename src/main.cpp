#include "codingstyle.h" // include/codingstyle.h
#include "scene/Scene.h"
#include "scene/Item.h"
#include "core/Timer.h"
#include "core/Configuration.h"
#include "core/GameManager.h"
#include "factory/Registration.h"
#include "factory/NativeItemFactory.h"
#include "resources/Loader.h"
#include "resources/Resources.h"
#include <QDebug>
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

    // Step 2: Initialize Timer
    qDebug() << "=== Initializing Timer ===";
    Timer& timer = Timer::getInstance();
    timer.initialize();
    timer.setFixedUpdateInterval(1.0f / 60.0f);  // 60 FPS for fixed updates
    qDebug() << "Timer initialized (fixed update:" << timer.getFixedUpdateInterval() << "s)";

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
    auto bg = registration.createItem("Native", bgProps);
    
    PropertyMap charProps = {
        {"type", "Character"},
        {"id", "character"},
        {"name", "Main Character"},
        {"source", "resources/character.png"},
        {"expression", "neutral"}
    };
    auto character = registration.createItem("Native", charProps);

    PropertyMap bgmProps = {
        {"type", "Audio"},
        {"id", "bgm"},
        {"name", "Background Music"},
        {"source", "resources/audio/opening.mp3"},
        {"loop", true}
    };
    auto bgm = registration.createItem("Native", bgmProps);

    PropertyMap videoProps = {
        {"type", "Video"},
        {"id", "opening_video"},
        {"name", "Opening Video"},
        {"source", "resources/video/opening.mp4"},
        {"loop", false}
    };
    auto openingVideo = registration.createItem("Native", videoProps);
    
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
        timer.update();
        
        qDebug() << "Frame" << frame << ":" << "deltaTime=" << timer.getDeltaTime() << "runtime=" << timer.getRuntime();
        
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

    // Step 8: Test resource loading
    qDebug() << "=== Testing Resource Loading ===";
    resources.addResource("opening_bitmap", "resources/background.png");
    resources.addResource("opening_video", "resources/video/opening.mp4");
    resources.addResource("scene_descriptor", "qrc:/scene.json");
    QVariant bitmapData = resources.load("opening_bitmap");
    if (bitmapData.isValid()) {
        qDebug() << "Loaded bitmap resource via named loader";
    }
    if (!resources.getLoader("opening_video").isNull()) {
        qDebug() << "Resolved named video loader successfully";
    }
    if (!resources.getLoader("opening_bitmap").isNull()) {
        qDebug() << "Opening bitmap loader initialized:" << resources.getLoader("opening_bitmap")->isInitialized();
    }

    // Step 9: Show stats
    qDebug() << "=== Engine Statistics ===";
    qDebug() << "Total frames:" << timer.getFrameCount();
    qDebug() << "Total runtime:" << timer.getRuntime() << "s";
    qDebug() << "Active scene:" << gameManager.getActiveSceneName();

    gameManager.stop();
    
    qDebug() << "=== Demonstration Completed Successfully! ===";
    qDebug() << "Architecture summary:";
    qDebug() << "  ✓ Timer singleton for frame timing";
    qDebug() << "  ✓ Configuration singleton for settings";
    qDebug() << "  ✓ Registration singleton for Item factories";
    qDebug() << "  ✓ Resources singleton for named loader asset management";
    qDebug() << "  ✓ GameManager singleton for game flow";
    qDebug() << "  ✓ Scene inheritance support for specialized scenes";
    qDebug() << "  ✓ Item update() and fixedUpdate() methods";

    return 0;
}
