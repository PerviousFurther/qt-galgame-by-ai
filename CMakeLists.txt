cmake_minimum_required(VERSION 3.16)

project(qt-galgame VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 packages
find_package(Qt6 COMPONENTS Core Qml Quick QUIET)

if(Qt6_FOUND)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    message(STATUS "Qt6 found, building with Qt support")
else()
    message(WARNING "Qt6 not found, building without Qt support (some features may be limited)")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/scene/Item.cpp
    src/scene/Scene.cpp
)

# Header files
set(HEADERS
    include/scene/Item.h
    include/scene/Scene.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link Qt6 libraries if found
if(Qt6_FOUND)
    target_link_libraries(${PROJECT_NAME}
        Qt6::Core
        Qt6::Qml
        Qt6::Quick
    )
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
