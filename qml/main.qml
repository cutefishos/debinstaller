import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import FishUI 1.0 as FishUI
import Cutefish.DebInstaller 1.0

FishUI.Window {
    id: control
    width: 480
    height: 450
    minimumWidth: 480
    minimumHeight: 450
    maximumWidth: 480
    maximumHeight: 450
    title: qsTr("Package Installer")
    visible: true

    background.color: FishUI.Theme.secondBackgroundColor

    headerItem: Label {
        text: control.title
        leftPadding: FishUI.Units.largeSpacing
    }

    DropArea {
        id: _dropArea
        anchors.fill: parent

        onDropped: {
            if (drop.hasUrls)
                Installer.fileName = drop.urls[0]
        }
    }

    Component {
        id: _homePage

        HomePage { }
    }

    Component {
        id: _appPage

        AppPage { }
    }

    Component {
        id: _installPage

        InstallPage { }
    }

    StackView {
        id: _stackView
        anchors.fill: parent
        initialItem: _homePage
    }

    Connections {
        target: Installer

        function onValidChanged() {
            if (Installer.valid)
                _stackView.push(_appPage)
        }

        function onStatusChanged() {
            if (Installer.status == DebInstaller.Installing)
                _stackView.push(_installPage)
        }

//        function onRequestSwitchToInstallPage() {
//            _stackView.push(_installPage)
//        }
    }
}
