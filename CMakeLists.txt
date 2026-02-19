cmake_minimum_required(VERSION 3.16)

project(qt-galgame VERSION 1.0.0 LANGUAGES CXX)
project(qt-galgame-by-ai VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 packages - REQUIRED since we use Qt containers throughout
find_package(Qt6 COMPONENTS Core Qml Quick REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
message(STATUS "Qt6 found, building with Qt support")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/scene/Item.cpp
    src/scene/Scene.cpp
    src/core/Timer.cpp
    src/core/Configuration.cpp
    src/core/GameManager.cpp
    src/factory/Registration.cpp
    src/factory/NativeItemFactory.cpp
    src/resources/Resource.cpp
    src/resources/Loader.cpp
    src/resources/Resources.cpp
)

# Header files
set(HEADERS
    include/scene/Item.h
    include/scene/Scene.h
    include/core/Timer.h
    include/core/Configuration.h
    include/core/GameManager.h
    include/factory/Factory.h
    include/factory/Registration.h
    include/factory/NativeItemFactory.h
    include/resources/Resource.h
    include/resources/Loader.h
    include/resources/Resources.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link Qt6 libraries (now required)
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Qml
    Qt6::Quick
)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
