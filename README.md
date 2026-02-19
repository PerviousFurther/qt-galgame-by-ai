# Qt Galgame Engine

A visual novel (galgame) development engine built with C++ and Qt6.

## Overview

This project provides a flexible framework for creating visual novels and interactive story-driven games. The engine is designed with a modular singleton-based architecture with support for different scene types (dialog scenes, mini-game scenes, settings scenes).

## Architecture

### Singleton Systems

#### Timer
Global timer singleton for managing frame timing:
- Provides delta time between frames
- Tracks total runtime and frame count
- Supports fixed update intervals for physics/mini-games
- FPS calculation
- Items get timing information from `Timer::getInstance()`

#### Configuration
Settings management singleton:
- Loads configuration from JSON files and command line arguments
- Manages audio volumes (master, music, sound effects, voice)
- Window settings (dimensions, fullscreen)
- Render settings (target FPS, V-Sync)
- Generic key-value storage for extensibility
- Can be modified through UI (see `resources/menu.qml`)

#### Registration
Factory pattern singleton for creating Items:
- Register/unregister Factory objects
- Create Items dynamically from JSON/QML properties
- Native Item factory included with extensibility comments
- Support for custom Item types through factory registration

#### Resources
Resource management singleton:
- Abstract Loader and Resource classes
- Supports multiple protocols (file://, qrc://)
- Async loading with callbacks for large files
- Resource types: TextureResource, AudioResource, JsonResource, QmlResource, MediaResource
- Automatic resource caching
- Memory management with unload capabilities

#### GameManager
Central game logic controller:
- Manages game state (running, paused, stopped)
- Container for multiple Scenes
- Scene switching and lifecycle management
- Game event system with callbacks
- Coordinates updates between systems

### Core Components

#### Item
Base class for all scene objects:
- Unique identification system (ID and name)
- Lifecycle management: `initialize()`, `update()`, `fixedUpdate()`, `cleanup()`
- `update()`: Called every frame, gets delta time from Timer singleton
- `fixedUpdate()`: Called at fixed intervals for physics/mini-games
- Extensible through inheritance

#### Scene
Container for Items with support for specialization:
- Manages collections of Items
- Supports loading from QML or JSON files
- Handles connections between Items transparently
- Can be inherited for specialized scenes (DialogScene, MiniGameScene, SettingsScene)
- Provides `update()` and `fixedUpdate()` for all contained Items

## Project Structure

```
qt-galgame/
├── CMakeLists.txt              # CMake build configuration
├── include/
│   ├── core/                   # Core singletons
│   │   ├── Timer.h            # Frame timing management
│   │   ├── Configuration.h    # Settings management
│   │   └── GameManager.h      # Game logic controller
│   ├── factory/               # Factory pattern for Items
│   │   ├── Factory.h          # Abstract factory base
│   │   ├── Registration.h     # Factory registry
│   │   └── NativeItemFactory.h # Built-in Item factory
│   ├── resources/             # Resource management
│   │   ├── Resource.h         # Resource types (Texture, Audio, etc.)
│   │   ├── Loader.h           # Resource loaders (file, qrc protocols)
│   │   └── Resources.h        # Resource manager singleton
│   └── scene/
│       ├── Item.h             # Item base class
│       └── Scene.h            # Scene container
├── src/                       # Implementation files
│   ├── main.cpp               # Application entry point
│   ├── core/
│   ├── factory/
│   ├── resources/
│   └── scene/
└── resources/                 # Game resources
    ├── config.json            # Default configuration
    ├── menu.qml               # Settings UI
    ├── scene.json             # Example JSON scene
    └── scene.qml              # Example QML scene
```

## Building the Project

### Prerequisites

- CMake 3.16 or higher
- C++20 compatible compiler
- Qt6 development packages (Core, Qml, Quick, Gui, Multimedia)

For Ubuntu/Debian, install dependencies with:

```bash
./scripts/install_qt6_deps_ubuntu.sh
```

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure the project
cmake ..

# Build the project
cmake --build .

# Run the application
./bin/qt-galgame
```

If CMake still cannot locate Qt6, set `Qt6_DIR` or `CMAKE_PREFIX_PATH` to your Qt installation path.

## Usage Example

### Basic Setup

```cpp
#include "core/Timer.h"
#include "core/Configuration.h"
#include "core/GameManager.h"
#include "factory/Registration.h"
#include "factory/NativeItemFactory.h"
#include "resources/Resources.h"

int main(int argc, char* argv[]) {
    // 1. Initialize Configuration (first step)
    Configuration& config = Configuration::getInstance();
    config.parseCommandLine(argc, argv);
    config.loadFromFile("resources/config.json");
    
    // 2. Initialize Timer
    Timer& timer = Timer::getInstance();
    timer.initialize();
    
    // 3. Register Item factories
    Registration& registration = Registration::getInstance();
    registration.registerFactory(std::make_shared<NativeItemFactory>());
    
    // 4. Initialize Resources
    Resources& resources = Resources::getInstance();
    
    // 5. Initialize GameManager
    GameManager& gameManager = GameManager::getInstance();
    gameManager.initialize();
    
    // 6. Create and add scenes
    auto scene = std::make_shared<Scene>();
    gameManager.addScene("main", scene);
    gameManager.setActiveScene("main");
    
    // 7. Game loop
    gameManager.start();
    while (gameManager.getState() == GameManager::State::Running) {
        timer.update();
        gameManager.update();
        
        if (timer.shouldFixedUpdate()) {
            gameManager.fixedUpdate();
        }
    }
    
    return 0;
}
```

### Creating Items with Factory

```cpp
// Create items from properties (simulating JSON/QML loading)
PropertyMap props = {
    {"type", "Item"},
    {"id", "character"},
    {"name", "Main Character"}
};

auto item = Registration::getInstance().createItem("Native", props);
scene->addItem(item);
```

### Loading Resources

```cpp
// Synchronous loading
auto texture = Resources::getInstance().load("file://textures/bg.png");

// Asynchronous loading (for large files)
Resources::getInstance().loadAsync("file://audio/bgm.mp3",
    [](std::shared_ptr<Resource> res, bool success) {
        if (success) {
            // Use the loaded resource
        }
    });
```

### Custom Item Types

```cpp
// 1. Create your Item subclass
class MyCustomItem : public Item {
public:
    void update() override {
        float deltaTime = Timer::getInstance().getDeltaTime();
        // Your custom logic here
    }
    
    void fixedUpdate() override {
        // Physics or time-critical updates
    }
};

// 2. Create a factory for your Item
class MyCustomFactory : public Factory {
public:
    std::shared_ptr<Item> create(const PropertyMap& properties) override {
        auto item = std::make_shared<MyCustomItem>();
        // Parse properties and configure item
        return item;
    }
    
    std::string getTypeName() const override {
        return "MyCustom";
    }
};

// 3. Register the factory
Registration::getInstance().registerFactory(
    std::make_shared<MyCustomFactory>()
);
```

## Features

### Implemented ✅
- Timer singleton for frame timing and fixed updates
- Configuration singleton for settings management
- Registration singleton with factory pattern for Items
- Resources singleton with async loading support
- GameManager singleton for game flow control
- Item lifecycle: initialize, update, fixedUpdate, cleanup
- Scene management with inheritance support
- Resource types: Texture, Audio, ChatSession
- Loader support for file:// and qrc:// protocols
- Event system for game events
- Configuration UI (menu.qml)

### In Progress 🚧
- JSON parsing for configuration and scenes
- QML scene loading integration
- Image decoding (stb_image or similar)
- Audio playback system
- Live2D support

### Planned 📋
- Animation system
- Dialog system with chat trees
- Save/Load system
- Mini-game framework
- Input management
- UI components

## Extending the Engine

### Adding New Native Item Types

See `include/factory/NativeItemFactory.h` for detailed instructions. The factory includes extensive comments on how to add new native Item types.

### Creating Custom Loaders

Inherit from `Loader` or combine `FileLoader`/`QrcLoader` with your format-specific logic:

```cpp
class FileAudioLoader : public FileLoader {
public:
    bool canLoad(const std::string& url) const override {
        return FileLoader::canLoad(url) && isAudioExtension(getExtension(url));
    }
    
    std::shared_ptr<Resource> loadSync(const std::string& url) override {
        // Your audio loading logic
    }
};

// Register with Resources
Resources::getInstance().registerLoader(
    std::make_shared<FileAudioLoader>()
);
```

## License

This project is created using Qt6 and GitHub Copilot.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.
