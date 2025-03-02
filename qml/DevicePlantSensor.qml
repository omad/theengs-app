import QtQuick
import QtQuick.Controls

import ThemeEngine 1.0

Loader {
    id: devicePlantSensor

    property var currentDevice: null

    ////////

    function loadDevice(clickedDevice) {
        // set device
        if (typeof clickedDevice === "undefined" || !clickedDevice) return
        if (!clickedDevice.isPlantSensor) return
        if (clickedDevice === currentDevice) return
        currentDevice = clickedDevice

        // load screen
        devicePlantSensor.active = true
        devicePlantSensor.item.loadDevice()

        // change screen
        appContent.state = "DevicePlantSensor"
    }

    ////////

    function backAction() {
        if (devicePlantSensor.status === Loader.Ready)
            devicePlantSensor.item.backAction()
    }

    ////////////////////////////////////////////////////////////////////////////

    active: false

    asynchronous: false
    sourceComponent: Item {
        id: itemDevicePlantSensor
        implicitWidth: 480
        implicitHeight: 720

        focus: parent.focus

        ////////

        Connections {
            target: currentDevice

            function onStatusUpdated() {
                plantSensorData.updateHeader()
            }
            function onSensorUpdated() {
                plantSensorData.updateHeader()
            }
            function onSensorsUpdated() {
                plantSensorData.updateHeader()
            }
            function onRefreshUpdated() {
                plantSensorData.resetHistoryMode()
                plantSensorData.updateGraph()
                plantSensorHistory.updateData()
            }
            function onHistoryUpdated() {
                plantSensorData.updateGraph()
                plantSensorHistory.updateData()
            }
        }

        Connections {
            target: settingsManager

            function onBigIndicatorChanged() {
                plantSensorData.reloadIndicators()
            }
            function onAppLanguageChanged() {
                plantSensorData.updateStatusText()
                plantSensorData.updateLegendSizes()
            }
            function onGraphHistoryChanged() {
                plantSensorHistory.updateHistoryMode()
            }
        }

        Connections {
            target: ThemeEngine
            function onCurrentThemeChanged() {
                plantSensorData.updateHeader()
                plantSensorHistory.updateHeader()
                plantSensorHistory.updateColors()
            }
        }

        Connections {
            target: appHeader

            // desktop only
            function onDeviceDataButtonClicked() {
                appHeader.setActiveDeviceData()
                plantSensorPages.currentIndex = 0
            }
            function onDeviceHistoryButtonClicked() {
                appHeader.setActiveDeviceHistory()
                plantSensorPages.currentIndex = 1
            }
            function onDeviceSettingsButtonClicked() {
                appHeader.setActiveDeviceSettings()
                plantSensorPages.currentIndex = 2
            }
        }

        Connections {
            target: mobileMenu

            // mobile only
            function onDeviceDataButtonClicked() {
                mobileMenu.setActiveDeviceData()
                plantSensorPages.currentIndex = 0
            }
            function onDeviceHistoryButtonClicked() {
                mobileMenu.setActiveDeviceHistory()
                plantSensorPages.currentIndex = 1
            }
            function onDeviceSettingsButtonClicked() {
                mobileMenu.setActiveDeviceSettings()
                plantSensorPages.currentIndex = 2
            }
        }

        ////////

        Keys.onPressed: (event) => {
            if (event.key === Qt.Key_Left) {
                event.accepted = true
                if (plantSensorPages.currentIndex > 0)
                    plantSensorPages.currentIndex--
            } else if (event.key === Qt.Key_Right) {
                event.accepted = true
                if (plantSensorPages.currentIndex+1 < plantSensorPages.count)
                    plantSensorPages.currentIndex++
            } else if (event.key === Qt.Key_F5) {
                event.accepted = true
                deviceManager.updateDevice(currentDevice.deviceAddress)
            } else if (event.key === Qt.Key_Backspace) {
                event.accepted = true
                backAction()
            }
        }

        function backAction() {
            if (plantSensorPages.currentIndex === 0) { // data
                plantSensorData.backAction()
                return
            }
            if (plantSensorPages.currentIndex === 1) { // history
                if (plantSensorHistory.isHistoryMode()) {
                    plantSensorHistory.resetHistoryMode()
                    return
                }

                appContent.state = "DeviceList"
            }
            if (plantSensorPages.currentIndex === 2) { // sensor settings
                appContent.state = "DeviceList"
                return
            }
        }

        ////////

        function isHistoryMode() {
            return (plantSensorData.isHistoryMode() || plantSensorHistory.isHistoryMode())
        }
        function resetHistoryMode() {
            plantSensorData.resetHistoryMode()
            plantSensorHistory.resetHistoryMode()
        }

        function loadDevice() {
            //console.log("DevicePlantSensor // loadDevice() >> " + currentDevice)

            plantSensorPages.disableAnimation()
            plantSensorPages.currentIndex = 0
            plantSensorPages.interactive = isPhone
            plantSensorPages.enableAnimation()

            plantSensorData.loadData()
            plantSensorHistory.loadData()
            plantSensorHistory.updateHeader()

            if (isMobile) mobileMenu.setActiveDeviceData()
            if (isDesktop) appHeader.setActiveDeviceData()
        }

        ////////////////////////////////////////////////////////////////////

        ItemBannerSync {
            id: bannerSync
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            z: 5
        }

        Item {
            anchors.top: parent.top
            anchors.topMargin: bannerSync.visible ? bannerSync.height : 0
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            SwipeView {
                id: plantSensorPages
                anchors.fill: parent

                interactive: isPhone

                currentIndex: 0
                onCurrentIndexChanged: {
                    if (isDesktop) {
                        if (plantSensorPages.currentIndex === 0)
                            appHeader.setActiveDeviceData()
                        else if (plantSensorPages.currentIndex === 1)
                            appHeader.setActiveDeviceHistory()
                        else if (plantSensorPages.currentIndex === 2)
                            appHeader.setActiveDeviceSettings()
                    } else {
                        if (plantSensorPages.currentIndex === 0)
                            mobileMenu.setActiveDeviceData()
                        else if (plantSensorPages.currentIndex === 1)
                            mobileMenu.setActiveDeviceHistory()
                        else if (plantSensorPages.currentIndex === 2)
                            mobileMenu.setActiveDeviceSettings()
                    }
                }

                function enableAnimation() {
                    contentItem.highlightMoveDuration = 333
                }
                function disableAnimation() {
                    contentItem.highlightMoveDuration = 0
                }

                DevicePlantSensorData {
                    clip: false
                    id: plantSensorData
                }
                DevicePlantSensorHistory {
                    clip: true
                    id: plantSensorHistory
                }
                DevicePlantSensorSettings {
                    clip: false
                    id: plantSensorSettings
                }
            }
        }

        ////////////////////////////////////////////////////////////////////
    }
}
