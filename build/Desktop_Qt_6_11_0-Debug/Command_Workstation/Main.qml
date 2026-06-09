import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

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
            Layout.preferredWidth: 300
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
                        text: qsTr("Presets")
                        font.bold: true
                        font.pixelSize: 14
                        color: win.textBright
                    }

                    Repeater {
                        id: presetRepeater
                        model: []

                        delegate: Column {
                            required property var modelData
                            width: leftCol.width
                            spacing: 6

                            Label {
                                text: modelData.name
                                font.bold: true
                                color: accent
                                font.pixelSize: 12
                            }

                            Repeater {
                                model: modelData.items
                                delegate: Button {
                                    required property var modelData
                                    width: parent.width
                                    text: modelData.label
                                    enabled: !App.busy
                                    onClicked: App.runPreset(modelData.id)
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
                        }

                        Label {
                            text: App.gitStatus
                            color: win.textBright
                            wrapMode: Text.Wrap
                            width: parent.width
                            font.family: "monospace"
                            font.pixelSize: 11
                        }

                        Label {
                            visible: App.gitRemote.length > 0
                            text: App.gitRemote
                            color: win.textMuted
                            wrapMode: Text.Wrap
                            width: parent.width
                            font.family: "monospace"
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
                            font.family: "monospace"
                            font.pixelSize: 11
                            background: Rectangle { color: "#131416" }
                        }
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
