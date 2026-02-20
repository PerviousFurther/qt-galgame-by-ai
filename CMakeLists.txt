cmake_minimum_required(VERSION 3.16)

project(qt-galgame VERSION 1.0.0 LANGUAGES CXX)
project(qt-galgame-by-ai VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6 packages - REQUIRED since we use Qt containers throughout
find_package(Qt6 COMPONENTS Core Qml Quick Gui Multimedia QUIET)
if (NOT Qt6_FOUND)
    message(FATAL_ERROR
        "Qt6 was not found. On Ubuntu, install dependencies with:\n"
        "  ./scripts/install_qt6_deps_ubuntu.sh\n"
        "or set Qt6_DIR/CMAKE_PREFIX_PATH to your Qt installation.")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
message(STATUS "Qt6 found, building with Qt support")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    resources/resources.qrc
    src/scene/Item.cpp
    src/scene/PlayableItem.cpp
    src/scene/AudioItem.cpp
    src/scene/VideoItem.cpp
    src/scene/CharacterItem.cpp
    src/scene/Scene.cpp
    src/core/Execution.cpp
    src/core/Configuration.cpp
    src/core/GameManager.cpp
    src/factory/Registration.cpp
    src/factory/NativeItemFactory.cpp
    src/resources/Resource.cpp
    src/resources/TextureResource.cpp
    src/resources/JsonResource.cpp
    src/resources/QmlResource.cpp
    src/resources/MediaResource.cpp
    src/resources/Loader.cpp
    src/resources/Resources.cpp
)

# Header files
set(HEADERS
    include/scene/Item.h
    include/scene/PlayableItem.h
    include/scene/AudioItem.h
    include/scene/VideoItem.h
    include/scene/CharacterItem.h
    include/scene/Scene.h
    include/core/Execution.h
    include/core/Configuration.h
    include/core/GameManager.h
    include/factory/Factory.h
    include/factory/Registration.h
    include/factory/NativeItemFactory.h
    include/resources/Resource.h
    include/resources/FormatSupport.h
    include/resources/TextureResource.h
    include/resources/JsonResource.h
    include/resources/QmlResource.h
    include/resources/MediaResource.h
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
    Qt6::Gui
    Qt6::Multimedia
)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Qt deployment - automatically copy required Qt DLLs after build
if(WIN32)
    # Find windeployqt executable
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
    
    if(WINDEPLOYQT_EXECUTABLE)
        message(STATUS "Found windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
        
        # Run windeployqt after build
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --qmldir "${CMAKE_SOURCE_DIR}"
                --no-compiler-runtime
                --no-system-d3d-compiler
                --no-translations
                "$<TARGET_FILE:${PROJECT_NAME}>"
            COMMENT "Deploying Qt dependencies..."
        )
    else()
        message(WARNING "windeployqt not found. Qt DLLs will not be automatically deployed.")
    endif()
elseif(APPLE)
    # Find macdeployqt executable
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
    
    if(MACDEPLOYQT_EXECUTABLE)
        message(STATUS "Found macdeployqt: ${MACDEPLOYQT_EXECUTABLE}")
        
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${MACDEPLOYQT_EXECUTABLE}"
                "$<TARGET_FILE:${PROJECT_NAME}>"
                -qmldir="${CMAKE_SOURCE_DIR}"
            COMMENT "Deploying Qt dependencies..."
        )
    else()
        message(WARNING "macdeployqt not found. Qt frameworks will not be automatically deployed.")
    endif()
endif()