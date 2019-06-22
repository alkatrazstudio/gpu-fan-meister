/****************************************************************************}
{ MainForm.qml - main window                                                 }
{                                                                            }
{ Copyright (c) 2018 Alexey Parfenov <zxed@alkatrazstudio.net>               }
{                                                                            }
{ This file is part of GPU Fan Meister.                                      }
{                                                                            }
{ GPU Fan Meister is free software: you can redistribute it and/or modify it }
{ under the terms of the GNU General Public License as published by          }
{ the Free Software Foundation, either version 3 of the License,             }
{ or (at your option) any later version.                                     }
{                                                                            }
{ GPU Fan Meister is distributed in the hope that it will be useful,         }
{ but WITHOUT ANY WARRANTY; without even the implied warranty of             }
{ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU           }
{ General Public License for more details: https://gnu.org/licenses/gpl.html }
{****************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import net.alkatrazstudio.gpufanmeister 1.0
import net.alkatrazstudio.WindowStateManager 1.0
import '../../forms'
import '../../components'
import '../../settings'

ApplicationWindow {
    id: mainWindow
    objectName: 'mainWindow' // for WindowStateManager

    Material.theme: Material.System

    visible: false
    title: app.applicationDisplayName
    minimumWidth: 1000   // TODO:
    minimumHeight: 700  //   calculate this

    property App app
    property GpuDeviceList devList: app.devList
    property int devIndex: 0
    property GpuDevice curDev: devList.devices.length
                                 ? devList.devices[devIndex]
                                 : null

    property bool canExit: false

    signal menuSettingsFormClicked
    signal menuEnabledPluginsFormClicked
    signal menuCurrentPluginsFormClicked
    signal menuAboutFormClicked

    onClosing: {
        if(canExit)
        {
            close.accepted = true
            canExit = false
            return
        }
        close.accepted = false

        requestClose(() => {
            canExit = true
            mainWindow.close()
        })
    }

    function requestCloseDevTab(y, n, tabIndex)
    {
        if(tabIndex >= devTabs.count)
            return y()

        devTabs.itemAt(tabIndex).requestClose(() => requestCloseDevTab(y, n, tabIndex+1), n)
    }

    function requestClose(y, n)
    {
        requestCloseDevTab(y, n, 0)
    }

    onDevListChanged: {
        devIndex = 0
        app.onNeedShowDevTab.connect(dev => {
            devIndex = devList.devices.indexOf(dev)
            mainForm.show()
        })
        app.onNeedShowDevOptions.connect(dev => {
            devIndex = devList.devices.indexOf(dev)
            mainForm.show()
            deviceFormLoader.item.show()
        })
    }

    WindowStateManager {
        win: mainWindow
    }

    AppConf {id: appConf}
    MainConf {id: mainConf}

    menuBar: MenuBar {
        MenuEx {
            title: qsTr('Application')

            MenuItem {
                text: qsTr('Settings...')
                onTriggered: menuSettingsFormClicked()
            }

            MenuItem {
                text: qsTr('Select plugins...')
                onTriggered: menuEnabledPluginsFormClicked()
            }

            MenuItem {
                text: qsTr('Quit')
                onTriggered: app.quit()
            }
        }

        MenuEx {
            id: aboutMenu
            title: qsTr('About')

            MenuItem {
                text: qsTr('About Qt...')
                onTriggered: app.aboutQt()
            }

            MenuItem {
                text: qsTr('About plugins...')
                onTriggered: menuCurrentPluginsFormClicked()
            }

            MenuItem {
                text: qsTr('About {}...').replace('{}', app.applicationDisplayName)
                onTriggered: menuAboutFormClicked()
            }

            MenuItem {
                text: qsTr('Read the manual')
                onTriggered: app.openManual()
            }

            MenuItem {
                text: qsTr('Visit the homepage')
                onTriggered: Qt.openUrlExternally('https://gpufanmeister.alkatrazstudio.net/')
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TabBar {
            id: tabBar
            Layout.fillWidth: true

            Repeater {
                model: devList.devices
                delegate: TabButton {
                    // QML does not provide any reasonable way to not use HTML here
                    // also it treats "&" as a shortcut prefix here
                    text: modelData.name.replace(/&/g, '&&amp;').replace(/</g, '&&lt;')
                }
            }

            currentIndex: devIndex
            onCurrentIndexChanged: devIndex = currentIndex
        }

        StackLayout {
            id: devTabs

            Layout.fillHeight: true
            Layout.fillWidth: true
            currentIndex: tabBar.currentIndex

            Repeater {
                model: devList.devices
                delegate: DevTab {
                    dev: modelData
                    conf: mainConf
                }
            }
        }

        Loader {
            id: deviceFormLoader
            sourceComponent: DeviceForm {
                dev: curDev
                onHelpClick: app.openManual('user/gui/devOptions')
            }
            active: devList.devices.length
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.minimumHeight: implicitHeight
            Layout.maximumHeight: implicitHeight
            Layout.margins: 10

            Button {
                text: qsTr('Help')

                onClicked: {
                    let mode = curDev.mode
                        .split('_')
                        .map(s => s[0]+s.substring(1).toLowerCase())
                        .join('')
                    mode = mode[0].toLowerCase()+mode.substring(1)
                    app.openManual(`user/modes/${mode}`)
                }
            }

            Button {
                text: qsTr('Device options...')

                onClicked: {
                    deviceFormLoader.item.show()
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr('Cancel')

                onClicked: {
                    mainConf.load()
                    devList.loadDeviceModeSettings()
                    if(appConf.closeOnLoad)
                        close()
                }
            }

            Button {
                text: qsTr('Save')

                onClicked: {
                    mainConf.save()
                    devList.saveDeviceModeSettings()
                    if(appConf.closeOnSave)
                        close()
                }
            }
        }
    }
}
