import QtQuick 2.0
import QtQuick.Controls 2.2

Button {
    id: componentButton

    property string name: ""
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
                componentButton.checked ? "qrc:/images/buttonS.svg" : "qrc:/images/button.svg"
            else if(type === 2)
                componentButton.checked ? "qrc:/images/knobS.svg" : "qrc:/images/knob.svg"
            else {
                ""
            }
        }
    }

    background: Rectangle{
        color : "transparent"
    }
}
