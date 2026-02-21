import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Galgame 1.0

Item {
    id: gameRoot
    anchors.fill: parent
    property var gameConstants: GameManager.getGameConstants()
    property real sceneOffsetX: 0

    // ── Story data ──────────────────────────────────────────────────────────
    // Each entry is one displayable step in the story.
    // Fields:
    //   shot          : shot number (int 1-7)
    //   shotTitle     : title shown during transition (string, optional)
    //   bg            : background colour (string)
    //   shake         : screen-shake active (bool)
    //   transition    : this step opens a new shot → hide HUD, fade (bool)
    //   charA/B/C     : { visible, emotion, side }  emotion: normal/angry/furious/surprised/happy/calm
    //   type          : "narration" | "dialogue" | "ending"
    //   speaker       : character display name (string, dialogue only)
    //   speakerChar   : "A" | "B" | "C" | "" (all)
    //   text          : display text
    property var fallbackStoryData: gameConstants.storyData !== undefined ? gameConstants.storyData : []
    property var storyData: fallbackStoryData

    // ── State ──────────────────────────────────────────────────────────────
    property int  currentStep:    GameManager.currentStoryStep
    readonly property var currentStepData: storyData.length > 0 ? storyData[Math.min(currentStep, storyData.length - 1)] : ({})
    readonly property int currentShot:    currentStepData.shot !== undefined ? currentStepData.shot : 1
    property bool inTransition:   false
    property bool fastForward:    false
    property bool hudVisible:     true
    readonly property int fastForwardIntervalMs: 600

    property var  visitedShots:   []
    readonly property var charMeta: gameConstants.charMeta !== undefined ? gameConstants.charMeta : ({
        A: { name: "凯瑟琳", symbol: "🤠", baseColor: "#8B5E3C" },
        B: { name: "蕾妮",   symbol: "🦅", baseColor: "#2F6FA8" },
        C: { name: "梦雪",   symbol: "🔮", baseColor: "#6A3FA8" }
    })

    function scheduleAutoAdvance() {
        autoAdvanceTimer.stop()
        if (storyData.length === 0 || currentStep >= storyData.length - 1) {
            return
        }
        if (currentStepData.autoAdvanceMs !== undefined && currentStepData.autoAdvanceMs > 0) {
            autoAdvanceTimer.interval = currentStepData.autoAdvanceMs
            autoAdvanceTimer.start()
        }
    }

    function showSettingsInScene() {
        if (settingsPopup.opened) {
            return
        }
        settingsSceneOverlay.visible = true
        settingsSceneOverlayAnimation.restart()
    }

    function advance() {
        if (inTransition) return
        const advanceResult = GameManager.advanceStory(storyData, visitedShots)
        if (advanceResult.advanced !== true) return
        currentStep = advanceResult.nextStep
        visitedShots = advanceResult.visitedShots
        if (advanceResult.shotChanged === true) {
            doTransition(advanceResult.nextStep, advanceResult.transitionStyle)
        } else {
            scheduleAutoAdvance()
        }
    }

    function doTransition(nextStepIdx, style) {
        inTransition = true
        hudVisible   = false
        transitionTimer.nextStep = nextStepIdx
        transitionTimer.style = style
        if (style === "slide_ltr") {
            transitionTimer.interval = gameConstants.sceneTransition !== undefined &&
                                       gameConstants.sceneTransition.slideSwitchDelayMs !== undefined
                                       ? gameConstants.sceneTransition.slideSwitchDelayMs : 250
        } else {
            transitionTimer.interval = gameConstants.sceneTransition !== undefined &&
                                       gameConstants.sceneTransition.fadeSwitchDelayMs !== undefined
                                       ? gameConstants.sceneTransition.fadeSwitchDelayMs : 400
            fadeOverlay.opacity = 1.0
        }
        transitionTimer.start()
    }

    Component.onCompleted: {
        currentStep = GameManager.currentStoryStep
        // Record initial shot as visited
        if (storyData.length > 0) {
            visitedShots = [storyData[currentStep].shot]
        }
        scheduleAutoAdvance()
    }

    onCurrentStepChanged: scheduleAutoAdvance()

    // Fast-forward timer
    Timer {
        id: ffTimer
        interval: gameRoot.fastForwardIntervalMs
        repeat: true
        running: gameRoot.fastForward
        onTriggered: {
            if (gameRoot.currentStep < gameRoot.storyData.length - 1) {
                gameRoot.advance()
            } else {
                gameRoot.fastForward = false
            }
        }
    }

    Timer {
        id: autoAdvanceTimer
        interval: 3000
        repeat: false
        onTriggered: gameRoot.advance()
    }

    // Transition timer: switch content mid-fade, then fade back in
    Timer {
        id: transitionTimer
        interval: 400
        property int nextStep: 0
        property string style: "fade"
        onTriggered: {
            // GameManager.currentStoryStep was already updated in advance()
            // before the transition began; only the local mirror needs syncing.
            gameRoot.currentStep = nextStep
            if (style === "slide_ltr") {
                gameRoot.sceneOffsetX = -sceneContent.width
                sceneSlideAnimation.start()
            } else {
                fadeOverlay.opacity = 0.0
                transitionEndTimer.start()
            }
        }
    }

    Timer {
        id: transitionEndTimer
        interval: 400
        onTriggered: {
            gameRoot.inTransition = false
            gameRoot.hudVisible   = true
        }
    }

    NumberAnimation {
        id: sceneSlideAnimation
        target: gameRoot
        property: "sceneOffsetX"
        to: 0
        duration: gameConstants.sceneTransition !== undefined &&
                  gameConstants.sceneTransition.slideLtrDurationMs !== undefined
                  ? gameConstants.sceneTransition.slideLtrDurationMs : 520
        onFinished: transitionEndTimer.start()
    }

    // ── Background ────────────────────────────────────────────────────────
    Rectangle {
        id: background
        anchors.fill: parent
        color: gameRoot.storyData.length > 0
               ? gameRoot.currentStepData.bg
               : "#87CEEB"

        Behavior on color { ColorAnimation { duration: 300 } }
    }

    // ── Scene content (characters + shot title) ────────────────────────────
    Item {
        id: sceneContent
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: dialogBox.top
        anchors.bottomMargin: 4
        transform: Translate { x: gameRoot.sceneOffsetX }

        // Shot title shown briefly during transitions
        Text {
            id: shotTitleText
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 20
            font.pixelSize: 28
            font.bold: true
            color: "#ffffff"
            style: Text.Outline
            styleColor: "#000000"
            opacity: gameRoot.inTransition ? 1.0 : 0.0
            text: gameRoot.storyData.length > 0 && gameRoot.currentStepData.shotTitle !== undefined
                  ? gameRoot.currentStepData.shotTitle : ""

            Behavior on opacity { NumberAnimation { duration: 300 } }
        }

        Item {
            id: parentSceneLayer
            anchors.fill: parent

            Item {
                id: childSceneLayer
                anchors.fill: parent

                Rectangle {
                    id: magicBeam
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: gameConstants.magicBeam !== undefined && gameConstants.magicBeam.width !== undefined
                           ? gameConstants.magicBeam.width : 26
                    height: gameConstants.magicBeam !== undefined && gameConstants.magicBeam.height !== undefined
                            ? gameConstants.magicBeam.height : 260
                    radius: width / 2
                    color: "#88CCFF"
                    border.color: "#DDF8FF"
                    border.width: 2
                    opacity: gameRoot.storyData.length > 0 &&
                             gameRoot.currentStepData.shot === 5 &&
                             gameRoot.currentStepData.charC !== undefined &&
                             gameRoot.currentStepData.charC.visible === true ? 0.92 : 0.0
                    y: beamDropAnimation.running ? beamDropAnimation.currentValue : -height

                    NumberAnimation {
                        id: beamDropAnimation
                        running: magicBeam.opacity > 0.0
                        loops: Animation.Infinite
                        from: -magicBeam.height
                        to: parent.height - magicBeam.height - 12
                        duration: gameConstants.magicBeam !== undefined && gameConstants.magicBeam.dropDurationMs !== undefined
                                  ? gameConstants.magicBeam.dropDurationMs : 1400
                    }
                }
            }
        }

        // Shake wrapper for shot 4
        Item {
            id: shakeWrapper
            anchors.fill: parent

            property real shakeX: 0.0
            property real shakeY: 0.0
            transform: Translate { x: shakeWrapper.shakeX; y: shakeWrapper.shakeY }

            SequentialAnimation {
                running: gameRoot.storyData.length > 0 && gameRoot.currentStepData.shake
                loops: Animation.Infinite
                NumberAnimation { target: shakeWrapper; property: "shakeX"; from: 0; to:  6; duration: 60 }
                NumberAnimation { target: shakeWrapper; property: "shakeX"; from: 6; to: -6; duration: 60 }
                NumberAnimation { target: shakeWrapper; property: "shakeX"; from: -6; to: 0; duration: 60 }
                NumberAnimation { target: shakeWrapper; property: "shakeY"; from: 0; to:  4; duration: 60 }
                NumberAnimation { target: shakeWrapper; property: "shakeY"; from: 4; to: -4; duration: 60 }
                NumberAnimation { target: shakeWrapper; property: "shakeY"; from: -4; to: 0; duration: 60 }
            }

            // ── Character A (left) ──────────────────────────────────────────
            Column {
                id: charADisplay
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.leftMargin: 80
                anchors.bottomMargin: 10
                spacing: 6
                visible: gameRoot.currentStepData.charA !== undefined && gameRoot.currentStepData.charA.visible === true
                opacity: visible ? 1.0 : 0.0
                Behavior on opacity { NumberAnimation { duration: 200 } }

                Rectangle {
                    width: 160; height: 300
                    radius: 12
                    color: GameManager.emotionColor(gameRoot.currentStepData.charA !== undefined ? (gameRoot.currentStepData.charA.emotion || "normal") : "normal",
                                                    gameRoot.charMeta["A"].baseColor)
                    border.color: Qt.darker(color, 1.5)
                    border.width: 3

                    Column {
                        anchors.centerIn: parent
                        spacing: 8
                        Text { anchors.horizontalCenter: parent.horizontalCenter; text: gameRoot.charMeta["A"].symbol; font.pixelSize: 56 }
                        Text { anchors.horizontalCenter: parent.horizontalCenter; text: GameManager.emotionEmoji(gameRoot.currentStepData.charA !== undefined ? (gameRoot.currentStepData.charA.emotion || "normal") : "normal"); font.pixelSize: 36 }
                    }
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: gameRoot.charMeta["A"].name
                    font.pixelSize: 16; font.bold: true; color: "#ffffff"
                    style: Text.Outline; styleColor: "#000000"
                }
            }

            // ── Character C (center) ───────────────────────────────────────
            Column {
                id: charCDisplay
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
                spacing: 6
                visible: gameRoot.currentStepData.charC !== undefined && gameRoot.currentStepData.charC.visible === true
                opacity: visible ? 1.0 : 0.0
                Behavior on opacity { NumberAnimation { duration: 200 } }

                Rectangle {
                    width: 160; height: 300
                    radius: 12
                    color: GameManager.emotionColor(gameRoot.currentStepData.charC !== undefined ? (gameRoot.currentStepData.charC.emotion || "normal") : "normal",
                                                    gameRoot.charMeta["C"].baseColor)
                    border.color: Qt.darker(color, 1.5)
                    border.width: 3

                    Column {
                        anchors.centerIn: parent
                        spacing: 8
                        Text { anchors.horizontalCenter: parent.horizontalCenter; text: gameRoot.charMeta["C"].symbol; font.pixelSize: 56 }
                        Text { anchors.horizontalCenter: parent.horizontalCenter; text: GameManager.emotionEmoji(gameRoot.currentStepData.charC !== undefined ? (gameRoot.currentStepData.charC.emotion || "normal") : "normal"); font.pixelSize: 36 }
                    }
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: gameRoot.charMeta["C"].name
                    font.pixelSize: 16; font.bold: true; color: "#ffffff"
                    style: Text.Outline; styleColor: "#000000"
                }
            }

            // ── Character B (right) ────────────────────────────────────────
            Column {
                id: charBDisplay
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.rightMargin: 80
                anchors.bottomMargin: 10
                spacing: 6
                visible: gameRoot.currentStepData.charB !== undefined && gameRoot.currentStepData.charB.visible === true
                opacity: visible ? 1.0 : 0.0
                Behavior on opacity { NumberAnimation { duration: 200 } }

                Rectangle {
                    width: 160; height: 300
                    radius: 12
                    color: GameManager.emotionColor(gameRoot.currentStepData.charB !== undefined ? (gameRoot.currentStepData.charB.emotion || "normal") : "normal",
                                                    gameRoot.charMeta["B"].baseColor)
                    border.color: Qt.darker(color, 1.5)
                    border.width: 3

                    Column {
                        anchors.centerIn: parent
                        spacing: 8
                        Text { anchors.horizontalCenter: parent.horizontalCenter; text: gameRoot.charMeta["B"].symbol; font.pixelSize: 56 }
                        Text { anchors.horizontalCenter: parent.horizontalCenter; text: GameManager.emotionEmoji(gameRoot.currentStepData.charB !== undefined ? (gameRoot.currentStepData.charB.emotion || "normal") : "normal"); font.pixelSize: 36 }
                    }
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: gameRoot.charMeta["B"].name
                    font.pixelSize: 16; font.bold: true; color: "#ffffff"
                    style: Text.Outline; styleColor: "#000000"
                }
            }
        }
    }

    // ── Dialog box ────────────────────────────────────────────────────────
    Rectangle {
        id: dialogBox
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 190
        color: "#CC000000"
        visible: gameRoot.storyData.length > 0 && gameRoot.currentStepData.type !== "ending"

        // Speaker name plate
        Rectangle {
            id: namePlate
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.topMargin: -20
            anchors.leftMargin: 30
            width: speakerLabel.implicitWidth + 32
            height: 36
            radius: 6
            color: "#CC333355"
            visible: gameRoot.storyData.length > 0 &&
                     gameRoot.currentStepData.type === "dialogue" &&
                     gameRoot.currentStepData.speaker !== undefined

            Text {
                id: speakerLabel
                anchors.centerIn: parent
                text: gameRoot.storyData.length > 0 && gameRoot.currentStepData.speaker !== undefined
                      ? gameRoot.currentStepData.speaker : ""
                font.pixelSize: 20; font.bold: true; color: "#ffffff"
            }
        }

        Text {
            id: dialogText
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: continueHint.top
            anchors.margins: 24
            text: gameRoot.storyData.length > 0 ? gameRoot.currentStepData.text : ""
            font.pixelSize: 24
            color: "#ffffff"
            wrapMode: Text.Wrap
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            id: continueHint
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 16
            text: "▼"
            font.pixelSize: 18
            color: "#aaaaaa"
            visible: gameRoot.currentStep < gameRoot.storyData.length - 1

            SequentialAnimation on opacity {
                loops: Animation.Infinite
                running: continueHint.visible
                NumberAnimation { from: 1.0; to: 0.3; duration: 600 }
                NumberAnimation { from: 0.3; to: 1.0; duration: 600 }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (gameRoot.currentStep < gameRoot.storyData.length - 1) {
                    gameRoot.advance()
                }
            }
        }
    }

    // ── HUD buttons ───────────────────────────────────────────────────────
    Row {
        id: hudButtons
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 12
        spacing: 8
        visible: gameRoot.hudVisible && !gameRoot.inTransition

        Button {
            text: "💾 存档"
            font.pixelSize: 14
            onClicked: {
                GameManager.save()
                saveNotice.visible = true
                saveNoticeTimer.restart()
            }
        }

        Button {
            text: "⚙ 设置"
            font.pixelSize: 14
            onClicked: gameRoot.showSettingsInScene()
        }

        Button {
            text: gameRoot.fastForward ? "⏩ 快进中" : "⏩ 快进"
            font.pixelSize: 14
            checkable: true
            checked: gameRoot.fastForward
            onClicked: gameRoot.fastForward = !gameRoot.fastForward
        }

        Button {
            text: "🗺 路径图"
            font.pixelSize: 14
            onClicked: routeMapPopup.open()
        }

        Button {
            text: "← 主菜单"
            font.pixelSize: 14
            onClicked: GameManager.currentScreen = "menu"
        }
    }

    // Save notice
    Text {
        id: saveNotice
        anchors.top: hudButtons.bottom
        anchors.right: parent.right
        anchors.margins: 12
        text: "存档已保存 ✓"
        font.pixelSize: 16
        color: "#88ff88"
        visible: false

        Timer {
            id: saveNoticeTimer
            interval: 2000
            onTriggered: saveNotice.visible = false
        }
    }

    // ── Ending overlay ────────────────────────────────────────────────────
    Rectangle {
        id: endingOverlay
        anchors.fill: parent
        color: "#CC000000"
        visible: gameRoot.storyData.length > 0 && gameRoot.currentStepData.type === "ending"

        Column {
            anchors.centerIn: parent
            spacing: 40

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: gameRoot.storyData.length > 0 && gameRoot.currentStepData.type === "ending"
                      ? gameRoot.currentStepData.text : ""
                font.pixelSize: 48
                font.bold: true
                color: "#FFD700"
                style: Text.Outline
                styleColor: "#000000"
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("返回主菜单")
                font.pixelSize: 24
                onClicked: GameManager.currentScreen = "menu"
            }
        }
    }

    // ── Transition fade overlay ────────────────────────────────────────────
    Rectangle {
        id: fadeOverlay
        anchors.fill: parent
        color: "#000000"
        opacity: 0.0

        Behavior on opacity { NumberAnimation { duration: 380 } }
    }

    // ── Click-to-advance on scene area ────────────────────────────────────
    MouseArea {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: dialogBox.visible ? dialogBox.top : parent.bottom
        onClicked: {
            if (!gameRoot.inTransition && gameRoot.currentStep < gameRoot.storyData.length - 1) {
                gameRoot.advance()
            }
        }
    }

    // ── Settings popup ────────────────────────────────────────────────────
    Rectangle {
        id: settingsSceneOverlay
        anchors.fill: parent
        color: "#55000000"
        visible: false
        opacity: 0.0
    }

    SequentialAnimation {
        id: settingsSceneOverlayAnimation
        NumberAnimation { target: settingsSceneOverlay; property: "opacity"; from: 0.0; to: 1.0; duration: 180 }
        ScriptAction { script: settingsPopup.open() }
        NumberAnimation { target: settingsSceneOverlay; property: "opacity"; from: 1.0; to: 0.0; duration: 180 }
        ScriptAction { script: settingsSceneOverlay.visible = false }
    }

    Popup {
        id: settingsPopup
        anchors.centerIn: parent
        width: 400; height: 300
        modal: true

        background: Rectangle { color: "#222233"; radius: 12; border.color: "#555577"; border.width: 2 }

        Column {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 20

            Text { text: "设置"; font.pixelSize: 28; font.bold: true; color: "#ffffff" }

            Row {
                spacing: 16
                Text { text: "主音量"; color: "#cccccc"; font.pixelSize: 18; anchors.verticalCenter: parent.verticalCenter }
                Slider {
                    width: 200
                    from: 0.0; to: 1.0
                    value: Configuration.masterVolume
                    onMoved: Configuration.masterVolume = value
                }
            }

            Button {
                text: qsTr("关闭")
                onClicked: settingsPopup.close()
            }
        }
    }

    // ── Route map popup ───────────────────────────────────────────────────
    Popup {
        id: routeMapPopup
        anchors.centerIn: parent
        width: 520; height: 420
        modal: true

        background: Rectangle { color: "#222233"; radius: 12; border.color: "#555577"; border.width: 2 }

        Column {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16

            Text { text: "路径图"; font.pixelSize: 28; font.bold: true; color: "#ffffff" }

            Grid {
                columns: 4
                spacing: 10

                Repeater {
                    model: GameManager.buildRouteShots(gameRoot.storyData)

                    Rectangle {
                        width: 110; height: 70
                        radius: 8
                        color: gameRoot.visitedShots.includes(modelData.num) ? "#334455" : "#1a1a2a"
                        border.color: gameRoot.currentShot === modelData.num ? "#FFD700" : "#444466"
                        border.width: gameRoot.currentShot === modelData.num ? 3 : 1

                        Column {
                            anchors.centerIn: parent
                            spacing: 4
                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: "镜头 " + modelData.num
                                font.pixelSize: 14; font.bold: true
                                color: gameRoot.visitedShots.includes(modelData.num) ? "#ffffff" : "#666688"
                            }
                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: modelData.title
                                font.pixelSize: 13
                                color: gameRoot.visitedShots.includes(modelData.num) ? "#aaaacc" : "#444466"
                            }
                        }
                    }
                }
            }

            Button {
                text: qsTr("关闭")
                onClicked: routeMapPopup.close()
            }
        }
    }
}
