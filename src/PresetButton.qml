import QtQuick 2.0
import QtQuick.Controls 2.2

Button{
    id: presetButton

    contentItem: Text {
        text: presetButton.text
        font: presetButton.font
        color: presetButton.checked ? "#0050b4" : "#d8d9d1"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle{
        implicitWidth: 33
        implicitHeight: 43
        color: presetButton.checked ? "#e8ac5b" : "transparent"
        border.color: presetButton.checked ? "#e8ac5b" : "#d8d9d1"
        border.width: 3
        radius: 8
    }
}

