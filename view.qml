import QtQuick 2.6

ListView {
    width: 800
    height: 480
    model: myModel
    orientation: ListView.Horizontal

    delegate: Item {
        height: parent.height;
        width: 200

        Rectangle {
            width: parent.width
            height: parent.height; clip: true
            border.color: "#8080b0"; radius: 8

            Rectangle { anchors.fill: column; border.color: "blue"; }
            Column {
                width: parent.width
                id: column
                spacing: 10
                padding: 6
                Text { anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 24; text: model.dayOfWeek + " " + model.time + "h" }
                Rectangle { width: parent.width ; height: 10 }
                Text { anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 24; text: model.temperature + "°C" }
                Image { anchors.horizontalCenter: parent.horizontalCenter
                    width: 82
                    height: 100
                    source: {
                        if (model.gustWind > 30 || model.averageWind > 30) {
                            "qrc:/pics/wind_turbine.png"
                        } else {
                            ""
                        }
                    }
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 24; text: model.averageWind + " km/h" }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    visible: model.gustWind > 30
                    color: "red"
                    font.pixelSize: 24
                    text: "Raf. " + model.gustWind + " km/h" }
            }
        }
    }
}
