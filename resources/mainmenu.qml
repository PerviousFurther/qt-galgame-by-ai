import QtQuick 2.15
import QtQuick.Controls 2.15
import Galgame 1.0

Item {
    id: menuRoot
    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#1a1a2e" }
            GradientStop { position: 1.0; color: "#16213e" }
        }

        Column {
            anchors.centerIn: parent
            spacing: 16

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "牛 逼 之 争"
                font.pixelSize: 64
                font.bold: true
                color: "#ffffff"
                font.letterSpacing: 8
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "一个关于游戏之争与友谊的故事"
                font.pixelSize: 22
                color: "#8899aa"
            }

            Item { width: 1; height: 48 }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 220; height: 56
                text: qsTr("开始游戏")
                font.pixelSize: 22
                onClicked: GameManager.startGame(0)
            }

            Button {
                id: loadButton
                anchors.horizontalCenter: parent.horizontalCenter
                width: 220; height: 56
                text: qsTr("读取游戏")
                font.pixelSize: 22
                enabled: GameManager.hasSaves()
                onClicked: GameManager.startGame(GameManager.savedStep)

                ToolTip.visible: hovered && !enabled
                ToolTip.text: qsTr("暂无存档")
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 220; height: 56
                text: qsTr("退出游戏")
                font.pixelSize: 22
                onClicked: Qt.quit()
            }
        }
    }
}
