import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Effects

ApplicationWindow {
    id: win
    width: 1280
    height: 760
    visible: true
    title: qsTr("Command Workstation")
    color: "#161719"

    readonly property color surface: "#202124"
    readonly property color border: "#34373c"
    readonly property color textMuted: "#aeb4bc"
    readonly property color textBright: "#f2f4f7"
    readonly property color accent: "#4c8bf5"
    readonly property string fontMono: "JetBrains Mono"
    font: Application.font

    function colorToRgbHex(c) {
        const s = c.toString()
        if (s.length >= 9 && s.charAt(0) === "#")
            return ("#" + s.substring(3, 9)).toLowerCase()
        if (s.length >= 7)
            return s.substring(0, 7).toLowerCase()
        return ""
    }

    function reloadPresets() {
        presetRepeater.model = App.presetGroups()
    }

    function loadRecent() {
        recentListModel.clear()
        if (!App.databaseOpen)
            return
        const rows = App.recentRuns(40)
        for (let i = 0; i < rows.length; ++i)
            recentListModel.append(rows[i])
    }

    function appendTerminal(text, isErr) {
        if (isErr)
            terminal.text += text // stderr igual ao stdout no MVP
        else
            terminal.text += text
        terminal.cursorPosition = terminal.length
    }

    Component.onCompleted: {
        reloadPresets()
        loadRecent()
        introOpenTimer.start()
    }

    Timer {
        id: introOpenTimer
        interval: 320
        repeat: false
        onTriggered: introPopup.open()
    }

    Popup {
        id: introPopup
        parent: Overlay.overlay
        modal: true
        dim: true
        padding: 0
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        font: Application.font

        width: Math.min(520, Overlay.overlay.width - 40)
        height: introCard.implicitHeight

        x: Math.round((parent.width - width) / 2)
        y: Math.round((parent.height - height) / 2)

        transformOrigin: Item.Center
        enter: Transition {
            ParallelAnimation {
                NumberAnimation {
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: 280
                    easing.type: Easing.OutCubic
                }
                NumberAnimation {
                    property: "scale"
                    from: 0.94
                    to: 1
                    duration: 340
                    easing.type: Easing.OutCubic
                }
            }
        }
        exit: Transition {
            NumberAnimation {
                property: "opacity"
                to: 0
                duration: 180
                easing.type: Easing.InCubic
            }
        }

        background: Item {}

        contentItem: Item {
            implicitWidth: introPopup.width
            implicitHeight: introCard.implicitHeight

            Rectangle {
                id: introCard
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                radius: 18
                color: "#24262c"
                border.width: 1
                border.color: "#3a3f48"
                implicitHeight: introColumn.implicitHeight + 40

                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    shadowColor: "#c0000000"
                    shadowVerticalOffset: 14
                    shadowBlur: 1.2
                }

                Rectangle {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 4
                    radius: 2
                    clip: true
                    gradient: Gradient {
                        orientation: Gradient.Horizontal
                        GradientStop { position: 0.0; color: "#5c9bff" }
                        GradientStop { position: 0.5; color: "#7c5cff" }
                        GradientStop { position: 1.0; color: "#3dd9c0" }
                    }
                }

                ColumnLayout {
                    id: introColumn
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 28
                    anchors.topMargin: 32
                    spacing: 14

                    Label {
                        text: qsTr("Command Workstation")
                        font.pixelSize: 13
                        font.letterSpacing: 0.8
                        font.weight: Font.DemiBold
                        color: accent
                        Layout.fillWidth: true
                    }

                    Label {
                        text: qsTr("Bem-vindo")
                        font.pixelSize: 26
                        font.weight: Font.Bold
                        color: textBright
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }

                    Label {
                        Layout.fillWidth: true
                        Layout.maximumWidth: parent.width
                        wrapMode: Text.WordWrap
                        color: textMuted
                        font.pixelSize: 14
                        lineHeight: 1.35
                        text: qsTr("Um painel unificado para comandos, Git e terminal — menos alternância entre janelas, "
                                   + "mais foco no que importa.")
                    }

                    ColumnLayout {
                        spacing: 8
                        Layout.topMargin: 6
                        Layout.fillWidth: true

                        Repeater {
                            model: [
                                qsTr("Escolha a pasta do projeto no topo e use o terminal integrado."),
                                qsTr("Execute presets ou salve comandos próprios com nome e pasta."),
                                qsTr("Histórico e configurações ficam no SQLite local.")
                            ]

                            delegate: RowLayout {
                                required property var modelData
                                spacing: 10
                                Layout.fillWidth: true

                                Rectangle {
                                    width: 6
                                    height: 6
                                    radius: 3
                                    color: accent
                                    Layout.alignment: Qt.AlignTop
                                    Layout.topMargin: 6
                                }

                                Label {
                                    text: modelData
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                    color: textBright
                                    font.pixelSize: 13
                                    lineHeight: 1.3
                                    opacity: 0.92
                                }
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.topMargin: 18
                        spacing: 12

                        Item { Layout.fillWidth: true }

                        Button {
                            text: qsTr("Começar")
                            highlighted: true
                            flat: false
                            onClicked: introPopup.close()

                            background: Rectangle {
                                implicitWidth: parent.implicitWidth
                                implicitHeight: 40
                                radius: 10
                                color: parent.down ? Qt.darker(accent, 1.15)
                                                 : (parent.hovered ? Qt.lighter(accent, 1.08) : accent)
                            }

                            contentItem: Label {
                                text: parent.text
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                                color: "#101216"
                                font.pixelSize: 14
                                font.weight: Font.DemiBold
                            }
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: App
        function onOutputLine(text, isErr) {
            appendTerminal(text, isErr)
        }
        function onDatabaseOpenChanged() {
            reloadPresets()
            loadRecent()
        }
        function onBusyChanged() {
            if (!App.busy)
                loadRecent()
        }
        function onWorkingDirectoryChanged() {
            reloadPresets()
        }
    }

    FolderDialog {
        id: folderDialog
        title: qsTr("Project folder")
        onAccepted: {
            App.pickFolderUrl(selectedFolder)
            reloadPresets()
            loadRecent()
        }
    }

    FolderDialog {
        id: presetSaveFolderDialog
        title: qsTr("Folder for saved command")
        onAccepted: {
            saveCwdField.text = App.urlToLocalPath(selectedFolder)
        }
    }

    FolderDialog {
        id: editPresetFolderDialog
        title: qsTr("Folder for saved command")
        onAccepted: {
            editCwdField.text = App.urlToLocalPath(selectedFolder)
        }
    }

    ColorDialog {
        id: savePresetColorDialog
        title: qsTr("Preset button color")
        onAccepted: saveButtonColorField.text = win.colorToRgbHex(selectedColor)
    }

    ColorDialog {
        id: editPresetColorDialog
        title: qsTr("Preset button color")
        onAccepted: editButtonColorField.text = win.colorToRgbHex(selectedColor)
    }

    Dialog {
        id: editPresetDialog
        property int editingId: -1

        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(460, parent.width - 32)
        modal: true
        title: qsTr("Edit saved command")
        font: Application.font

        ColumnLayout {
            spacing: 10
            width: parent.width

            TextField {
                id: editNameField
                Layout.fillWidth: true
                placeholderText: qsTr("Display name")
                enabled: App.databaseOpen && !App.busy
            }
            TextField {
                id: editCmdField
                Layout.fillWidth: true
                placeholderText: qsTr("Shell command")
                font.family: win.fontMono
                enabled: App.databaseOpen && !App.busy
            }
            RowLayout {
                Layout.fillWidth: true
                TextField {
                    id: editCwdField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Folder (empty = home directory)")
                    enabled: App.databaseOpen && !App.busy
                }
                Button {
                    text: qsTr("Browse…")
                    enabled: App.databaseOpen && !App.busy
                    onClicked: editPresetFolderDialog.open()
                }
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                Label {
                    text: qsTr("Button color")
                    color: win.textMuted
                    Layout.alignment: Qt.AlignVCenter
                }
                Rectangle {
                    width: 28
                    height: 28
                    radius: 4
                    color: editButtonColorField.text.trim().length ? editButtonColorField.text : win.accent
                    border.width: 1
                    border.color: win.border
                }
                TextField {
                    id: editButtonColorField
                    Layout.fillWidth: true
                    placeholderText: qsTr("#RRGGBB (empty = default)")
                    font.family: win.fontMono
                    enabled: App.databaseOpen && !App.busy
                }
                Button {
                    text: qsTr("Pick…")
                    enabled: App.databaseOpen && !App.busy
                    onClicked: {
                        const t = editButtonColorField.text.trim()
                        editPresetColorDialog.selectedColor = (t.length >= 4 && t.startsWith("#")) ? Qt.color(t) : win.accent
                        editPresetColorDialog.open()
                    }
                }
                Button {
                    text: qsTr("Default")
                    enabled: App.databaseOpen && !App.busy
                    onClicked: editButtonColorField.text = ""
                }
            }
            Label {
                id: editPresetError
                visible: text.length > 0
                Layout.fillWidth: true
                wrapMode: Text.Wrap
                font.pixelSize: 11
                color: "#ffb74d"
                text: ""
            }
            RowLayout {
                Layout.alignment: Qt.AlignRight
                spacing: 8
                Button {
                    text: qsTr("Cancel")
                    onClicked: editPresetDialog.close()
                }
                Button {
                    text: qsTr("Save")
                    highlighted: true
                    onClicked: {
                        editPresetError.text = ""
                        if (!App.updateCustomPreset(editPresetDialog.editingId,
                                                    editNameField.text,
                                                    editCmdField.text,
                                                    editCwdField.text,
                                                    editButtonColorField.text)) {
                            editPresetError.text =
                                    qsTr("Could not save. Check the name, command, and folder path.")
                        } else {
                            editPresetDialog.close()
                            reloadPresets()
                        }
                    }
                }
            }
        }
    }

    Dialog {
        id: deletePresetDialog
        property int presetId: -1
        property string presetLabel: ""

        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(400, parent.width - 32)
        modal: true
        title: qsTr("Delete saved command?")
        font: Application.font

        ColumnLayout {
            spacing: 14
            width: parent.width

            Label {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                color: win.textBright
                text: qsTr("Remove \"%1\"? This cannot be undone.").arg(deletePresetDialog.presetLabel)
            }
            RowLayout {
                Layout.alignment: Qt.AlignRight
                spacing: 8
                Button {
                    text: qsTr("Cancel")
                    onClicked: deletePresetDialog.close()
                }
                Button {
                    text: qsTr("Delete")
                    onClicked: {
                        App.deleteCustomPreset(deletePresetDialog.presetId)
                        deletePresetDialog.close()
                        reloadPresets()
                    }
                }
            }
        }
    }

    Dialog {
        id: gitCommitDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(480, parent.width - 32)
        modal: true
        title: qsTr("Commit changes")
        font: Application.font

        onOpened: gitCommitMessageField.text = ""

        ColumnLayout {
            spacing: 10
            width: parent.width

            Label {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                color: win.textMuted
                text: qsTr("Stage all changes (git add -A) and create a commit with the message below.")
            }
            TextField {
                id: gitCommitMessageField
                Layout.fillWidth: true
                placeholderText: qsTr("Commit message")
                font.family: win.fontMono
                enabled: !App.busy
            }
            RowLayout {
                Layout.alignment: Qt.AlignRight
                spacing: 8
                Button {
                    text: qsTr("Cancel")
                    onClicked: gitCommitDialog.close()
                }
                Button {
                    text: qsTr("Commit")
                    highlighted: true
                    enabled: gitCommitMessageField.text.trim().length > 0 && !App.busy
                    onClicked: {
                        App.gitCommitAll(gitCommitMessageField.text)
                        gitCommitDialog.close()
                    }
                }
            }
        }
    }

    Dialog {
        id: gitDiscardDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(440, parent.width - 32)
        modal: true
        title: qsTr("Discard local changes?")
        font: Application.font

        ColumnLayout {
            spacing: 14
            width: parent.width

            Label {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                color: win.textBright
                text: qsTr("This runs git reset --hard HEAD. All uncommitted changes to tracked files "
                          + "will be lost. Untracked files are not removed. This cannot be undone.")
            }
            RowLayout {
                Layout.alignment: Qt.AlignRight
                spacing: 8
                Button {
                    text: qsTr("Cancel")
                    onClicked: gitDiscardDialog.close()
                }
                Button {
                    text: qsTr("Discard changes")
                    enabled: !App.busy
                    onClicked: {
                        App.gitDiscardChanges()
                        gitDiscardDialog.close()
                    }
                }
            }
        }
    }

    header: ToolBar {
        padding: 8
        background: Rectangle { color: win.surface; border.color: win.border; border.width: 0 }

        RowLayout {
            anchors.fill: parent
            spacing: 10

            Button {
                text: qsTr("Choose folder…")
                onClicked: folderDialog.open()
            }

            Label {
                text: App.workingDirectory
                elide: Text.ElideMiddle
                Layout.fillWidth: true
                color: win.textBright
                font.pixelSize: 13
            }

            BusyIndicator {
                running: App.busy
                implicitWidth: 28
                implicitHeight: 28
            }

            Button {
                text: qsTr("Refresh Git")
                enabled: !App.busy
                onClicked: App.refreshGit()
            }

            Button {
                text: qsTr("Stop")
                enabled: App.busy
                onClicked: App.stopCommand()
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 12

        Rectangle {
            Layout.preferredWidth: 420
            Layout.fillHeight: true
            color: win.surface
            radius: 6
            border.color: win.border
            border.width: 1

            ScrollView {
                id: leftScroll
                anchors.fill: parent
                anchors.margins: 8
                clip: true

                Column {
                    id: leftCol
                    width: leftScroll.availableWidth
                    spacing: 14

                    Label {
                        text: qsTr("Save command")
                        font.bold: true
                        font.pixelSize: 14
                        color: win.textBright
                    }

                    TextField {
                        id: saveNameField
                        width: parent.width
                        placeholderText: qsTr("Display name")
                        enabled: App.databaseOpen && !App.busy
                    }

                    TextField {
                        id: saveCmdField
                        width: parent.width
                        placeholderText: qsTr("Shell command")
                        font.family: win.fontMono
                        enabled: App.databaseOpen && !App.busy
                    }

                    RowLayout {
                        width: parent.width
                        TextField {
                            id: saveCwdField
                            Layout.fillWidth: true
                            placeholderText: qsTr("Folder (empty = home directory)")
                            enabled: App.databaseOpen && !App.busy
                        }
                        Button {
                            text: qsTr("Browse…")
                            enabled: App.databaseOpen && !App.busy
                            onClicked: presetSaveFolderDialog.open()
                        }
                    }

                    RowLayout {
                        width: parent.width
                        spacing: 8
                        Label {
                            text: qsTr("Button color")
                            color: win.textMuted
                            Layout.alignment: Qt.AlignVCenter
                        }
                        Rectangle {
                            width: 28
                            height: 28
                            radius: 4
                            color: saveButtonColorField.text.trim().length ? saveButtonColorField.text : win.accent
                            border.width: 1
                            border.color: win.border
                        }
                        TextField {
                            id: saveButtonColorField
                            Layout.fillWidth: true
                            placeholderText: qsTr("#RRGGBB (empty = default)")
                            font.family: win.fontMono
                            enabled: App.databaseOpen && !App.busy
                        }
                        Button {
                            text: qsTr("Pick…")
                            enabled: App.databaseOpen && !App.busy
                            onClicked: {
                                const t = saveButtonColorField.text.trim()
                                savePresetColorDialog.selectedColor = (t.length >= 4 && t.startsWith("#")) ? Qt.color(t) : win.accent
                                savePresetColorDialog.open()
                            }
                        }
                        Button {
                            text: qsTr("Default")
                            enabled: App.databaseOpen && !App.busy
                            onClicked: saveButtonColorField.text = ""
                        }
                    }

                    Label {
                        id: savePresetHint
                        visible: text.length > 0
                        width: parent.width
                        wrapMode: Text.Wrap
                        font.pixelSize: 10
                        color: "#ffb74d"
                        text: ""
                    }

                    RowLayout {
                        width: parent.width
                        spacing: 8
                        Button {
                            text: qsTr("Save")
                            enabled: App.databaseOpen && !App.busy
                                     && saveNameField.text.trim().length > 0
                                     && saveCmdField.text.trim().length > 0
                            onClicked: {
                                savePresetHint.text = ""
                                const ok = App.saveCustomPreset(saveNameField.text,
                                                               saveCmdField.text,
                                                               saveCwdField.text,
                                                               saveButtonColorField.text)
                                if (ok) {
                                    saveNameField.text = ""
                                    saveCmdField.text = ""
                                    saveCwdField.text = ""
                                    saveButtonColorField.text = ""
                                    reloadPresets()
                                } else {
                                    savePresetHint.text = qsTr("Could not save. Check folder path or database.")
                                }
                            }
                        }
                        Button {
                            text: qsTr("Clear folder (use home)")
                            enabled: App.databaseOpen
                            onClicked: saveCwdField.text = ""
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: 1
                        color: win.border
                    }

                    Label {
                        text: qsTr("Presets")
                        font.bold: true
                        font.pixelSize: 14
                        color: win.textBright
                    }

                    Repeater {
                        id: presetRepeater
                        model: []

                        delegate: Column {
                            id: presetCategoryColumn
                            required property var modelData
                            width: leftCol.width
                            spacing: 6

                            readonly property string groupName: String(modelData.name ?? "")

                            Label {
                                text: modelData.name
                                font.bold: true
                                color: accent
                                font.pixelSize: 12
                            }

                            Repeater {
                                model: modelData.items
                                delegate: RowLayout {
                                    required property var modelData
                                    width: presetCategoryColumn.width
                                    spacing: 4

                                    readonly property bool isSavedPreset:
                                        (presetCategoryColumn.groupName === "Personalizados")
                                        || (String(modelData.category ?? "") === "Personalizados")

                                    Button {
                                        visible: !isSavedPreset
                                        Layout.fillWidth: true
                                        Layout.minimumWidth: 80
                                        text: modelData.label
                                        enabled: !App.busy
                                        onClicked: App.runPreset(modelData.id)
                                        ToolTip.visible: hovered
                                        ToolTip.delay: 400
                                        ToolTip.text: {
                                            const cmd = modelData.command ?? ""
                                            const d = modelData.cwd ?? ""
                                            const cat = String(modelData.category
                                                              ?? presetCategoryColumn.groupName
                                                              ?? "")
                                            if (d.length)
                                                return cmd + "\n—\n" + d
                                            if (cat === "Personalizados")
                                                return cmd + "\n—\n" + qsTr("(home folder)")
                                            return cmd + "\n—\n" + qsTr("(project folder)")
                                        }
                                    }
                                    Button {
                                        id: savedPresetRunBtn
                                        visible: isSavedPreset
                                        Layout.fillWidth: true
                                        Layout.minimumWidth: 80
                                        text: modelData.label
                                        enabled: !App.busy
                                        onClicked: App.runPreset(modelData.id)

                                        readonly property color fillColor: {
                                            const t = String(modelData.buttonColor ?? "").trim()
                                            if (t.length >= 4 && t.startsWith("#"))
                                                return Qt.color(t)
                                            return win.accent
                                        }
                                        background: Rectangle {
                                            implicitHeight: 38
                                            radius: 6
                                            color: savedPresetRunBtn.down ? Qt.darker(savedPresetRunBtn.fillColor, 1.18)
                                                                           : (savedPresetRunBtn.hovered ? Qt.lighter(savedPresetRunBtn.fillColor, 1.12)
                                                                                                         : savedPresetRunBtn.fillColor)
                                            border.width: 1
                                            border.color: Qt.rgba(0, 0, 0, 0.28)
                                        }
                                        contentItem: Label {
                                            text: savedPresetRunBtn.text
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                            color: savedPresetRunBtn.fillColor.hslLightness > 0.62 ? "#101216"
                                                                                                 : "#f2f4f7"
                                            font.pixelSize: 13
                                        }
                                        ToolTip.visible: hovered
                                        ToolTip.delay: 400
                                        ToolTip.text: {
                                            const cmd = modelData.command ?? ""
                                            const d = modelData.cwd ?? ""
                                            const cat = String(modelData.category
                                                              ?? presetCategoryColumn.groupName
                                                              ?? "")
                                            if (d.length)
                                                return cmd + "\n—\n" + d
                                            if (cat === "Personalizados")
                                                return cmd + "\n—\n" + qsTr("(home folder)")
                                            return cmd + "\n—\n" + qsTr("(project folder)")
                                        }
                                    }
                                    ToolButton {
                                        visible: isSavedPreset
                                        text: qsTr("Edit")
                                        font.pixelSize: 11
                                        Layout.minimumWidth: 54
                                        enabled: App.databaseOpen && !App.busy
                                        onClicked: {
                                            editPresetDialog.editingId = modelData.id
                                            editNameField.text = modelData.label
                                            editCmdField.text = modelData.command
                                            const c = modelData.cwd
                                            editCwdField.text = (c !== undefined && c !== null) ? String(c) : ""
                                            const bc = modelData.buttonColor
                                            editButtonColorField.text = (bc !== undefined && bc !== null) ? String(bc) : ""
                                            editPresetError.text = ""
                                            editPresetDialog.open()
                                        }
                                    }
                                    ToolButton {
                                        visible: isSavedPreset
                                        text: qsTr("Del")
                                        font.pixelSize: 11
                                        Layout.minimumWidth: 44
                                        enabled: App.databaseOpen && !App.busy
                                        onClicked: {
                                            deletePresetDialog.presetId = modelData.id
                                            deletePresetDialog.presetLabel = modelData.label
                                            deletePresetDialog.open()
                                        }
                                    }
                                }
                            }

                            Item { width: 1; height: 4 }
                        }
                    }

                    Label {
                        visible: !App.databaseOpen
                        text: qsTr("Database unavailable — presets disabled.")
                        wrapMode: Text.Wrap
                        color: "#ff8a65"
                        width: parent.width
                    }

                    Label {
                        text: qsTr("Recent runs")
                        font.bold: true
                        font.pixelSize: 14
                        color: win.textBright
                    }

                    ListView {
                        id: recentView
                        width: parent.width
                        height: Math.min(220, recentListModel.count * 56)
                        model: recentListModel
                        clip: true
                        spacing: 6
                        delegate: Rectangle {
                            required property var startedAt
                            required property var cwd
                            required property var command
                            required property var exitCode
                            required property var durationMs
                            width: recentView.width
                            radius: 4
                            color: "#2a2d33"
                            height: col.height + 12
                            border.color: win.border
                            border.width: 1

                            Column {
                                id: col
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.margins: 8
                                spacing: 2
                                Label {
                                    width: parent.width
                                    text: startedAt + " · exit " + exitCode + " · " + durationMs + " ms"
                                    font.pixelSize: 10
                                    color: win.textMuted
                                    elide: Text.ElideRight
                                }
                                Label {
                                    width: parent.width
                                    text: command
                                    color: win.textBright
                                    font.pixelSize: 11
                                    elide: Text.ElideRight
                                }
                            }
                        }
                    }
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 150
                color: win.surface
                radius: 6
                border.color: win.border
                border.width: 1

                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 10

                    Column {
                        width: parent.width
                        spacing: 6

                        RowLayout {
                            width: parent.width
                            Label {
                                text: qsTr("Branch:")
                                color: win.textMuted
                            }
                            Label {
                                text: App.gitBranch.length ? App.gitBranch : qsTr("—")
                                color: win.textBright
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }
                            Button {
                                text: qsTr("Pull")
                                enabled: !App.busy
                                onClicked: App.gitPull()
                            }
                            Button {
                                text: qsTr("Push")
                                enabled: !App.busy
                                onClicked: App.gitPush()
                            }
                            Button {
                                text: qsTr("Commit…")
                                enabled: App.gitRepo && App.gitDirty && !App.busy
                                onClicked: gitCommitDialog.open()
                            }
                            Button {
                                text: qsTr("Discard changes")
                                enabled: App.gitRepo && App.gitDirty && !App.busy
                                onClicked: gitDiscardDialog.open()
                            }
                        }

                        Label {
                            text: App.gitStatus
                            color: win.textBright
                            wrapMode: Text.Wrap
                            width: parent.width
                            font.family: win.fontMono
                            font.pixelSize: 11
                        }

                        Label {
                            visible: App.gitRemote.length > 0
                            text: App.gitRemote
                            color: win.textMuted
                            wrapMode: Text.Wrap
                            width: parent.width
                            font.family: win.fontMono
                            font.pixelSize: 10
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: win.surface
                radius: 6
                border.color: win.border
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 6

                    RowLayout {
                        Layout.fillWidth: true
                        Label {
                            text: qsTr("Terminal")
                            font.bold: true
                            color: win.textBright
                            Layout.fillWidth: true
                        }
                        Button {
                            text: qsTr("Clear")
                            onClicked: terminal.text = ""
                        }
                    }

                    ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        TextArea {
                            id: terminal
                            readOnly: true
                            wrapMode: Text.Wrap
                            color: "#d7dbe0"
                            selectedTextColor: "#101216"
                            selectionColor: accent
                            font.family: win.fontMono
                            font.pixelSize: 11
                            background: Rectangle { color: "#131416" }
                        }
                    }

                    CheckBox {
                        Layout.fillWidth: true
                        text: qsTr("System terminal (real TTY — sudo, passwords, interactive)")
                        checked: App.useSystemTerminal
                        onToggled: App.useSystemTerminal = checked
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8

                        TextField {
                            id: customCmd
                            Layout.fillWidth: true
                            placeholderText: qsTr("Type a shell command…")
                            onAccepted: {
                                App.runShellCommand(text)
                                text = ""
                            }
                        }

                        Button {
                            text: qsTr("Run")
                            enabled: !App.busy && customCmd.text.trim().length > 0
                            onClicked: {
                                App.runShellCommand(customCmd.text)
                                customCmd.text = ""
                            }
                        }
                    }
                }
            }
        }
    }

    ListModel {
        id: recentListModel
    }

    Label {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 8
        text: App.databaseOpen ? (qsTr("SQLite: ") + App.databasePath) : qsTr("SQLite: not connected")
        font.pixelSize: 10
        color: win.textMuted
    }
}
