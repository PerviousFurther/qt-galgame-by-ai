// Example QML scene file (stub)
// This will be used to load scenes using Qt QML

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtMultimedia
import Galgame 1.0

Item {
    id: root
    width: 1280
    height: 720
    property bool happyExpression: false

    AudioOutput {
        id: bgmAudioOutput
        volume: 0.8
    }

    AudioOutput {
        id: videoAudioOutput
        volume: 0.8
    }

    MediaPlayer {
        id: bgmPlayer
        source: "resources/audio/opening.mp3"
        audioOutput: bgmAudioOutput
    }

    MediaPlayer {
        id: openingVideoPlayer
        source: "resources/video/opening.mp4"
        audioOutput: videoAudioOutput
        videoOutput: openingVideoOutput
    }

    // Background
    Image {
        id: background
        anchors.fill: parent
        source: "resources/background.png"
    }

    // Character sprite
    Image {
        id: character
        x: 100
        y: 200
        source: root.happyExpression ? "resources/character_happy.png" : "resources/character.png"
        width: 320
        height: 480
    }

    VideoOutput {
        id: openingVideoOutput
        anchors.right: parent.right
        anchors.top: parent.top
        width: parent.width * 0.45
        height: parent.height * 0.45
    }

    // Dialog box
    Rectangle {
        id: dialogBox
        anchors.bottom: parent.bottom
        width: parent.width
        height: 200
        color: "#80000000"

        Row {
            anchors.centerIn: parent
            spacing: 10

            Button {
                text: bgmPlayer.playbackState === MediaPlayer.PlayingState ? "Pause BGM" : "Play BGM"
                onClicked: {
                    if (bgmPlayer.playbackState === MediaPlayer.PlayingState) {
                        bgmPlayer.pause()
                    } else {
                        bgmPlayer.play()
                    }
                }
            }

            Button {
                text: openingVideoPlayer.playbackState === MediaPlayer.PlayingState ? "Pause Video" : "Play Video"
                onClicked: {
                    if (openingVideoPlayer.playbackState === MediaPlayer.PlayingState) {
                        openingVideoPlayer.pause()
                    } else {
                        openingVideoPlayer.play()
                    }
                }
            }

            Button {
                text: qsTr("Toggle Portrait Expression")
                onClicked: root.happyExpression = !root.happyExpression
            }

            Button {
                text: qsTr("Start Game")
                onClicked: GameBridge.startGame()
            }

            Button {
                text: qsTr("Pause Game")
                onClicked: GameBridge.pauseGame()
            }

            Button {
                text: qsTr("Resume Game")
                onClicked: GameBridge.resumeGame()
            }

            Button {
                text: qsTr("Stop Game")
                onClicked: GameBridge.stopGame()
            }

            Text {
                text: qsTr("State: %1  Scene: %2").arg(GameBridge.gameState).arg(GameBridge.activeScene)
                color: "#ffffff"
            }
        }
    }
}
