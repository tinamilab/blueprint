import QtQuick 2.9
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Controls.Styles 1.4
import QtQml 2.1
//import QtQuick.Dialogs 1.1

//Our modele
import com.tinami.backend 1.0
//import io.qt.examples.controller 1.0

ApplicationWindow{
    id: app
    title: qsTr("Blueprint")
    visible: true
    width: 1024
    height: 768

    property int currentPreset: 0

    Component.onCompleted: {
        presetButtons.itemAt(0).checked = true
    }

    BackEnd {
        id: backend

        onDeviceStatusChanged: {
            switch(backend.deviceStatus){
            case BackEnd.Unplugged:
                deviceStatusLabel.text = "unplugged";
                break;
            case BackEnd.Ready_for_config:
                deviceStatusLabel.text = "ready for config";
                break;
            case BackEnd.Ready_for_update:
                deviceStatusLabel.text = "ready for update";
                break;
            case BackEnd.Ok_data:
                deviceStatusLabel.text = "ok Data";
                break;
            case BackEnd.Working:
                deviceStatusLabel.text = "working";
                break;
            case BackEnd.Wrong_data:
                deviceStatusLabel.text = "wrong Data";
                break;
            }
        }

        onPresetChanged: {
            componentButtons.itemAt(0).checked = true;
        }

        onComponentModeChanged: {
            switch(backend.deviceMode){
            case BackEnd.VoiceNote:
                componentMode_combo.currentIndex = 0
                break;
            case BackEnd.VoiceControlChange:
                componentMode_combo.currentIndex = 1
                break;
            }
        }

        onComponentButtonBehaviourChanged: {
            switch(backend.componentButtonBehaviour){
            case BackEnd.None:
                break;
            case BackEnd.Momentary:
                componentButtonBehaviour_combo.currentIndex = 0
                break;
            case BackEnd.Toggle:
                componentButtonBehaviour_combo.currentIndex = 1
                break;
            }
        }

        onComponentDataChanged: {
            componentData_combo.currentIndex = componentData;
        }

        onComponentChannelChanged: {
            if(componentChannel >= 16)
                componentChannel_combo.currentIndex = 16;
            else
                componentChannel_combo.currentIndex = componentChannel;
        }

        onComponentMinValueChanged: {
            componentMinValue_textInput.text = componentMinValue;
        }

        onComponentMaxValueChanged: {
            componentMaxValue_textInput.text = componentMaxValue;
        }

        onSynchronizingChanged: {
            if(!backend.synchronizing){
                clock.visible = false;
                syncButton.visible = true;
            } else {
                clock.visible = true;
                syncButton.visible = false;
            }
        }

        onGlobalChannelChanged: {
            globalChannel_combo.currentIndex = backend.globalChannel
        }
    }

    Image {
        id: image
        x: 0
        y: 0
        width: 1024
        height: 768
        source: "images/background.png"

        Rectangle {
            id: rectangle
            x: 579
            y: 0
            width: 445
            height: 768
            color: "#1e1e1e"
            border.color: "#00000000"

            Image {
                id: image1
                x: 144
                y: 48
                width: 169
                height: 64
                source: "images/Logo-MD1BP.svg"
            }

            Text {
                id: comp_name
                x: 60
                y: 165
                color: "#e8ac5b"
                text: qsTr("component")
                font.bold: true
                font.pixelSize: 36
                font.family: "Arial"
            }

            Text {
                id: mode_txt
                x: 123
                y: 237
                color: "#e8ac5b"
                text: qsTr("mode")
                horizontalAlignment: Text.AlignRight
                font.bold: true
                font.pixelSize: 18
                font.family: "Arial"
            }

            ComboBox {
                id: componentMode_combo
                x: 186
                y: 232
                wheelEnabled: false
                focusPolicy: Qt.StrongFocus
                enabled: true
                font.family: "HelveticaNeueLTStd-Bd"
                font.pixelSize: 14
                textRole: "key"
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 30
                    color: "#d8d9d1"
                }
                model: ListModel{
                    ListElement{ key: "Voice Note"; value: 0 }
                    ListElement{ key: "CC"; value: 2 }
                }

                onCurrentTextChanged: {
                    if(currentIndex === 0){
                        backend.setComponentMode(0)
                    } else if (currentIndex === 1){
                        backend.setComponentMode(2)
                    }
                }
            }

            Text {
                id: cc_txt
                x: 80
                y: 287
                color: "#e8ac5b"
                text: qsTr("cc number")
                horizontalAlignment: Text.AlignRight
                font.bold: true
                font.pixelSize: 18
                font.family: "Arial"
            }

            ComboBox {
                id: componentData_combo
                x: 186
                y: 282
                wheelEnabled: false
                focusPolicy: Qt.StrongFocus
                enabled: true
                font.family: "HelveticaNeueLTStd-Bd"
                font.pixelSize: 14
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 30
                    color: "#d8d9d1"
                }

                model: ListModel{
                    ListElement{text: "0"}
                    ListElement{text: "1"}
                    ListElement{text: "2"}
                    ListElement{text: "3"}
                    ListElement{text: "4"}
                    ListElement{text: "5"}
                    ListElement{text: "6"}
                    ListElement{text: "7"}
                    ListElement{text: "8"}
                    ListElement{text: "9"}
                    ListElement{text: "10"}
                    ListElement{text: "11"}
                    ListElement{text: "12"}
                    ListElement{text: "13"}
                    ListElement{text: "14"}
                    ListElement{text: "15"}
                    ListElement{text: "16"}
                    ListElement{text: "17"}
                    ListElement{text: "18"}
                    ListElement{text: "19"}
                    ListElement{text: "20"}
                    ListElement{text: "21"}
                    ListElement{text: "22"}
                    ListElement{text: "23"}
                    ListElement{text: "24"}
                    ListElement{text: "25"}
                    ListElement{text: "26"}
                    ListElement{text: "27"}
                    ListElement{text: "28"}
                    ListElement{text: "29"}
                    ListElement{text: "30"}
                    ListElement{text: "31"}
                    ListElement{text: "32"}
                    ListElement{text: "33"}
                    ListElement{text: "34"}
                    ListElement{text: "35"}
                    ListElement{text: "36"}
                    ListElement{text: "37"}
                    ListElement{text: "38"}
                    ListElement{text: "39"}
                    ListElement{text: "40"}
                    ListElement{text: "41"}
                    ListElement{text: "42"}
                    ListElement{text: "43"}
                    ListElement{text: "44"}
                    ListElement{text: "45"}
                    ListElement{text: "46"}
                    ListElement{text: "47"}
                    ListElement{text: "48"}
                    ListElement{text: "49"}
                    ListElement{text: "50"}
                    ListElement{text: "51"}
                    ListElement{text: "52"}
                    ListElement{text: "53"}
                    ListElement{text: "54"}
                    ListElement{text: "55"}
                    ListElement{text: "56"}
                    ListElement{text: "57"}
                    ListElement{text: "58"}
                    ListElement{text: "59"}
                    ListElement{text: "60"}
                    ListElement{text: "61"}
                    ListElement{text: "62"}
                    ListElement{text: "63"}
                    ListElement{text: "64"}
                    ListElement{text: "65"}
                    ListElement{text: "66"}
                    ListElement{text: "67"}
                    ListElement{text: "68"}
                    ListElement{text: "69"}
                    ListElement{text: "70"}
                    ListElement{text: "71"}
                    ListElement{text: "72"}
                    ListElement{text: "73"}
                    ListElement{text: "74"}
                    ListElement{text: "75"}
                    ListElement{text: "76"}
                    ListElement{text: "77"}
                    ListElement{text: "78"}
                    ListElement{text: "79"}
                    ListElement{text: "80"}
                    ListElement{text: "81"}
                    ListElement{text: "82"}
                    ListElement{text: "83"}
                    ListElement{text: "84"}
                    ListElement{text: "85"}
                    ListElement{text: "86"}
                    ListElement{text: "87"}
                    ListElement{text: "88"}
                    ListElement{text: "89"}
                    ListElement{text: "90"}
                    ListElement{text: "91"}
                    ListElement{text: "92"}
                    ListElement{text: "93"}
                    ListElement{text: "94"}
                    ListElement{text: "95"}
                    ListElement{text: "96"}
                    ListElement{text: "97"}
                    ListElement{text: "98"}
                    ListElement{text: "99"}
                    ListElement{text: "100"}
                    ListElement{text: "101"}
                    ListElement{text: "102"}
                    ListElement{text: "103"}
                    ListElement{text: "104"}
                    ListElement{text: "105"}
                    ListElement{text: "106"}
                    ListElement{text: "107"}
                    ListElement{text: "108"}
                    ListElement{text: "109"}
                    ListElement{text: "110"}
                    ListElement{text: "111"}
                    ListElement{text: "112"}
                    ListElement{text: "113"}
                    ListElement{text: "114"}
                    ListElement{text: "115"}
                    ListElement{text: "116"}
                    ListElement{text: "117"}
                    ListElement{text: "118"}
                    ListElement{text: "119"}
                    ListElement{text: "120"}
                    ListElement{text: "121"}
                    ListElement{text: "122"}
                    ListElement{text: "123"}
                    ListElement{text: "124"}
                    ListElement{text: "125"}
                    ListElement{text: "126"}
                    ListElement{text: "127"}
                }
                onCurrentTextChanged: {
                    backend.setComponentData(currentIndex)
                }
            }

            Text {
                id: channel_txt
                x: 104
                y: 337
                color: "#e8ac5b"
                text: qsTr("channel")
                horizontalAlignment: Text.AlignRight
                font.bold: true
                font.pixelSize: 18
                font.family: "Arial"
            }

            ComboBox {
                id: componentChannel_combo
                x: 186
                y: 332
                wheelEnabled: false
                focusPolicy: Qt.StrongFocus
                enabled: true
                font.family: "HelveticaNeueLTStd-Bd"
                font.pixelSize: 14
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 30
                    color: "#d8d9d1"
                }
                model: ListModel{
                    ListElement{text: "1"}
                    ListElement{text: "2"}
                    ListElement{text: "3"}
                    ListElement{text: "4"}
                    ListElement{text: "5"}
                    ListElement{text: "6"}
                    ListElement{text: "7"}
                    ListElement{text: "8"}
                    ListElement{text: "9"}
                    ListElement{text: "10"}
                    ListElement{text: "11"}
                    ListElement{text: "12"}
                    ListElement{text: "13"}
                    ListElement{text: "14"}
                    ListElement{text: "15"}
                    ListElement{text: "16"}
                    ListElement{text: "Global"}
                }
                onCurrentTextChanged: {
                    backend.setComponentChannel(currentIndex)
                }
            }

            Text {
                id: minval_txt
                x: 104
                y: 387
                width: 68
                color: "#e8ac5b"
                text: qsTr("min value")
                horizontalAlignment: Text.AlignRight
                font.bold: true
                font.pixelSize: 18
                font.family: "Arial"
            }

            TextField {
                id: componentMinValue_textInput
                x: 186
                y: 385
                color: "#d9d1d1"
                text: backend.componentMinValue //text: rango.first.value
                inputMask: qsTr("")
                topPadding: 10
                bottomPadding: 9
                leftPadding: 12
                rightPadding: 30
                font.bold: true
                selectionColor: "#d1d5d9"
                font.pixelSize: 14
                font.family: "Arial"
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 30
                    color: "transparent"
                    border.color: "#d8d9d1"
                    border.width: 0.5
                }

                onTextChanged:{
                    backend.setComponentMinValue(text)
                    rango.first.value = text
                }
            }

            Text {
                id: maxval_txt
                x: 104
                y: 437
                width: 68
                color: "#e8ac5b"
                text: qsTr("max value")
                horizontalAlignment: Text.AlignRight
                font.bold: true
                font.pixelSize: 18
                font.family: "Arial"
            }

            TextField {
                id: componentMaxValue_textInput
                x: 186
                y: 435
                color: "#d9d1d1"
                text: backend.componentMaxValue//text: rango.second.value
                inputMask: qsTr("")
                topPadding: 6
                font.bold: true
                selectionColor: "#d1d5d9"
                font.pixelSize: 14
                font.family: "Arial"
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 30
                    color: "transparent"
                    border.color: "#d8d9d1"
                    border.width: 0.5
                }

                onTextChanged:{
                    backend.setComponentMaxValue(text)
                    rango.second.value = text
                }
            }

            Text {
                id: button_behaviour_txt
                x: 104
                y: 544
                width: 68
                color: "#e8ac5b"
                text: qsTr("button behaviour")
                horizontalAlignment: Text.AlignRight
                font.bold: true
                font.pixelSize: 18
                font.family: "Arial"
            }

            ComboBox {
                id: componentButtonBehaviour_combo
                x: 186
                y: 542
                wheelEnabled: false
                focusPolicy: Qt.StrongFocus
                enabled: true
                font.family: "HelveticaNeueLTStd-Bd"
                font.pixelSize: 14
                textRole: "key"
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 30
                    color: "#d8d9d1"
                }
                model: ListModel{
                    ListElement{ key: "momentary"; value: 1 }
                    ListElement{ key: "toggle"; value: 2 }
                }
                onCurrentTextChanged: {
                    backend.setComponentButtonBehaviour(currentIndex + 1)
                }
            }

            Button {
                id: syncButton
                x: 160
                y: 628
                text: qsTr("sync")
                focusPolicy: Qt.TabFocus
                font.family: "HelveticaNeueLTStd-Bd"
                font.pixelSize: 14

                contentItem: Text {
                    text: syncButton.text
                    color: "#1e1e1e"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                background: Rectangle {
                    implicitWidth: 125
                    implicitHeight: 40
                    color: "#e8ac5b"
                }

                onClicked: {
                    backend.syncHost2Device();
                }
            }

            Image {
                id: clock
                x: 160
                y: 620

                source: "images/clock.svg"
                visible: false
            }

            RangeSlider {
                id: rango
                x: 129
                y: 482
                orientation: Qt.Horizontal
                stepSize: 1
                to: 127
                first.onValueChanged: {
                    backend.setComponentMinValue(componentMinValue_textInput.text)
                    componentMinValue_textInput.text = first.value.toFixed(0)
                }
                second.onValueChanged:{
                    backend.setComponentMaxValue(componentMaxValue_textInput.text)
                    componentMaxValue_textInput.text = second.value.toFixed(0)
                }
            }
        }

        Image {
            id: back_controler
            x: 90
            y: 60
            width: 400
            height: 393
            source: "images/back.svg"

            property int i: 0
            property int j: 0
            property int component: 0

            function selectComponentWithKeys(){
                back_controler.component = back_controler.i * 4 + back_controler.j;

                componentButtons.itemAt(back_controler.component).checked = true
            }

            Keys.onUpPressed: {
                console.log("up")

                if(back_controler.i > 0)
                    back_controler.i--;

                selectComponentWithKeys();
            }

            Keys.onDownPressed: {
                console.log("down")

                if(back_controler.i < 3)
                    back_controler.i++;

                selectComponentWithKeys();
            }

            Keys.onRightPressed:  {
                console.log("right")

                if(back_controler.j < 3)
                    back_controler.j++;

                selectComponentWithKeys();
            }

            Keys.onLeftPressed: {
                console.log("left")

                if(back_controler.j > 0)
                    back_controler.j--;

                selectComponentWithKeys();
            }

            Grid {  //  Component buttons grid
                id: componentGrid
                x: 34
                y: 50
                columns: 4
                rows: 4
                columnSpacing: -14
                rowSpacing: -9

                Repeater {
                    id: componentButtons
                    model: 16

                    ComponentButton{
                        name: {
                            switch(index){
                            case 0:
                                "a1";
                                break;
                            case 1:
                                "a2";
                                break;
                            case 2:
                                "a3";
                                break;
                            case 3:
                                "a4";
                                break;
                            case 4:
                                "b1";
                                break;
                            case 5:
                                "b2";
                                break;
                            case 6:
                                "b3";
                                break;
                            case 7:
                                "b4";
                                break;
                            case 8:
                                "c1";
                                break;
                            case 9:
                                "c2";
                                break;
                            case 10:
                                "c3";
                                break;
                            case 11:
                                "c4";
                                break;
                            case 12:
                                "d1";
                                break;
                            case 13:
                                "d2";
                                break;
                            case 14:
                                "d3";
                                break;
                            case 15:
                                "d4";
                                break;
                            }
                        }

                        type: {
                            switch(index){
                            case 0:
                                backend.control0Type
                                break;
                            case 1:
                                backend.control1Type
                                break;
                            case 2:
                                backend.control2Type
                                break;
                            case 3:
                                backend.control3Type
                                break;
                            case 4:
                                backend.control4Type
                                break;
                            case 5:
                                backend.control5Type
                                break;
                            case 6:
                                backend.control6Type
                                break;
                            case 7:
                                backend.control7Type
                                break;
                            case 8:
                                backend.control8Type
                                break;
                            case 9:
                                backend.control9Type
                                break;
                            case 10:
                                backend.control10Type
                                break;
                            case 11:
                                backend.control11Type
                                break;
                            case 12:
                                backend.control12Type
                                break;
                            case 13:
                                backend.control13Type
                                break;
                            case 14:
                                backend.control14Type
                                break;
                            case 15:
                                backend.control15Type
                                break;
                            }
                        }

                        onCheckedChanged: {
                            comp_name.text = codeName()
                            back_controler.i = index / 4
                            back_controler.j = index % 4
                            backend.selectComponent(index)
                        }
                    }
                }
            }
        }

        Text {
            id: preset_txt
            visible: true
            x: 107
            y: 506
            color: "#e8ac5b"
            text: qsTr("preset")
            horizontalAlignment: Text.AlignRight
            font.bold: true
            font.pixelSize: 18
            font.family: "Arial"
        }

        ComboBox {
            id: globalChannel_combo
            visible: true
            x: 244
            y: 658
            wheelEnabled: true
            focusPolicy: Qt.StrongFocus
            enabled: true
            font.family: "HelveticaNeueLTStd-Bd"
            font.pixelSize: 14

            background: Rectangle {
                implicitWidth: 200
                implicitHeight: 30
                color: "#d8d9d1"
            }
            model: ListModel{
                ListElement{text: "1"}
                ListElement{text: "2"}
                ListElement{text: "3"}
                ListElement{text: "4"}
                ListElement{text: "5"}
                ListElement{text: "6"}
                ListElement{text: "7"}
                ListElement{text: "8"}
                ListElement{text: "9"}
                ListElement{text: "10"}
                ListElement{text: "11"}
                ListElement{text: "12"}
                ListElement{text: "13"}
                ListElement{text: "14"}
                ListElement{text: "15"}
                ListElement{text: "16"}
            }
            onCurrentTextChanged:{
                backend.setGlobalChannel(currentIndex)
            }
        }

        Text {
            id: global_txt
            visible: true
            x: 104
            y: 664
            color: "#e8ac5b"
            text: qsTr("global channel")
            horizontalAlignment: Text.AlignRight
            font.bold: true
            font.pixelSize: 18
            font.family: "Arial"
        }

        Grid {  //  Preset buttons grid
            x: 107
            y: 537
            columns: 8
            rows: 2
            columnSpacing: 15
            rowSpacing: 10

            Repeater {
                id: presetButtons
                model: 16

                PresetButton {
                    text: index + 1
                    onClicked: {
                        currentPreset = index
                        backend.setPreset(currentPreset)
                    }
                }
            }
        }

        Rectangle {
            id: rectangle1
            x: 0
            y: 728
            width: 1024
            height: 40
            color: "#000000"
            visible: true
        }

        Text {
            id: deviceStatusLabel
            x: 25
            y: 12
            color: "#e8ac5b"
            //            text: backend.deviceStatus //qsTr("Ready!")
            font.pixelSize: 14
            font.bold: true
            font.family: "Arial"

            onTextChanged: {
                if (text !== "unplugged"){
                    deviceBar.value = 1
                }else {
                    comp_name.text = "component"
                    deviceBar.value = 0
                }
            }
        }

        ProgressBar {
            id: deviceBar
            x: 799
            y: 17
            Behavior on value {
                NumberAnimation { duration: 1250 }
            }
        }

    }
}
