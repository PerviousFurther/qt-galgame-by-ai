import QtQuick 2.15
import QtQuick.Window 2.15
import Galgame 1.0

Window {
    id: rootWindow
    width: 1280
    height: 720
    visible: true
    title: Configuration.applicationName

    property string currentScreen: ""

    Component.onCompleted: {
        if (Configuration.openingAnimationPlayed) {
            currentScreen = "menu"
        } else {
            currentScreen = "opening"
        }
    }

    Connections {
        target: GameManager
        function onScreenChangeRequested(screen) {
            rootWindow.currentScreen = screen
        }
    }

    Loader {
        id: screenLoader
        anchors.fill: parent
        source: {
            if (rootWindow.currentScreen === "opening") return "qrc:/opening.qml"
            if (rootWindow.currentScreen === "menu")    return "qrc:/mainmenu.qml"
            if (rootWindow.currentScreen === "game")    return "qrc:/game.qml"
            return ""
        }
    }
}
