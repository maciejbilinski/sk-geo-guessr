import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Layouts 1.14
import QtQuick.Controls 2.15

Window {
    property string bgColor: "#1E1E1E";
    property int headerFontSize: 40;
    property int labelFontSize: 14;
    property int controlWidth: 200;
    property int headerPadding: 20;
    property int buttonMargin: 20;
    property int buttonPadding: 15;

    id: root;
    width: 1280; height: 720;
    minimumWidth: 1280; minimumHeight: 720;
    maximumWidth: 1280; maximumHeight: 720;

    visible: true
    title: qsTr("GeoGuessr")

    Rectangle {
        anchors.fill: parent;
        color: bgColor;

        ColumnLayout{
            id: introduction;

            anchors.centerIn: parent;
            spacing: 5;
            Text{
                Layout.alignment: Qt.AlignCenter;
                padding: headerPadding;
                text: "GeoGuessr";
                font.pointSize: headerFontSize;
                color: "#C1C1C4"
            }

            ColumnLayout{
                Layout.alignment: Qt.AlignCenter;
                Text{
                    text: "Pickup server"
                    color: "#C1C1C4"
                    Layout.alignment: Qt.AlignLeft;
                    font.pointSize: labelFontSize;
                }
                ComboBox{
                    Layout.alignment: Qt.AlignLeft;
                    Layout.preferredWidth: controlWidth;
                    model: ListModel{
                        id: serverCB
                        ListElement { text: "Server on Port 50000"; }
                        ListElement { text: "Server on Port 50010"; }
                        ListElement { text: "Server on Port 50020"; }
                        ListElement { text: "Server on Port 50030"; }
                        ListElement { text: "Server on Port 50040"; }
                    }
                }
            }

            ColumnLayout{
                Layout.alignment: Qt.AlignCenter;
                Text{
                    text: "Introduce yourself"
                    color: "#C1C1C4"
                    Layout.alignment: Qt.AlignLeft;
                    font.pointSize: labelFontSize;
                }
                TextField{
                    Layout.alignment: Qt.AlignLeft;
                    Layout.preferredWidth: controlWidth;
                    id: nameTF

                }
            }


            Button{
                id: submit;
                Layout.alignment: Qt.AlignCenter;
                Layout.margins: buttonMargin;
                Layout.preferredWidth: controlWidth;
                text: qsTr("Join server")
                contentItem: Text{
                    text: submit.text;
                    font: submit.font;
                    color: "#FFF";
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle{
                    color: enabled ? (submit.down ? "#898DE8": "#9a9ef9") : "#AAA";
                    radius: 2;
                }


                padding: buttonPadding;
                enabled:nameTF.text.length > 0
                onClicked: function(){
                    introduction.visible = false;
                    loader.visible = true;
                }
            }

        }

        ColumnLayout{
            id: loader
            visible: false
            anchors.centerIn: parent;

            BusyIndicator{
                running: true
                palette.dark: "#FFF"
                Layout.alignment: Qt.AlignCenter;

            }
            Button{
                id: cancel;
                Layout.alignment: Qt.AlignCenter;
                Layout.margins: 2*buttonMargin;
                Layout.preferredWidth: controlWidth;
                text: qsTr("Cancel")
                palette.buttonText: "#FFF"
                background: Rectangle{
                    color: "#9a9ef9"
                    radius: 2;
                }
                padding: buttonPadding;
                onClicked: function(){
                    introduction.visible = true;
                    loader.visible = false;
                }
            }

        }


    }
}

//import QtPositioning 5.15
//import QtLocation 5.15
//    Plugin {
//        id: mapPlugin
//        name: "osm" // "mapboxgl", "esri", ...
//    }

//    Map {
//        anchors.fill: parent
//        plugin: mapPlugin
//        center: QtPositioning.coordinate(59.91, 10.75) // Oslo
//        zoomLevel: 14
//    }
