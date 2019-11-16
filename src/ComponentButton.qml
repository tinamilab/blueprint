import QtQuick 2.0
import QtQuick.Controls 2.2

Button {
    id: componentButton

    autoExclusive: true
    checkable: true

    property string name: ""
    property int type: 0

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
