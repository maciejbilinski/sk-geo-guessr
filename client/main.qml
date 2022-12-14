import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Layouts 1.14
import QtQuick.Controls 2.15

import QtPositioning 5.15
import QtLocation 5.15

Window {
    property string bgColor: "#1E1E1E"
    property int headerFontSize: 40
    property int labelFontSize: 14
    property int controlWidth: 200
    property int headerPadding: 20
    property int buttonMargin: 20
    property int buttonPadding: 15

    id: root
    width: 1280
    height: 720
    minimumWidth: 1280
    minimumHeight: 720
    maximumWidth: 1280
    maximumHeight: 720
    Plugin {
        id: mapPlugin
        name: "osm" // "mapboxgl", "esri", ...
    }
    visible: true
    title: qsTr("GeoGuessr")

    Rectangle {
        anchors.fill: parent
        color: bgColor
        //Strona początkowa
        ColumnLayout {
            id: introduction

            anchors.centerIn: parent
            spacing: 5
            Text {
                Layout.alignment: Qt.AlignCenter
                padding: headerPadding
                text: "GeoGuessr"
                font.pointSize: headerFontSize
                color: "#C1C1C4"
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignCenter
                Text {
                    text: "Pickup server"
                    color: "#C1C1C4"
                    Layout.alignment: Qt.AlignLeft
                    font.pointSize: labelFontSize
                }
                ComboBox {
                    Layout.alignment: Qt.AlignLeft
                    Layout.preferredWidth: controlWidth
                    model: ListModel {
                        id: serverCB
                        ListElement {
                            text: "50000"
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.alignment: Qt.AlignCenter
                Text {
                    text: "Introduce yourself"
                    color: "#C1C1C4"
                    Layout.alignment: Qt.AlignLeft
                    font.pointSize: labelFontSize
                }
                TextField {
                    Layout.alignment: Qt.AlignLeft
                    Layout.preferredWidth: controlWidth
                    id: nameTF
                }
            }

            Button {
                id: submit
                Layout.alignment: Qt.AlignCenter
                Layout.margins: buttonMargin
                Layout.preferredWidth: controlWidth
                text: qsTr("Join server")
                contentItem: Text {
                    text: submit.text
                    font: submit.font
                    color: "#FFF"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    color: enabled ? (submit.down ? "#898DE8" : "#9a9ef9") : "#AAA"
                    radius: 2
                }

                padding: buttonPadding
                enabled: nameTF.text.length > 0
                onClicked: function () {
                    introduction.visible = false
                    loader.visible = true
                }
            }
        }

        //Łączenie do serwera
        ColumnLayout {
            id: loader
            visible: false
            anchors.centerIn: parent

            BusyIndicator {
                running: true
                palette.dark: "#FFF"
                Layout.alignment: Qt.AlignCenter
            }
            Text {
                id: waiting_for_game
                text: qsTr("Game has already started, waiting for next round")
                color: "#ffffff"
            }
            Button {
                id: cancel
                Layout.alignment: Qt.AlignCenter
                Layout.margins: 2 * buttonMargin
                Layout.preferredWidth: controlWidth
                text: qsTr("Cancel")
                palette.buttonText: "#FFF"
                background: Rectangle {
                    color: "#9a9ef9"
                    radius: 2
                }
                padding: buttonPadding
                onClicked: function () {
                    choose_team_and_host.visible = true
                    loader.visible = false
                }
            }
        }
        //Głosowanie i wybieranie druzyny
        ColumnLayout {
            id: choose_team_and_host
            anchors.centerIn: parent
            visible: false
            ColumnLayout {
                Layout.alignment: Qt.AlignCenter
                Text {
                    Layout.alignment: Qt.AlignCenter
                    padding: headerPadding
                    text: "GeoGuessr"
                    font.pointSize: headerFontSize
                    color: "#C1C1C4"
                }
            }
            ColumnLayout {
                Layout.alignment: Qt.AlignCenter
                Text {
                    id: vote_host
                    text: qsTr("Vote for game host")
                    color: "#FFF"
                }
                ComboBox {
                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: controlWidth
                    model: ListModel {
                        id: vote_host_combobox
                        ListElement {
                            text: "None"
                        }
                    }
                }
            }
            ColumnLayout {
                Layout.alignment: Qt.AlignCenter

                Text {
                    id: choose_team
                    text: qsTr("Choose team")
                    color: "#FFF"
                }
                ComboBox {
                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: controlWidth
                    model: ListModel {
                        id: choose_team_combobox
                        ListElement {
                            text: "Green"
                        }
                        ListElement {
                            text: "Yellow"
                        }
                        ListElement {
                            text: "Orange"
                        }
                        ListElement {
                            text: "Pink"
                        }
                    }
                }
            }
            Button {
                id: set_ready_status
                Layout.alignment: Qt.AlignCenter
                Layout.margins: 2 * buttonMargin
                text: qsTr("Send and set ready status")
                palette.buttonText: "#FFF"
                background: Rectangle {
                    color: "#9a9ef9"
                    radius: 2
                }
                padding: buttonPadding
                onClicked: function () {
                    choose_team_and_host.visible = false
                    waiting_for_game_ranking.visible = true
                }
            }
        }
        //Oczekiwanie na hosta
        ColumnLayout {
            id: waiting_for_game_ranking
            visible: false
            anchors.centerIn: parent
            Text {
                Layout.alignment: Qt.AlignCenter
                padding: headerPadding
                text: "Waiting for host"
                font.pointSize: headerFontSize
                color: "#C1C1C4"
            }
            BusyIndicator {
                running: true
                palette.dark: "#FFF"
                Layout.alignment: Qt.AlignCenter
            }
            Text {
                Layout.alignment: Qt.AlignCenter
                padding: headerPadding
                text: "Ranking"
                font.pointSize: 30
                color: "#C1C1C4"
            }
            Text {
                text: qsTr("1. Andrzej")
                font.pointSize: 18
                color: "#ffffff"
            }
        }
        //Ekran gry
        GridLayout {
            width: 1200
            height: 720
            id: game
            visible: false

            Layout.alignment: Qt.AlignLeft
            ColumnLayout {
                width: 720

                Item {
                    height: 120
                    width: 800
                    Layout.alignment: Qt.AlignCenter

                    Text {
                        id: time
                        color: "#FFF"
                        anchors.centerIn: parent
                        text: "Pozostały czas: 997s"
                        Layout.alignment: Qt.AlignCenter
                    }
                }
                Map {
                    height: 600
                    width: 800
                    plugin: mapPlugin
                    id: map
                    center: QtPositioning.coordinate(52.40371, 16.9495) // PP <3
                    zoomLevel: 20
                }
            }
            ColumnLayout {
                width: 460
                height: 720
                Layout.alignment: Qt.AlignTop
                Item {
                    height: 120
                    width: 470
                    Text {
                        anchors.centerIn: parent
                        id: round
                        text: qsTr("Runda X")
                        color: "#FFF"
                    }
                }
                Item {
                    height: 600
                    width: 460
                    Layout.alignment: Qt.AlignCenter
                    Image {
                        id: photo
                        width: 460
                        height: 600
                        sourceSize.width: 480
                        sourceSize.height: 600
                        Layout.alignment: Qt.AlignCenter

                        fillMode: Image.PreserveAspectFit
                        source: "https://i.ytimg.com/vi/abUhPANjIsM/maxresdefault.jpg"
                    }
                }
            }
        }
        //Ekran hosta
        GridLayout {
            width: 1200
            height: 720
            id: admin_panel
            visible: false
            Layout.alignment: Qt.AlignLeft
            ColumnLayout {
                width: 720

                Item {
                    height: 120
                    width: 800
                    Layout.alignment: Qt.AlignCenter

                    Text {
                        id: admin_header
                        color: "#FFF"
                        anchors.centerIn: parent
                        text: "Host panel"
                        font.pointSize: headerFontSize
                        Layout.alignment: Qt.AlignCenter
                    }
                }
                Map {
                    height: 600
                    width: 800
                    plugin: mapPlugin
                    id: map_admin
                    center: QtPositioning.coordinate(52.40371, 16.9495) // PP <3
                    zoomLevel: 20
                }
            }
            ColumnLayout {
                width: 460
                height: 720
                Layout.alignment: Qt.AlignTop
                Item {
                    height: 120
                    width: 470
                    Text {
                        anchors.centerIn: parent
                        id: round_counter_admin
                        text: qsTr("Runda X")
                        color: "#FFF"
                    }
                }
                Item {
                    height: 300
                    width: 460
                    Layout.alignment: Qt.AlignCenter
                    Image {
                        id: photo_preview
                        width: 460
                        height: 300
                        sourceSize.width: 480
                        sourceSize.height: 300
                        Layout.alignment: Qt.AlignCenter

                        fillMode: Image.PreserveAspectFit
                        source: "https://i.ytimg.com/vi/abUhPANjIsM/maxresdefault.jpg"
                    }
                }
                Item {
                    height: 50
                    width: 470
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("Paste here photo url")
                        color: "#FFF"
                    }
                }
                Item {
                    height: 100
                    width: 470

                    Rectangle {
                        anchors.fill: parent
                        color: '#ffffff'
                        TextEdit {
                            id: photo_url
                            width: 470
                            focus: true
                            wrapMode: TextEdit.WrapAnywhere
                        }
                    }
                }
                RowLayout {
                    height: 100
                    width: 460
                    Layout.alignment: Qt.AlignCenter

                    Button {
                        id: preview_photo
                        Layout.alignment: Qt.AlignCenter
                        Layout.margins: 2 * buttonMargin
                        text: qsTr("Preview")
                        palette.buttonText: "#FFF"
                        background: Rectangle {
                            color: "#9a9ef9"
                            radius: 2
                        }
                        padding: buttonPadding
                        onClicked: function () {//                            choose_team_and_host.visible = false
                            //                            waiting_for_game_ranking.visible = true
                        }
                    }
                    Button {
                        id: ok_button
                        Layout.alignment: Qt.AlignCenter
                        Layout.margins: 2 * buttonMargin
                        text: qsTr("Start")
                        palette.buttonText: "#FFF"
                        background: Rectangle {
                            color: "#9a9ef9"
                            radius: 2
                        }
                        padding: buttonPadding
                        onClicked: function () {//choose_team_and_host.visible = false
                            //waiting_for_game_ranking.visible = true
                        }
                    }
                }
            }
        }
    }
}
