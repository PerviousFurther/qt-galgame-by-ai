// Example QML scene file (stub)
// This will be used to load scenes using Qt QML

import QtQuick 2.15

Item {
    id: root
    width: 1280
    height: 720

    // Background
    Image {
        id: background
        anchors.fill: parent
        source: "background.png"
    }

    // Character sprite
    Image {
        id: character
        x: 100
        y: 200
        source: "character.png"
    }

    // Dialog box
    Rectangle {
        id: dialogBox
        anchors.bottom: parent.bottom
        width: parent.width
        height: 200
        color: "#80000000"
    }
}
