import QtQuick 2.6

ListView {
    id: view
    width: 800
    height: 480

    property bool useProxy: false

    model: useProxy ? proxyModel : myModel
    orientation: ListView.Horizontal
    property var currentDateTime: myModel.currentDateTime
    onModelChanged: reposition()
    onCurrentDateTimeChanged: reposition()

    function reposition() {
        positionViewAtIndex(model.indexForCurrentDateTime, ListView.Beginning);
    }

    PinchArea {
        id: pinchy
        anchors.fill: parent
        onPinchFinished: {
            if (pinch.scale < 1) // zoom out
                useProxy = true
            else if (pinch.scale > 1)
                useProxy = false
        }
    }

    MultiPointTouchArea {
        enabled: false
        property real startX: 0
        property real startY: 0
        anchors.fill: parent
        onPressed: { startX = touchPoints[0].x; startY = touchPoints[0].y; }
        onReleased: {
            // touchPoints[0].startX/Y is always 0, no idea why
            var diffX = touchPoints[0].x - startX;
            var diffY = touchPoints[0].y - startY;
            console.debug("diffX=" + diffX + " diffY=" + diffY);
            if (Math.abs(diffY) > Math.abs(diffX) && Math.abs(diffY) > 10) {
                myModel.toggleBackend();
            }
        }
    }

    delegate: Item {
        height: view.height;
        width: 200

        Rectangle {
            width: parent.width
            height: parent.height; clip: true
            border.color: "#8080b0"; radius: 8
            color: model.time < 8 || model.time > 20 ? "#AAAAAA" : "white"

            // Rectangle { anchors.fill: column; border.color: "blue"; }
            Column {
                width: parent.width
                id: column
                spacing: 10
                padding: 6

                // Date and Time
                Text {
                    property string baseText: model.dayOfWeek + " " + model.time + "h"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 24
                    text: model.dateTime < myModel.currentDateTime ? "(" + baseText + ")" : baseText;
                }
                Item { width: parent.width ; height: 10 }

                // Temperature
                Image { anchors.horizontalCenter: parent.horizontalCenter
                    width: 100
                    height: 100
                    source: model.weatherIcon
                }
                Text { anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 24; text: model.weatherDescription }

                Text { anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 24;
                    font.bold: model.time === 15
                    text: model.temperature + "°C" }

                // Wind
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

    Text {
        anchors { bottom: parent.bottom; right: parent.right }
        text: myModel.backendName + " last updated " + myModel.lastSuccessfulUpdate;
    }
}
