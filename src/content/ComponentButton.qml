import QtQuick 2.0
import QtQuick.Controls 2.2

Button {
    id: componentButton

    property var name: ""
    property int type: 0
    property int row: 0
    property int column: 0
    property int index: row * 4 + column

    x: 34 + 81 * column
    y: 50 + 81 * row

    function codeName() {
        if(type === 1)
            return "button " + name;
        if(type === 2)
            return "knob " + name;
    }

    contentItem: Image {
        width: 81
        height: 81
        source: {
            if(type === 1)
                componentButton.checked ? "qrc:///content/images/buttonS.svg" : "qrc:///content/images/button.svg"
            if(type === 2)
                componentButton.checked ? "qrc:///content/images/knobS.svg" : "qrc:///content/images/knob.svg"
        }
    }

    background: Rectangle{
        color : "transparent"
    }
}
