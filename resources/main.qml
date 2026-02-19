import QtQuick 2.15
import QtQuick.Window 2.15
import Galgame 1.0

Window {
    id: rootWindow
    width: 1280
    height: 720
    visible: true
    title: Configuration.applicationName

    Loader {
        anchors.fill: parent
        source: "qrc:/scene.qml"
    }
}
