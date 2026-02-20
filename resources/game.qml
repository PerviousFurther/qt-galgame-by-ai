import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Galgame 1.0

Item {
    id: gameRoot
    anchors.fill: parent

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
    property var storyData: [
        // ── Shot 1: 开场 ───────────────────────────────────────────────────
        { shot: 1, shotTitle: "镜头一：开场", bg: "#87CEEB", shake: false,
          transition: true,
          charA: { visible: true,  emotion: "normal",  side: "left"  },
          charB: { visible: true,  emotion: "normal",  side: "right" },
          charC: { visible: false, emotion: "normal",  side: "center" },
          type: "narration", text: "空旷的广场，两位原创角色面对面站立。" },

        { shot: 1, bg: "#87CEEB", shake: false, transition: false,
          charA: { visible: true,  emotion: "angry",  side: "left"  },
          charB: { visible: true,  emotion: "normal", side: "right" },
          charC: { visible: false, emotion: "normal", side: "center" },
          type: "dialogue", speaker: "凯瑟琳", speakerChar: "A",
          text: "原神牛逼！" },

        // ── Shot 2: 对峙 ───────────────────────────────────────────────────
        { shot: 2, shotTitle: "镜头二：对峙", bg: "#4A90D9", shake: false,
          transition: true,
          charA: { visible: false, emotion: "normal", side: "left"   },
          charB: { visible: true,  emotion: "angry",  side: "center" },
          charC: { visible: false, emotion: "normal", side: "center" },
          type: "narration", text: "镜头切到角色B，表情同样愤怒。" },

        { shot: 2, bg: "#4A90D9", shake: false, transition: false,
          charA: { visible: false, emotion: "normal", side: "left"   },
          charB: { visible: true,  emotion: "angry",  side: "center" },
          charC: { visible: false, emotion: "normal", side: "center" },
          type: "dialogue", speaker: "蕾妮", speakerChar: "B",
          text: "鸣潮牛逼！" },

        // ── Shot 3: 升级 ───────────────────────────────────────────────────
        { shot: 3, shotTitle: "镜头三：升级", bg: "#E8A020", shake: false,
          transition: true,
          charA: { visible: true,  emotion: "angry", side: "left"  },
          charB: { visible: true,  emotion: "angry", side: "right" },
          charC: { visible: false, emotion: "normal",side: "center" },
          type: "narration", text: "背景音乐紧张，镜头快速切换，营造即将打架的气氛。" },

        { shot: 3, bg: "#E8A020", shake: false, transition: false,
          charA: { visible: true,  emotion: "angry", side: "left"  },
          charB: { visible: false, emotion: "normal",side: "right" },
          charC: { visible: false, emotion: "normal",side: "center" },
          type: "dialogue", speaker: "凯瑟琳", speakerChar: "A",
          text: "原神牛逼！" },

        { shot: 3, bg: "#E8A020", shake: false, transition: false,
          charA: { visible: false, emotion: "normal",side: "left"  },
          charB: { visible: true,  emotion: "angry", side: "right" },
          charC: { visible: false, emotion: "normal",side: "center" },
          type: "dialogue", speaker: "蕾妮", speakerChar: "B",
          text: "鸣潮牛逼！" },

        { shot: 3, bg: "#E8A020", shake: false, transition: false,
          charA: { visible: true,  emotion: "furious", side: "left"  },
          charB: { visible: false, emotion: "normal",  side: "right" },
          charC: { visible: false, emotion: "normal",  side: "center" },
          type: "dialogue", speaker: "凯瑟琳", speakerChar: "A",
          text: "原神牛逼！！" },

        { shot: 3, bg: "#E8A020", shake: false, transition: false,
          charA: { visible: false, emotion: "normal",  side: "left"  },
          charB: { visible: true,  emotion: "furious", side: "right" },
          charC: { visible: false, emotion: "normal",  side: "center" },
          type: "dialogue", speaker: "蕾妮", speakerChar: "B",
          text: "鸣潮牛逼！！" },

        // ── Shot 4: 冲突临近 ───────────────────────────────────────────────
        { shot: 4, shotTitle: "镜头四：冲突临近", bg: "#D4380D", shake: true,
          transition: true,
          charA: { visible: true, emotion: "furious", side: "left"  },
          charB: { visible: true, emotion: "furious", side: "right" },
          charC: { visible: false,emotion: "normal",  side: "center" },
          type: "narration", text: "两人动作加剧，准备扑向对方。" },

        { shot: 4, bg: "#D4380D", shake: true, transition: false,
          charA: { visible: true, emotion: "furious", side: "left"  },
          charB: { visible: true, emotion: "furious", side: "right" },
          charC: { visible: false,emotion: "normal",  side: "center" },
          type: "narration",
          text: "角色A抬起拳头，角色B张开翅膀。画面抖动，制造紧张感。" },

        // ── Shot 5: 转折 ───────────────────────────────────────────────────
        { shot: 5, shotTitle: "镜头五：转折", bg: "#722ED1", shake: false,
          transition: true,
          charA: { visible: true, emotion: "furious", side: "left"   },
          charB: { visible: true, emotion: "furious", side: "right"  },
          charC: { visible: true, emotion: "calm",    side: "center" },
          type: "narration",
          text: "突然，角色C从画面中央冲入！她用魔法光束拉开两人。" },

        { shot: 5, bg: "#722ED1", shake: false, transition: false,
          charA: { visible: true, emotion: "surprised", side: "left"   },
          charB: { visible: true, emotion: "surprised", side: "right"  },
          charC: { visible: true, emotion: "calm",      side: "center" },
          type: "dialogue", speaker: "梦雪", speakerChar: "C",
          text: "终末地不牛逼？" },

        // ── Shot 6: 顿悟 ───────────────────────────────────────────────────
        { shot: 6, shotTitle: "镜头六：顿悟", bg: "#52C41A", shake: false,
          transition: true,
          charA: { visible: true,  emotion: "surprised", side: "left"  },
          charB: { visible: true,  emotion: "surprised", side: "right" },
          charC: { visible: false, emotion: "normal",    side: "center" },
          type: "narration",
          text: "角色A和角色B愣住，表情从愤怒转为恍然大悟。" },

        { shot: 6, bg: "#52C41A", shake: false, transition: false,
          charA: { visible: true,  emotion: "happy", side: "left"  },
          charB: { visible: true,  emotion: "happy", side: "right" },
          charC: { visible: false, emotion: "normal",side: "center" },
          type: "narration",
          text: "两人互相看了一眼，突然笑了。音乐转为轻快搞笑。" },

        // ── Shot 7: 结尾 ───────────────────────────────────────────────────
        { shot: 7, shotTitle: "镜头七：结尾", bg: "#FFD700", shake: false,
          transition: true,
          charA: { visible: true, emotion: "happy", side: "left"   },
          charB: { visible: true, emotion: "happy", side: "right"  },
          charC: { visible: true, emotion: "happy", side: "center" },
          type: "narration", text: "三人并肩站在一起，面对屏幕。" },

        { shot: 7, bg: "#FFD700", shake: false, transition: false,
          charA: { visible: true, emotion: "happy", side: "left"   },
          charB: { visible: true, emotion: "happy", side: "right"  },
          charC: { visible: true, emotion: "happy", side: "center" },
          type: "dialogue", speaker: "全员", speakerChar: "",
          text: "终末地不牛逼！" },

        { shot: 7, bg: "#FFD700", shake: false, transition: false,
          charA: { visible: true, emotion: "happy", side: "left"   },
          charB: { visible: true, emotion: "happy", side: "right"  },
          charC: { visible: true, emotion: "happy", side: "center" },
          type: "ending", text: "友情才是最牛逼的！" }
    ]

    // ── State ──────────────────────────────────────────────────────────────
    property int  currentStep:    GameManager.currentStoryStep
    property int  currentShot:    storyData.length > 0 ? step().shot : 1
    property bool inTransition:   false
    property bool fastForward:    false
    property bool hudVisible:     true
    readonly property int fastForwardIntervalMs: 600

    property var  visitedShots:   []
    readonly property var charMeta: ({
        A: { name: "凯瑟琳", symbol: "🤠", baseColor: "#8B5E3C" },
        B: { name: "蕾妮",   symbol: "🦅", baseColor: "#2F6FA8" },
        C: { name: "梦雪",   symbol: "🔮", baseColor: "#6A3FA8" }
    })

    function emotionColor(emotion, base) {
        switch (emotion) {
        case "angry":     return Qt.darker(base, 1.3)
        case "furious":   return Qt.darker(base, 1.6)
        case "surprised": return Qt.lighter(base, 1.4)
        case "happy":     return Qt.lighter(base, 1.3)
        case "calm":      return base
        default:          return base
        }
    }

    function emotionEmoji(emotion) {
        switch (emotion) {
        case "angry":     return "😠"
        case "furious":   return "🤬"
        case "surprised": return "😲"
        case "happy":     return "😄"
        case "calm":      return "😌"
        default:          return "😐"
        }
    }

    function step() {
        return storyData[Math.min(currentStep, storyData.length - 1)]
    }

    function advance() {
        if (inTransition) return
        if (currentStep >= storyData.length - 1) return

        const nextStep = currentStep + 1
        const nextShot = storyData[nextStep].shot

        if (nextShot !== currentShot) {
            // Shot change → auto-save, hide HUD, transition
            GameManager.currentStoryStep = nextStep
            GameManager.save()
            if (!visitedShots.includes(nextShot)) {
                visitedShots = visitedShots.concat([nextShot])
            }
            doTransition(nextStep)
        } else {
            currentStep = nextStep
            GameManager.currentStoryStep = nextStep
        }
    }

    function doTransition(nextStepIdx) {
        inTransition = true
        hudVisible   = false
        fadeOverlay.opacity = 1.0
        transitionTimer.nextStep = nextStepIdx
        transitionTimer.start()
    }

    Component.onCompleted: {
        currentStep = GameManager.currentStoryStep
        // Record initial shot as visited
        if (storyData.length > 0) {
            visitedShots = [storyData[currentStep].shot]
        }
    }

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

    // Transition timer: switch content mid-fade, then fade back in
    Timer {
        id: transitionTimer
        interval: 400
        property int nextStep: 0
        onTriggered: {
            // GameManager.currentStoryStep was already updated in advance()
            // before the transition began; only the local mirror needs syncing.
            gameRoot.currentStep = nextStep
            fadeOverlay.opacity = 0.0
            transitionEndTimer.start()
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

    // ── Background ────────────────────────────────────────────────────────
    Rectangle {
        id: background
        anchors.fill: parent
        color: gameRoot.storyData.length > 0
               ? gameRoot.step().bg
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
            text: gameRoot.storyData.length > 0 && gameRoot.step().shotTitle !== undefined
                  ? gameRoot.step().shotTitle : ""

            Behavior on opacity { NumberAnimation { duration: 300 } }
        }

        // Shake wrapper for shot 4
        Item {
            id: shakeWrapper
            anchors.fill: parent

            property real shakeX: 0.0
            property real shakeY: 0.0
            transform: Translate { x: shakeWrapper.shakeX; y: shakeWrapper.shakeY }

            SequentialAnimation {
                running: gameRoot.storyData.length > 0 && gameRoot.step().shake
                loops: Animation.Infinite
                NumberAnimation { target: shakeWrapper; property: "shakeX"; from: 0; to:  6; duration: 60 }
                NumberAnimation { target: shakeWrapper; property: "shakeX"; from: 6; to: -6; duration: 60 }
                NumberAnimation { target: shakeWrapper; property: "shakeX"; from: -6; to: 0; duration: 60 }
                NumberAnimation { target: shakeWrapper; property: "shakeY"; from: 0; to:  4; duration: 60 }
                NumberAnimation { target: shakeWrapper; property: "shakeY"; from: 4; to: -4; duration: 60 }
                NumberAnimation { target: shakeWrapper; property: "shakeY"; from: -4; to: 0; duration: 60 }
            }

            // Character display function
            function charData(which) {
                if (gameRoot.storyData.length === 0) return { visible: false }
                const s = gameRoot.step()
                if (which === "A") return s.charA
                if (which === "B") return s.charB
                return s.charC
            }

            // ── Character A (left) ──────────────────────────────────────────
            Column {
                id: charADisplay
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                anchors.leftMargin: 80
                anchors.bottomMargin: 10
                spacing: 6
                visible: shakeWrapper.charData("A").visible === true
                opacity: visible ? 1.0 : 0.0
                Behavior on opacity { NumberAnimation { duration: 200 } }

                Rectangle {
                    width: 160; height: 300
                    radius: 12
                    color: gameRoot.emotionColor(shakeWrapper.charData("A").emotion || "normal",
                                                 gameRoot.charMeta["A"].baseColor)
                    border.color: Qt.darker(color, 1.5)
                    border.width: 3

                    Column {
                        anchors.centerIn: parent
                        spacing: 8
                        Text { anchors.horizontalCenter: parent.horizontalCenter; text: gameRoot.charMeta["A"].symbol; font.pixelSize: 56 }
                        Text { anchors.horizontalCenter: parent.horizontalCenter; text: gameRoot.emotionEmoji(shakeWrapper.charData("A").emotion || "normal"); font.pixelSize: 36 }
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
                visible: shakeWrapper.charData("C").visible === true
                opacity: visible ? 1.0 : 0.0
                Behavior on opacity { NumberAnimation { duration: 200 } }

                Rectangle {
                    width: 160; height: 300
                    radius: 12
                    color: gameRoot.emotionColor(shakeWrapper.charData("C").emotion || "normal",
                                                 gameRoot.charMeta["C"].baseColor)
                    border.color: Qt.darker(color, 1.5)
                    border.width: 3

                    Column {
                        anchors.centerIn: parent
                        spacing: 8
                        Text { anchors.horizontalCenter: parent.horizontalCenter; text: gameRoot.charMeta["C"].symbol; font.pixelSize: 56 }
                        Text { anchors.horizontalCenter: parent.horizontalCenter; text: gameRoot.emotionEmoji(shakeWrapper.charData("C").emotion || "normal"); font.pixelSize: 36 }
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
                visible: shakeWrapper.charData("B").visible === true
                opacity: visible ? 1.0 : 0.0
                Behavior on opacity { NumberAnimation { duration: 200 } }

                Rectangle {
                    width: 160; height: 300
                    radius: 12
                    color: gameRoot.emotionColor(shakeWrapper.charData("B").emotion || "normal",
                                                 gameRoot.charMeta["B"].baseColor)
                    border.color: Qt.darker(color, 1.5)
                    border.width: 3

                    Column {
                        anchors.centerIn: parent
                        spacing: 8
                        Text { anchors.horizontalCenter: parent.horizontalCenter; text: gameRoot.charMeta["B"].symbol; font.pixelSize: 56 }
                        Text { anchors.horizontalCenter: parent.horizontalCenter; text: gameRoot.emotionEmoji(shakeWrapper.charData("B").emotion || "normal"); font.pixelSize: 36 }
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
        visible: gameRoot.storyData.length > 0 && gameRoot.step().type !== "ending"

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
                     gameRoot.step().type === "dialogue" &&
                     gameRoot.step().speaker !== undefined

            Text {
                id: speakerLabel
                anchors.centerIn: parent
                text: gameRoot.storyData.length > 0 && gameRoot.step().speaker !== undefined
                      ? gameRoot.step().speaker : ""
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
            text: gameRoot.storyData.length > 0 ? gameRoot.step().text : ""
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
            onClicked: settingsPopup.open()
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
        visible: gameRoot.storyData.length > 0 && gameRoot.step().type === "ending"

        Column {
            anchors.centerIn: parent
            spacing: 40

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: gameRoot.storyData.length > 0 && gameRoot.step().type === "ending"
                      ? gameRoot.step().text : ""
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
                    model: [
                        { num: 1, title: "开场" },
                        { num: 2, title: "对峙" },
                        { num: 3, title: "升级" },
                        { num: 4, title: "冲突" },
                        { num: 5, title: "转折" },
                        { num: 6, title: "顿悟" },
                        { num: 7, title: "结尾" }
                    ]

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
