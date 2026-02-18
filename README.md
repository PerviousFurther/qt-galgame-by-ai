# Qt Galgame Engine

A visual novel (galgame) development engine built with C++ and Qt6.

## Overview

This project provides a flexible framework for creating visual novels and interactive story-driven games. The engine is designed with a modular architecture centered around Items and Scenes.

## Architecture

### Core Components

#### Item
The `Item` class is the base class for all objects that can be placed in a scene. Most objects in the engine inherit from this class, providing:
- Unique identification system
- Lifecycle management (initialize, update, cleanup)
- Base functionality for scene objects

#### Scene
The `Scene` class serves as a container for Items and manages their interactions:
- Manages collections of Items
- Handles connections between Items transparently
- Supports loading scenes from QML or JSON files
- Items don't need to directly interact with each other; the Scene handles their relationships

## Project Structure

```
qt-galgame/
├── CMakeLists.txt          # CMake build configuration
├── include/
│   └── scene/
│       ├── Item.h          # Item base class header
│       └── Scene.h         # Scene container header
├── src/
│   ├── main.cpp            # Application entry point
│   └── scene/
│       ├── Item.cpp        # Item implementation
│       └── Scene.cpp       # Scene implementation
└── resources/              # Game resources (images, scenes, etc.)
    ├── scene.json          # Example JSON scene definition
    └── scene.qml           # Example QML scene definition
```

## Building the Project

### Prerequisites

- CMake 3.16 or higher
- C++17 compatible compiler
- Qt6 (Core, Qml, Quick modules) - optional but recommended

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

### Building without Qt6

The project can be built without Qt6, but some features (QML loading) will be limited:

```bash
cmake -DQt6_DIR=/path/to/qt6 ..
```

## Usage Example

```cpp
#include "scene/Scene.h"
#include "scene/Item.h"

// Create a scene
Scene mainScene;
mainScene.setId("main_scene");

// Create items
auto background = std::make_shared<Item>();
background->setId("background");
background->setName("Background Image");

// Add items to scene
mainScene.addItem(background);

// Initialize and run
mainScene.initialize();
mainScene.update(deltaTime);
```

## Features

- ✅ Basic Item and Scene architecture
- ✅ Item lifecycle management
- ✅ Scene item management (add, remove, retrieve)
- 🚧 JSON scene loading (stub implemented)
- 🚧 QML scene loading (stub implemented)
- 📋 Future: Resource management system
- 📋 Future: Event system for item communication
- 📋 Future: Animation system
- 📋 Future: Audio system

## License

This project is created using Qt6 and GitHub Copilot.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.
