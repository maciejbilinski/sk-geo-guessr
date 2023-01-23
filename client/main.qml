import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Layouts 1.14
import QtQuick.Controls 2.15

import QtPositioning 5.15
import QtLocation 5.15

import put.geoguessr.ServerTools 1.0

Window {
    property string bgColor: "#1E1E1E"
    property int headerFontSize: 40
    property int labelFontSize: 14
    property int controlWidth: 200
    property int headerPadding: 20
    property int buttonMargin: 20
    property int buttonPadding: 15

    property string noPhoto: "https://t4.ftcdn.net/jpg/04/70/29/97/360_F_470299797_UD0eoVMMSUbHCcNJCdv2t8B2g1GVqYgs.jpg";
    property string errorPhoto: "https://www.salonlfc.com/wp-content/uploads/2018/01/image-not-found-1-scaled-1150x647.png"

    property bool alreadyAnswered: false;

    id: root
    width: 1280
    height: 720
    minimumWidth: 1280
    minimumHeight: 720
    maximumWidth: 1280
    maximumHeight: 720
    visible: true
    title: qsTr("GeoGuessr")

    Plugin {
        id: mapPlugin
        name: "osm"
    }

    function _timer() {
        return Qt.createQmlObject("import QtQuick 2.0; Timer {}", root);
    }

    function countChar(word, c) {
      var count = 0
      for (let i = 0; i <= word.length; i++) {
        if (c.includes(word.charCodeAt(i))) {
          count += 1
        }
      }
      return count;
    }

    function setTimeout(cb, delayTime) {
        var timer = _timer();
        timer.interval = delayTime;
        timer.repeat = false;
        timer.triggered.connect(cb);
        timer.start();
    }

    ServerTools{
        id: serverTools
        onStateChanged: function(state){
            introduction.visible = false;
            loader.visible = false;
            choose_team_and_host.visible = false;
            waiting_for_game_ranking.visible = false;
            game.visible = false;
            admin_panel.visible = false;
            introError.visible = false;
            introNameError.visible = false;
            waiting_for_game.visible = false;

            if(state <= 0){
                if(state === ServerTools.ERROR){
                    introduction.visible = true;
                    introError.visible = true;
                }else if(state === ServerTools.NAME_EXISTS){
                    introduction.visible = true;
                    introNameError.visible = true;
                }else if(state === ServerTools.GAME_STARTED){
                    loader.visible = true;
                    waiting_for_game.visible = true;
                }else{
                    introduction.visible = true;
                }
            }else if(state === ServerTools.WAIT_FOR_GAME){
                loader.visible = true;
            }else if(state === ServerTools.VOTING){
                choose_team_and_host.visible = true;
            }else if(state === ServerTools.WAIT_FOR_RANKING){
                waiting_for_game_ranking.visible = true;
            }else if(state === ServerTools.ADMIN_PANEL){
                admin_panel.visible = true;
            }else if(state === ServerTools.GAME){
                alreadyAnswered = false;
                game.visible = true;
            }
        }
        onPlayersChanged: function(){
            vote_host_combobox.clear();
            serverTools.players.forEach(function(player){
                vote_host_combobox.append({"text": player})
            });
        }
        onRankingChanged: function(){
            setTimeout(function(){
                rankingModel.clear();
                serverTools.ranking.forEach(function(playerName, index){
                    rankingModel.append({"rank": index+1, "playerName": playerName})
                });
            }, 500);
        }
        onRoundChanged: function(){
            if(serverTools.state === ServerTools.GAME){
                game_markers.clear();
                alreadyAnswered = false;

                introduction.visible = false;
                loader.visible = false;
                choose_team_and_host.visible = false;
                waiting_for_game_ranking.visible = false;
                game.visible = true;
                introNameError.visible = false;
                admin_panel.visible = false;
                introError.visible = false;
                waiting_for_game.visible = false;
            }

        }
        onAnswersChanged: function(){
            var min = 0;
            if(game_markers.count > 0){
                if(game_markers.get(0).id === serverTools.me){
                    min = 1;
                }
            }

            for(var i=game_markers.count-1; i>=min; i--){
                game_markers.remove(i);
            }
            Object.keys(serverTools.answers).forEach(function(key){
                game_markers.insert(min, {
                    "position": QtPositioning.coordinate(serverTools.answers[key].x, serverTools.answers[key].y), "id": key
                });
                min++;
            });
        }


    }

    Rectangle {
        anchors.fill: parent
        Layout.fillWidth: true
        Layout.fillHeight: true
        color: bgColor

        //Strona początkowa
        ColumnLayout {
            id: introduction
            visible: true
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
                    id: serverCB
                    Layout.alignment: Qt.AlignLeft
                    Layout.preferredWidth: controlWidth
                    model: ListModel {
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
                    serverTools.connect(nameTF.text, parseInt(serverCB.currentText));
                }
            }
            Text {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: controlWidth
                id: introError
                text: qsTr("Server erorr. Try again.")
                color: "#e53935"
                visible: false
            }
            Text {
                Layout.alignment: Qt.AlignCenter
                Layout.preferredWidth: controlWidth
                id: introNameError
                text: qsTr("This name is already taken.")
                color: "#e53935"
                visible: false
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
                text: qsTr("Game has already started or waits for more players")
                color: "#ffffff"
                visible: false
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
                    serverTools.quit();
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
                    id: cpCB
                    Layout.alignment: Qt.AlignCenter
                    Layout.preferredWidth: controlWidth
                    model: ListModel {
                        id: vote_host_combobox
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
                    id: ctCB
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
                enabled: cpCB.currentText !== ""
                Layout.alignment: Qt.AlignCenter
                Layout.margins: 2 * buttonMargin
                text: qsTr("Send and set ready status")
                palette.buttonText: "#FFF"
                background: Rectangle {
                    color: enabled ? (set_ready_status.down ? "#898DE8" : "#9a9ef9") : "#AAA"
                    radius: 2
                }
                padding: buttonPadding
                onClicked: function () {
                    choose_team_and_host.visible = false
                    loader.visible = true
                    serverTools.vote(cpCB.currentText, ctCB.currentText)
                }
            }
        }

        //Oczekiwanie na hosta
        ColumnLayout {
            id: waiting_for_game_ranking
            visible: false
            anchors.centerIn: parent
            Layout.fillWidth: parent
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

            ListModel{
                id: rankingModel
            }
            Component {
               id: rankingDelegate
               Text {
                   text: qsTr(rank + ". " + playerName)
                   font.pointSize: 18
                   color: "#ffffff"
               }

            }
            ListView {
                model: rankingModel
                delegate: rankingDelegate
                Layout.fillWidth: parent

                height: childrenRect.height

            }

        }

        //Ekran gry
        Row {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10
            id: game
            visible: false
            Column {
                width: parent.width*0.75 - 5
                height: parent.height

                Text {
                    id: time
                    color: "#FFF"
                    text: qsTr("Pozostały czas: " + serverTools.timeLeft + "s")
                    font.pointSize: headerFontSize
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                ListModel {
                    id: game_markers
                    dynamicRoles: true
                }

                Map {
                    width: parent.width
                    height: parent.height-time.height
                    plugin: mapPlugin
                    id: map_game
                    center: QtPositioning.coordinate(52.40371, 16.9495) // PP <3
                    zoomLevel: 20

                    MapItemView{
                        model: game_markers
                        delegate: MapQuickItem {
                            coordinate: model.position
                            anchorPoint.x: game_marker_img.width * 0.5
                            anchorPoint.y: game_marker_img.height
                            sourceItem: Column{
                                Text{
                                    text: model.id
                                }

                                Image {
                                    id: game_marker_img
                                    source: "marker.png"
                                }
                            }
                        }
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            var coord = map_game.toCoordinate(Qt.point(mouse.x,mouse.y));
                            if(game_markers.count > 0){
                                if(game_markers.get(0).id === serverTools.me){
                                    game_markers.remove(0);
                                }
                            }

                            game_markers.insert(0, {"position": coord, "id": serverTools.me})
                        }
                    }
                }
            }
            Column {
                width: parent.width*0.25 - 5
                height: parent.height

                Text {
                    id: round_counter_game
                    text: qsTr("Runda " + serverTools.round)
                    color: "#FFF"

                    font.pointSize: headerFontSize
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Image {
                    id: photo_challenge
                    sourceSize.width: parent.width
                    Layout.alignment: Qt.AlignCenter

                    fillMode: Image.PreserveAspectFit
                    source: serverTools.photoURL
                }

                Rectangle{
                    width: parent.width
                    height: parent.height - round_counter_game.height-photo_challenge.height-challenge_button.height
                    color: bgColor
                }

                Button {
                    id: challenge_button
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Answer")
                    enabled: game_markers.count > 0 && game_markers.get(0).id === serverTools.me && !alreadyAnswered && serverTools.timeLeft > 0

                    palette.buttonText: "#FFF"
                    background: Rectangle {
                        color: enabled ? (challenge_button.down ? "#898DE8" : "#9a9ef9") : "#AAA"
                        radius: 2
                    }
                    padding: buttonPadding
                    onClicked: function () {
                        const coords = game_markers.get(0);
                        serverTools.sendAnswer(coords.position.latitude, coords.position.longitude)
                        alreadyAnswered = true
                    }
                }

            }
        }


        //Ekran hosta
        Row {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10
            id: admin_panel
            visible: false
            Column {
                width: parent.width*0.75 - 5
                height: parent.height

                Text {
                    id: admin_header
                    color: "#FFF"
                    text: "Host panel"
                    font.pointSize: headerFontSize
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                ListModel {
                    id: marker
                    dynamicRoles: true
                }

                Map {
                    width: parent.width
                    height: parent.height-admin_header.height
                    plugin: mapPlugin
                    id: map_admin
                    center: QtPositioning.coordinate(52.40371, 16.9495) // PP <3
                    zoomLevel: 20

                    MapItemView{
                        model: marker
                        delegate: MapQuickItem {
                            coordinate: model.position
                            anchorPoint.x: marker_img.width * 0.5
                            anchorPoint.y: marker_img.height
                            sourceItem: Image {
                                id: marker_img
                                source: "marker.png"
                            }
                        }
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            var coord = map_admin.toCoordinate(Qt.point(mouse.x,mouse.y));
                            marker.clear();
                            marker.append({"position": coord})
                        }
                    }
                }
            }
            Column {
                width: parent.width*0.25 - 5
                height: parent.height

                Text {
                    id: round_counter_admin
                    text: qsTr("Runda " + serverTools.round)
                    color: "#FFF"

                    font.pointSize: headerFontSize
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Image {
                    id: photo_preview
                    sourceSize.width: parent.width
                    Layout.alignment: Qt.AlignCenter

                    fillMode: Image.PreserveAspectFit
                    source: noPhoto
                    onStatusChanged: function(){
                        if(this.status === Image.Error || this.status === Image.Null){
                            this.source = errorPhoto
                        }
                    }
                }
                Column{
                    width: parent.width
                    height: parent.height - photo_preview.height - round_counter_admin.height - admin_panel_btns.height
                    Text {
                        id: paste_url_text
                        Layout.alignment: Qt.AlignCenter
                        text: qsTr("Paste here photo url")
                        color: "#FFF"
                    }
                    Rectangle {
                        property real teMaxHeight: parent.height - paste_url_text.height;
                        width: parent.width
                        color: '#ffffff'
                        height: photo_url.height > teMaxHeight ? teMaxHeight : photo_url.height
                        TextEdit {
                            width: parent.width
                            id: photo_url
                            focus: true
                            wrapMode: TextEdit.WrapAnywhere
                            onTextChanged: function(e){
                                var save = photo_url.cursorPosition;
                                const newlines = countChar(photo_url.text.substring(0, save), [10, 9])
                                if(newlines !== 0){
                                    photo_url.text = photo_url.text.replace(String.fromCharCode(10), '');                                    photo_url.cursorPosition = save > 256 ? 256 : save;
                                    photo_url.text = photo_url.text.replace(String.fromCharCode(9), '');                                    photo_url.cursorPosition = save > 256 ? 256 : save;
                                    save -= newlines;
                                }

                                if(photo_url.length >= 256){
                                    photo_url.text = photo_url.text.substring(0, 256)
                                }

                                photo_url.cursorPosition = save > 256 ? 256 : save;
                            }
                        }
                    }
                }
                Row{
                    id: admin_panel_btns
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 10

                    Button {
                        id: preview_photo
                        text: qsTr("Preview")
                        palette.buttonText: "#FFF"
                        background: Rectangle {
                            color: "#9a9ef9"
                            radius: 2
                        }
                        padding: buttonPadding
                        onClicked: function () {
                            photo_preview.source = photo_url.text
                        }
                    }
                    Button {
                        id: ok_button
                        text: qsTr("Start")
                        enabled: [noPhoto, errorPhoto].every(function(a){
                            return a != photo_preview.source
                        }) && marker.count === 1

                        palette.buttonText: "#FFF"
                        background: Rectangle {
                            color: enabled ? (ok_button.down ? "#898DE8" : "#9a9ef9") : "#AAA"
                            radius: 2
                        }
                        padding: buttonPadding
                        onClicked: function () {
                            const coords = marker.get(0);
                            serverTools.sendPhoto(photo_preview.source, coords.position.latitude, coords.position.longitude)
                            photo_url.text = ""
                            photo_url.focus = true
                            photo_preview.source = noPhoto
                            marker.clear()
                        }
                    }
                }
            }
        }
    }
}
