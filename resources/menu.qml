// Settings/Menu Scene for the Galgame Engine
// This QML file defines the UI for game settings
// It interacts with the Configuration singleton to modify settings

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: settingsRoot
    width: 1280
    height: 720

    Rectangle {
        anchors.fill: parent
        color: "#2a2a2a"

        ColumnLayout {
            anchors.centerIn: parent
            width: parent.width * 0.6
            spacing: 20

            Text {
                text: "Settings"
                font.pixelSize: 48
                color: "#ffffff"
                Layout.alignment: Qt.AlignHCenter
            }

            // Audio Settings Section
            GroupBox {
                title: "Audio Settings"
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10

                    Label {
                        text: "Master Volume"
                        color: "#ffffff"
                    }
                    Slider {
                        id: masterVolumeSlider
                        from: 0
                        to: 1.0
                        value: 1.0
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "Music Volume"
                        color: "#ffffff"
                    }
                    Slider {
                        id: musicVolumeSlider
                        from: 0
                        to: 1.0
                        value: 0.8
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "Sound Effects Volume"
                        color: "#ffffff"
                    }
                    Slider {
                        id: sfxVolumeSlider
                        from: 0
                        to: 1.0
                        value: 0.8
                        Layout.fillWidth: true
                    }

                    Label {
                        text: "Voice Volume"
                        color: "#ffffff"
                    }
                    Slider {
                        id: voiceVolumeSlider
                        from: 0
                        to: 1.0
                        value: 1.0
                        Layout.fillWidth: true
                    }
                }
            }

            // Video Settings Section
            GroupBox {
                title: "Video Settings"
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 10

                    CheckBox {
                        text: "Fullscreen"
                        checked: false
                    }

                    CheckBox {
                        text: "V-Sync"
                        checked: true
                    }

                    Label {
                        text: "Target FPS"
                        color: "#ffffff"
                    }
                    SpinBox {
                        from: 30
                        to: 144
                        value: 60
                        Layout.fillWidth: true
                    }
                }
            }

            // Action Buttons
            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 20

                Button {
                    text: "Apply"
                    onClicked: {
                        // TODO: Apply settings to Configuration singleton
                        console.log("Settings applied")
                    }
                }

                Button {
                    text: "Reset to Defaults"
                    onClicked: {
                        // TODO: Reset Configuration to defaults
                        console.log("Settings reset")
                    }
                }

                Button {
                    text: "Back"
                    onClicked: {
                        // TODO: Return to previous scene
                        console.log("Back to main menu")
                    }
                }
            }
        }
    }
}
