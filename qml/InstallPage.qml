import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import FishUI 1.0 as FishUI
import Cutefish.DebInstaller 1.0

Item {
    id: control

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: FishUI.Units.largeSpacing
        anchors.rightMargin: FishUI.Units.largeSpacing
        anchors.bottomMargin: FishUI.Units.largeSpacing

        RowLayout {
            Item { Layout.fillWidth: true }

            Item {
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30

                Image {
                    anchors.fill: parent
                    sourceSize: Qt.size(width, height)
                    source: Installer.status === DebInstaller.Succeeded ? "qrc:/images/success.svg"
                                                                        : "qrc:/images/error.svg"
                    visible: Installer.status != DebInstaller.Installing
                }

                FishUI.BusyIndicator {
                    anchors.fill: parent
                    width: 30
                    height: 30
                    visible: Installer.status == DebInstaller.Installing
                }
            }

            Label {
                text: Installer.statusMessage
            }

            Item { Layout.fillWidth: true }
        }

        Item {
            height: FishUI.Units.largeSpacing
        }

        TextArea {
            Layout.fillHeight: true
            Layout.fillWidth: true
            text: Installer.statusDetails
            enabled: false
        }

        Button {
            Layout.fillWidth: true
            flat: true
            text: qsTr("Quit")
            enabled: Installer.status == DebInstaller.Succeeded
            onClicked: Qt.quit()
        }
    }
}
