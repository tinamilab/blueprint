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

import "content"

Window {

    visible: true
    width: 1024
    height: 768
    title: qsTr("Tinami")

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
            componentChannel_combo.currentIndex = componentChannel;
        }

        onComponentMinValueChanged: {
            componentMinValue_textInput.text = componentMinValue;
        }

        onComponentMaxValueChanged: {
            componentMaxValue_textInput.text = componentMaxValue;
        }

        onControl0TypeChanged:{

        }
        onControl1TypeChanged:{
        }
        onControl2TypeChanged:{
        }
        onControl3TypeChanged:{
        }
        onControl4TypeChanged:{
        }
        onControl5TypeChanged:{
        }
        onControl6TypeChanged:{
        }
        onControl7TypeChanged:{
        }
        onControl8TypeChanged:{
        }
        onControl9TypeChanged:{
        }
        onControl10TypeChanged:{
        }
        onControl11TypeChanged:{
        }
        onControl12TypeChanged:{
        }
        onControl13TypeChanged:{
        }
        onControl14TypeChanged:{
        }
        onControl15TypeChanged:{
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
    }

    Image {
        id: image
        x: 0
        y: 0
        width: 1024
        height: 768
        source: "content/images/background.png"

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
                source: "content/images/Logo-MD1BP.svg"
            }

            Text {
                id: comp_name
                x: 60
                y: 165
                color: "#e8ac5b"
                text: qsTr("knob a1")
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
            /*TextInput{
                id: cc_txtInput
                x: 186
                y: 288
                width: 200
                height: 30
                color: "#d9d1d1"
                font.pixelSize: 14
                font.family: "Arial"
                text: backend.ccNumber
                font.bold: true
                topPadding: 6
                inputMask: qsTr("")
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                onTextChanged: {
                    backend.setCcNumber(text)
                }
            }*/

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
                    ListElement{text: "Global"}
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

                source: "content/images/clock.svg"
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
            source: "content/images/back.svg"

            property int dx: 81
            property int dy: 81
            property int xo: 34
            property int yo: 50

            property var i: 0
            property var j: 0
            property var component: 0

            function selectComponentWithKeys(){
                back_controler.component = back_controler.i * 4 + back_controler.j;

                switch(back_controler.component){
                case 0:
                    a1ComponentB.checked = true;
                    break;
                case 1:
                    a2ComponentB.checked = true;
                    break;
                case 2:
                    a3ComponentB.checked = true;
                    break;
                case 3:
                    a4ComponentB.checked = true;
                    break;
                case 4:
                    b1ComponentB.checked = true;
                    break;
                case 5:
                    b2ComponentB.checked = true;
                    break;
                case 6:
                    b3ComponentB.checked = true;
                    break;
                case 7:
                    b4ComponentB.checked = true;
                    break;
                case 8:
                    c1ComponentB.checked = true;
                    break;
                case 9:
                    c2ComponentB.checked = true;
                    break;
                case 10:
                    c3ComponentB.checked = true;
                    break;
                case 11:
                    c4ComponentB.checked = true;
                    break;
                case 12:
                    d1ComponentB.checked = true;
                    break;
                case 13:
                    d2ComponentB.checked = true;
                    break;
                case 14:
                    d3ComponentB.checked = true;
                    break;
                case 15:
                    d4ComponentB.checked = true;
                    break;
                }
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

            Rectangle{
                ComponentButton{
                    id: a1ComponentB
                    row: 0
                    column: 0
                    autoExclusive: true
                    checkable: true
                    name: "a1"
                    type: backend.control0Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(a1ComponentB.index);
                    }
                }

                ComponentButton{
                    id: a2ComponentB
                    row: 0
                    column: 1
                    autoExclusive: true
                    checkable: true
                    name: "a2"
                    type: backend.control1Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(a2ComponentB.index);
                    }
                }

                ComponentButton{
                    id: a3ComponentB
                    row: 0
                    column: 2
                    autoExclusive: true
                    checkable: true
                    name: "a3"
                    type: backend.control2Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(a3ComponentB.index);
                    }
                }

                ComponentButton{
                    id: a4ComponentB
                    row: 0
                    column: 3
                    autoExclusive: true
                    checkable: true
                    name: "a4"
                    type: backend.control3Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(a4ComponentB.index);
                    }
                }

                ComponentButton{
                    id: b1ComponentB
                    row: 1
                    column: 0
                    autoExclusive: true
                    checkable: true
                    name: "b1"
                    type: backend.control4Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(b1ComponentB.index);
                    }
                }

                ComponentButton{
                    id: b2ComponentB
                    row: 1
                    column: 1
                    autoExclusive: true
                    checkable: true
                    name: "b2"
                    type: backend.control5Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(b2ComponentB.index);
                    }
                }

                ComponentButton{
                    id: b3ComponentB
                    row: 1
                    column: 2
                    autoExclusive: true
                    checkable: true
                    name: "b3"
                    type: backend.control6Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(b3ComponentB.index);
                    }
                }

                ComponentButton{
                    id: b4ComponentB
                    row: 1
                    column: 3
                    autoExclusive: true
                    checkable: true
                    name: "b4"
                    type: backend.control7Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(b4ComponentB.index);
                    }
                }

                ComponentButton{
                    id: c1ComponentB
                    row: 2
                    column: 0
                    autoExclusive: true
                    checkable: true
                    name: "c1"
                    type: backend.control8Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(c1ComponentB.index);
                    }
                }

                ComponentButton{
                    id: c2ComponentB
                    row: 2
                    column: 1
                    autoExclusive: true
                    checkable: true
                    name: "c2"
                    type: backend.control9Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(c2ComponentB.index);
                    }
                }

                ComponentButton{
                    id: c3ComponentB
                    row: 2
                    column: 2
                    autoExclusive: true
                    checkable: true
                    name: "c3"
                    type: backend.control10Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(c3ComponentB.index);
                    }
                }

                ComponentButton{
                    id: c4ComponentB
                    row: 2
                    column: 3
                    autoExclusive: true
                    checkable: true
                    name: "c4"
                    type: backend.control11Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(c4ComponentB.index);
                    }
                }

                ComponentButton{
                    id: d1ComponentB
                    row: 3
                    column: 0
                    autoExclusive: true
                    checkable: true
                    name: "d1"
                    type: backend.control12Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(d1ComponentB.index);
                    }
                }

                ComponentButton{
                    id: d2ComponentB
                    row: 3
                    column: 1
                    autoExclusive: true
                    checkable: true
                    name: "d2"
                    type: backend.control13Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(d2ComponentB.index);
                    }
                }

                ComponentButton{
                    id: d3ComponentB
                    row: 3
                    column: 2
                    autoExclusive: true
                    checkable: true
                    name: "d3"
                    type: backend.control14Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(d3ComponentB.index);
                    }
                }

                ComponentButton{
                    id: d4ComponentB
                    row: 3
                    column: 3
                    autoExclusive: true
                    checkable: true
                    name: "d4"
                    type: backend.control15Type

                    onCheckedChanged: {
                        comp_name.text = codeName();
                        back_controler.i = row;
                        back_controler.j = column;

                        backend.selectComponent(d4ComponentB.index);
                    }
                }
            }
        }

        Text {
            id: preset_txt
            visible: true
            x: 107
            y: 486
            color: "#e8ac5b"
            text: qsTr("preset")
            horizontalAlignment: Text.AlignRight
            font.bold: true
            font.pixelSize: 18
            font.family: "Arial"
        }

        ComboBox {
            id: global_combo
            visible: true
            x: 244
            y: 638
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
            //onCurrentTextChanged: backend.setDeviceMode(currentIndex)
        }

        Text {
            id: global_txt
            visible: true
            x: 104
            y: 644
            color: "#e8ac5b"
            text: qsTr("global channel")
            horizontalAlignment: Text.AlignRight
            font.bold: true
            font.pixelSize: 18
            font.family: "Arial"
        }
        Rectangle {
            id:presets
            PresetButton {
                id: p1
                visible: true
                x: 107
                y: 517
                checkable: true
                autoExclusive: true
                checked: true
                text: qsTr("1")
                font.family: "Arial"
                onClicked: backend.setPreset(0)
            }

            PresetButton {
                id: p2
                visible: true
                x: 155
                y: 517
                checkable: true
                autoExclusive: true
                text: qsTr("2")
                font.family: "Arial"
                onClicked: backend.setPreset(1)
            }

            PresetButton {
                id: p3
                visible: true
                x: 203
                y: 517
                checkable: true
                autoExclusive: true
                text: qsTr("3")
                font.family: "Arial"
                onClicked: backend.setPreset(2)
            }

            PresetButton {
                id: p4
                visible: true
                x: 251
                y: 517
                checkable: true
                autoExclusive: true
                text: qsTr("4")
                font.family: "Arial"
                onClicked: backend.setPreset(3)
            }

            PresetButton {
                id: p5
                visible: true
                x: 299
                y: 517
                checkable: true
                autoExclusive: true
                text: qsTr("5")
                font.family: "Arial"
                onClicked: backend.setPreset(4)
            }

            PresetButton {
                id: p6
                visible: true
                x: 347
                y: 517
                checkable: true
                autoExclusive: true
                text: qsTr("6")
                font.family: "Arial"
                onClicked: backend.setPreset(5)
            }

            PresetButton {
                id: p7
                visible: true
                x: 395
                y: 517
                checkable: true
                autoExclusive: true
                text: qsTr("7")
                font.family: "Arial"
                onClicked: backend.setPreset(6)
            }

            PresetButton {
                id: p8
                visible: true
                x: 443
                y: 517
                checkable: true
                autoExclusive: true
                text: qsTr("8")
                font.family: "Arial"
                onClicked: backend.setPreset(7)
            }

            PresetButton {
                id: p9
                visible: true
                x: 107
                y: 570
                checkable: true
                autoExclusive: true
                text: qsTr("9")
                font.family: "Arial"
                onClicked: backend.setPreset(8)
            }

            PresetButton {
                id: p10
                visible: true
                x: 155
                y: 570
                checkable: true
                autoExclusive: true
                text: qsTr("10")
                font.family: "Arial"
                onClicked: backend.setPreset(9)
            }

            PresetButton {
                id: p11
                visible: true
                x: 203
                y: 570
                checkable: true
                autoExclusive: true
                text: qsTr("11")
                font.family: "Arial"
                onClicked: backend.setPreset(10)
            }

            PresetButton {
                id: p12
                visible: true
                x: 251
                y: 570
                checkable: true
                autoExclusive: true
                text: qsTr("12")
                font.family: "Arial"
                onClicked: backend.setPreset(11)
            }

            PresetButton {
                id: p13
                visible: true
                x: 299
                y: 570
                checkable: true
                autoExclusive: true
                text: qsTr("13")
                font.family: "Arial"
                onClicked: backend.setPreset(12)
            }

            PresetButton {
                id: p14
                visible: true
                x: 347
                y: 570
                checkable: true
                autoExclusive: true
                text: qsTr("14")
                font.family: "Arial"
                onClicked: backend.setPreset(13)
            }

            PresetButton {
                id: p15
                visible: true
                x: 395
                y: 570
                checkable: true
                autoExclusive: true
                text: qsTr("15")
                font.family: "Arial"
                onClicked: backend.setPreset(14)
            }

            PresetButton {
                id: p16
                visible: true
                x: 443
                y: 570
                checkable: true
                autoExclusive: true
                text: qsTr("16")
                font.family: "Arial"
                onClicked: backend.setPreset(15)
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
                    comp_name.text = "knob a1"
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
