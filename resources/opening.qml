import QtQuick 2.15
import QtQuick.Controls 2.15
import Galgame 1.0

Item {
    id: openingRoot
    anchors.fill: parent

    function finishOpening() {
        Configuration.openingAnimationPlayed = true
        if (!Configuration.saveConfig()) {
            console.warn("Failed to save config after opening animation")
        }
        GameManager.requestScreen("menu")
    }

    Rectangle {
        anchors.fill: parent
        color: "#0a0a0a"

        Column {
            anchors.centerIn: parent
            spacing: 24

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "牛逼之争"
                font.pixelSize: 80
                font.bold: true
                color: "#ffffff"
                opacity: 0.0

                NumberAnimation on opacity {
                    from: 0.0; to: 1.0; duration: 2000; running: true
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "一个关于游戏之争与友谊的故事"
                font.pixelSize: 26
                color: "#aaaaaa"
                opacity: 0.0

                SequentialAnimation on opacity {
                    running: true
                    PauseAnimation { duration: 1500 }
                    NumberAnimation { from: 0.0; to: 1.0; duration: 1500 }
                }
            }
        }

        Text {
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 40
            text: "点击任意处继续"
            font.pixelSize: 18
            color: "#666666"
            opacity: 0.0

            SequentialAnimation on opacity {
                running: true
                PauseAnimation { duration: 3000 }
                NumberAnimation { from: 0.0; to: 1.0; duration: 800 }
            }
        }

        Button {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 16
            text: qsTr("跳过")
            onClicked: openingRoot.finishOpening()
        }

        MouseArea {
            anchors.fill: parent
            onClicked: openingRoot.finishOpening()
        }
    }

    Timer {
        interval: 5000
        running: true
        onTriggered: openingRoot.finishOpening()
    }
}
