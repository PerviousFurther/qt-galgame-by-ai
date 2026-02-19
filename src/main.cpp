#include "scene/Scene.h"
#include "scene/Item.h"
#include "core/Timer.h"
#include "core/Configuration.h"
#include "core/GameManager.h"
#include "factory/Registration.h"
#include "factory/NativeItemFactory.h"
#include "resources/Resources.h"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

int main(int argc, char *argv[]) {
    std::cout << "Qt Galgame Engine - Visual Novel Development Framework" << std::endl;
    std::cout << "======================================================" << std::endl;
    std::cout << std::endl;

    // Step 1: Initialize Configuration (first step in any application)
    std::cout << "=== Initializing Configuration ===" << std::endl;
    Configuration& config = Configuration::getInstance();
    config.parseCommandLine(argc, argv);
    config.loadFromFile("resources/config.json");
    
    std::cout << "Configuration loaded:" << std::endl;
    std::cout << "  Window: " << config.getWindowWidth() << "x" << config.getWindowHeight() << std::endl;
    std::cout << "  Target FPS: " << config.getTargetFPS() << std::endl;
    std::cout << "  Master Volume: " << config.getMasterVolume() << std::endl;
    std::cout << std::endl;

    // Step 2: Initialize Timer
    std::cout << "=== Initializing Timer ===" << std::endl;
    Timer& timer = Timer::getInstance();
    timer.initialize();
    timer.setFixedUpdateInterval(1.0f / 60.0f);  // 60 FPS for fixed updates
    std::cout << "Timer initialized (fixed update: " << timer.getFixedUpdateInterval() << "s)" << std::endl;
    std::cout << std::endl;

    // Step 3: Register factories for Item creation
    std::cout << "=== Registering Factories ===" << std::endl;
    Registration& registration = Registration::getInstance();
    auto nativeFactory = std::make_shared<NativeItemFactory>();
    registration.registerFactory(nativeFactory);
    std::cout << "Native Item factory registered" << std::endl;
    std::cout << "Registered types: ";
    for (const auto& type : registration.getRegisteredTypes()) {
        std::cout << type << " ";
    }
    std::cout << std::endl << std::endl;

    // Step 4: Initialize Resources
    std::cout << "=== Initializing Resources ===" << std::endl;
    Resources& resources = Resources::getInstance();
    std::cout << "Resources system initialized with default loaders" << std::endl;
    std::cout << std::endl;

    // Step 5: Initialize GameManager
    std::cout << "=== Initializing GameManager ===" << std::endl;
    GameManager& gameManager = GameManager::getInstance();
    gameManager.initialize();
    std::cout << std::endl;

    // Step 6: Create scenes
    std::cout << "=== Creating Scenes ===" << std::endl;
    
    // Create a dialog scene
    auto dialogScene = std::make_shared<Scene>();
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
        audioItem->play();
        std::cout << "Audio item ready: " << audioItem->getSource().toStdString()
                  << ", playing=" << audioItem->isPlaying() << std::endl;
    }
    if (auto videoItem = openingVideo.dynamicCast<VideoItem>()) {
        videoItem->play();
        std::cout << "Video item ready: " << videoItem->getSource().toStdString()
                  << ", playing=" << videoItem->isPlaying() << std::endl;
    }
    
    gameManager.addScene("dialog", dialogScene);
    gameManager.setActiveScene("dialog");
    std::cout << std::endl;

    // Step 7: Simulate game loop
    std::cout << "=== Starting Game Loop ===" << std::endl;
    gameManager.start();
    
    // Simulate a few frames
    for (int frame = 0; frame < 5; ++frame) {
        timer.update();
        
        std::cout << "Frame " << frame << ": ";
        std::cout << "deltaTime=" << timer.getDeltaTime() << "s, ";
        std::cout << "runtime=" << timer.getRuntime() << "s" << std::endl;
        
        // Regular update
        gameManager.update();
        
        // Fixed update (if needed)
        if (timer.shouldFixedUpdate()) {
            std::cout << "  -> Fixed update triggered" << std::endl;
            gameManager.fixedUpdate();
        }
        
        // Simulate frame delay
        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
    }
    std::cout << std::endl;

    // Step 8: Test resource loading
    std::cout << "=== Testing Resource Loading ===" << std::endl;
    std::cout << "Note: Image decoding not yet implemented, showing stub behavior" << std::endl;
    
    // Sync loading
    auto texture = resources.load("file://resources/test_image.png");
    if (texture) {
        std::cout << "Loaded texture: " << texture->getUrl() << std::endl;
    }
    
    // Async loading demo
    std::cout << "Starting async load..." << std::endl;
    resources.loadAsync("file://resources/test_image2.png", 
        [](std::shared_ptr<Resource> res, bool success) {
            if (success) {
                std::cout << "Async load completed: " << res->getUrl() << std::endl;
            } else {
                std::cout << "Async load failed" << std::endl;
            }
        });
    
    // Wait for async load
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << std::endl;

    // Step 9: Show stats
    std::cout << "=== Engine Statistics ===" << std::endl;
    std::cout << "Total frames: " << timer.getFrameCount() << std::endl;
    std::cout << "Total runtime: " << timer.getRuntime() << "s" << std::endl;
    std::cout << "Cached resources: " << resources.getCacheSize() << " bytes" << std::endl;
    std::cout << "Active scene: " << gameManager.getActiveSceneName() << std::endl;
    std::cout << std::endl;

    gameManager.stop();
    
    std::cout << "=== Demonstration Completed Successfully! ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Architecture summary:" << std::endl;
    std::cout << "  ✓ Timer singleton for frame timing" << std::endl;
    std::cout << "  ✓ Configuration singleton for settings" << std::endl;
    std::cout << "  ✓ Registration singleton for Item factories" << std::endl;
    std::cout << "  ✓ Resources singleton for asset management" << std::endl;
    std::cout << "  ✓ GameManager singleton for game flow" << std::endl;
    std::cout << "  ✓ Scene inheritance support for specialized scenes" << std::endl;
    std::cout << "  ✓ Item update() and fixedUpdate() methods" << std::endl;
    std::cout << "  ✓ Async resource loading with callbacks" << std::endl;

    return 0;
}
