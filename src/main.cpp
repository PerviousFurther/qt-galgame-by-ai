#include "scene/Scene.h"
#include "scene/Item.h"
#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    std::cout << "Qt Galgame Engine - Visual Novel Development Framework" << std::endl;
    std::cout << "======================================================" << std::endl;
    std::cout << std::endl;

    // Create a scene
    Scene mainScene;
    mainScene.setId("main_scene");
    
    std::cout << "Created scene with ID: " << mainScene.getId() << std::endl;

    // Create some items
    auto item1 = std::make_shared<Item>();
    item1->setId("background");
    item1->setName("Background Image");

    auto item2 = std::make_shared<Item>();
    item2->setId("character");
    item2->setName("Main Character");

    auto item3 = std::make_shared<Item>();
    item3->setId("dialog_box");
    item3->setName("Dialog Box");

    // Add items to scene
    std::cout << "Adding items to scene..." << std::endl;
    mainScene.addItem(item1);
    mainScene.addItem(item2);
    mainScene.addItem(item3);

    std::cout << "Total items in scene: " << mainScene.getItems().size() << std::endl;

    // Initialize the scene
    std::cout << "Initializing scene..." << std::endl;
    mainScene.initialize();

    // Simulate an update cycle
    std::cout << "Running update cycle..." << std::endl;
    mainScene.update(0.016f); // ~60 FPS

    // Retrieve an item by ID
    auto foundItem = mainScene.getItem("character");
    if (foundItem) {
        std::cout << "Found item: " << foundItem->getName() << std::endl;
    }

    // Test loading (stubs for now)
    std::cout << std::endl;
    std::cout << "Testing scene loading capabilities:" << std::endl;
    mainScene.loadFromJson("resources/scene.json");
    mainScene.loadFromQml("resources/scene.qml");

    std::cout << std::endl;
    std::cout << "Basic demonstration completed successfully!" << std::endl;

    return 0;
}
