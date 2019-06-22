/****************************************************************************}
{ DeviceForm.qml - device settings form                                      }
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

import QtQuick 2.11
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import net.alkatrazstudio.gpufanmeister 1.0
import net.alkatrazstudio.WindowStateManager 1.0
import net.alkatrazstudio.ColorPicker 1.0
import '../components'

DialogWindow {
    id: deviceWindow
    title: dev.name + ' - ' + qsTr('Device options')

    property GpuDevice dev

    signal helpClick

    function save()
    {
        dev.saveGeneralSettings()
        unsavedChanges.reset()
        close()
    }

    function cancel()
    {
        dev.loadGeneralSettings()
        unsavedChanges.reset()
        close()
    }

    UnsavedChanges {
        id: unsavedChanges

        obj: dev
        props: [
            'customName',

            'colorTempNormal',
            'tempWarning',
            'colorTempWarning',
            'tempDanger',
            'colorTempDanger',

            'colorFanSpeedNormal',
            'fanSpeedWarning',
            'colorFanSpeedWarning',
            'fanSpeedDanger',
            'colorFanSpeedDanger',

            'colorBackground',

            'trayUpdateInterval'
        ]

        onSave: deviceWindow.save()
        onCancel: deviceWindow.cancel()
    }

    onClosing: close.accepted = unsavedChanges.check()
    onVisibleChanged: if(visible) unsavedChanges.reset()

    RowLayout {
        Layout.fillWidth: true

        Label {
            text: qsTr('Display name')
        }

        TextArea {
            Layout.fillWidth: true
            text: dev.customName
            onTextChanged: dev.customName = text
            placeholderText: dev.realName
        }
    }

    GroupBox {
        id: trayColors

        property GpuDevice dev

        Layout.fillWidth: true

        label: Label {
            text: qsTr('Tray icon colors')
            font.bold: true
        }

        GridLayout {
            columns: 6

            Label {
                text: qsTr('Temperature')
                Layout.alignment: Qt.AlignRight
            }

            ColorPicker {
                id: picker

                curColor: dev.colorTempNormal
                onChanged: dev.colorTempNormal = value
                showAlpha: true
            }

            SpinBox {
                value: dev.tempWarning
                onValueChanged: dev.tempWarning = value
                from: 1
                to: dev.tempDanger - 1
            }

            ColorPicker {
                curColor: dev.colorTempWarning
                onChanged: dev.colorTempWarning = value
                showAlpha: true
            }

            SpinBox {
                value: dev.tempDanger
                onValueChanged: dev.tempDanger = value
                from: dev.tempWarning + 1
                to: 99
            }

            ColorPicker {
                curColor: dev.colorTempDanger
                onChanged: dev.colorTempDanger = value
                showAlpha: true
            }

            Label {
                text: qsTr('Fan speed')
                Layout.alignment: Qt.AlignRight
            }

            ColorPicker {
                curColor: dev.colorFanSpeedNormal
                onChanged: dev.colorFanSpeedNormal = value
                showAlpha: true
            }

            SpinBox {
                value: dev.fanSpeedWarning
                onValueChanged: dev.fanSpeedWarning = value
                from: 1
                to: dev.fanSpeedDanger - 1
            }

            ColorPicker {
                curColor: dev.colorFanSpeedWarning
                onChanged: dev.colorFanSpeedWarning = value
                showAlpha: true
            }

            SpinBox {
                value: dev.fanSpeedDanger
                onValueChanged: dev.fanSpeedDanger = value
                from: dev.fanSpeedWarning + 1
                to: 99
            }

            ColorPicker {
                curColor: dev.colorFanSpeedDanger
                onChanged: dev.colorFanSpeedDanger = value
                showAlpha: true
            }

            Label {
                text: qsTr('Background')
                Layout.alignment: Qt.AlignRight
            }

            ColorPicker {
                id: bgPicker
                curColor: dev.colorBackground
                onChanged: dev.colorBackground = value
                showAlpha: true

                // can't just bind because picker.height is initially zero
                // and changing preferredHeight afterwards makes the widget invisible
                Component.onCompleted: bgPicker.Layout.preferredHeight = picker.height
            }

            Item {
                Layout.rowSpan: 4
            }
        }
    }

    RowLayout {
        Layout.fillWidth: true

        Label {
            text: qsTr('Update the tray icon every')
        }

        SpinBox {
            from: 1
            to: 60
            value: dev.trayUpdateInterval
            onValueChanged: dev.trayUpdateInterval = value
        }

        Label {
            text: qsTr('seconds', 'Update the tray icon every')
        }
    }

    PluginInfo {
        id: pluginInfo

        plugin: dev.plugin
        scrollable: true

        visible: false
        label: Label {
           text: qsTr('Plugin info')
           font.bold: true
        }
        Layout.fillWidth: true
        Layout.maximumWidth: trayColors.width
    }

    Item {
        Layout.fillHeight: true
    }

    RowLayout {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignRight

        Button {
            text: qsTr('Help')
            onClicked: helpClick()
        }

        Button {
            text: pluginInfo.visible
                  ? qsTr('Hide plugin info')
                  : qsTr('Show plugin info')
            onClicked: pluginInfo.visible = !pluginInfo.visible
        }

        Item {
            Layout.fillWidth: true
        }

        Button {
            id: btnCancel
            text: qsTr('Cancel')
            onClicked: cancel()
        }

        Button {
            id: btnSave
            text: qsTr('Save')
            onClicked: save()
        }
    }
}
