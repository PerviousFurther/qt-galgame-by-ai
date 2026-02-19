// Example QML scene file (stub)
// This will be used to load scenes using Qt QML

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtMultimedia

Item {
    id: root
    width: 1280
    height: 720
    property bool happyExpression: false

    AudioOutput {
        id: bgmAudioOutput
        volume: 0.8
    }

    MediaPlayer {
        id: bgmPlayer
        source: "audio/opening.mp3"
        audioOutput: bgmAudioOutput
    }

    MediaPlayer {
        id: openingVideoPlayer
        source: "video/opening.mp4"
        audioOutput: bgmAudioOutput
        videoOutput: openingVideoOutput
    }

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
        source: root.happyExpression ? "character_happy.png" : "character.png"
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
                text: "切换立绘表情"
                onClicked: root.happyExpression = !root.happyExpression
            }
        }
    }
}
