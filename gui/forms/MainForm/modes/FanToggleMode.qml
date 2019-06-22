/****************************************************************************}
{ FanMode.qml - settings for "fan toggle" mode                               }
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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import net.alkatrazstudio.gpufanmeister 1.0

ColumnLayout {
    property GpuDevice dev

    Label {
        Layout.fillWidth: true

        text: qsTr(
            'The fans are enabled or disabled by the rules below.\n'
            +'Disabled fans are controlled by this application and their speed is always zero.\n'
            +'Enabled fans are controlled by GPU and their speed may or may not be zero.\n'
            +'Temperature is measured in degrees Celcium.\n'
            +'Fan speed is measured in percents.')
        wrapMode: Text.WordWrap

        padding: 10

        background: Rectangle {
            color: palette.light
        }
    }

    ColumnLayout {
        Layout.alignment: Qt.AlignTop
        Layout.fillHeight: true

        Layout.margins: 10

        GridLayout {
            columns: 4

            // auto mode temp

            Label {}

            Label {
                text: qsTr('If the temperature is', 'enable fans temp')
            }

            SpinBox {
                value: dev.fanToggleEnableFansTemp
                onValueChanged: dev.fanToggleEnableFansTemp = value
            }

            Label {
                text: qsTr('or higher', 'enable fans temp')
            }

            Label {}

            Label {
                text: qsTr('for more than', 'enable fans temp')
            }

            SpinBox {
                value: dev.fanToggleEnableFansTempSecs
                onValueChanged: dev.fanToggleEnableFansTempSecs = value
            }

            Label {
                text: qsTr('seconds then enable the fans.', 'enable fans temp')
            }

            // low fan speed

            CheckBox {
                checked: dev.fanToggleUseLowFanSpeed
                onCheckedChanged: dev.fanToggleUseLowFanSpeed = checked
            }

            Label {
                text: qsTr('If the fan speed is', 'low fan speed')
                enabled: dev.fanToggleUseLowFanSpeed
            }

            SpinBox {
                value: dev.fanToggleLowFanSpeed
                onValueChanged: dev.fanToggleLowFanSpeed = value
                enabled: dev.fanToggleUseLowFanSpeed
            }

            Label {
                text: qsTr('or lower', 'low fan speed')
                enabled: dev.fanToggleUseLowFanSpeed
            }

            Label {}

            Label {
                text: qsTr('for more than', 'low fan speed')
                enabled: dev.fanToggleUseLowFanSpeed
            }

            SpinBox {
                value: dev.fanToggleLowFanSpeedSecs
                onValueChanged: dev.fanToggleLowFanSpeedSecs = value
                enabled: dev.fanToggleUseLowFanSpeed
            }

            Label {
                text: qsTr('seconds then disable the fans.', 'low fan speed')
                enabled: dev.fanToggleUseLowFanSpeed
            }

            // low temp

            CheckBox {
                checked: dev.fanToggleUseLowTemp
                onCheckedChanged: dev.fanToggleUseLowTemp = checked
            }

            Label {
                text: qsTr('If the temperature is', 'low temp')
                enabled: dev.fanToggleUseLowTemp
            }

            SpinBox {
                value: dev.fanToggleLowTemp
                onValueChanged: dev.fanToggleLowTemp = value
                enabled: dev.fanToggleUseLowTemp
            }

            Label {
                text: qsTr('or lower', 'low temp')
                enabled: dev.fanToggleUseLowTemp
            }

            Label {}

            Label {
                text: qsTr('for more than', 'low temp')
                enabled: dev.fanToggleUseLowTemp
            }

            SpinBox {
                value: dev.fanToggleLowTempSecs
                onValueChanged: dev.fanToggleLowTempSecs = value
                enabled: dev.fanToggleUseLowTemp
            }

            Label {
                text: qsTr('seconds then disable the fans.', 'low temp')
                enabled: dev.fanToggleUseLowTemp
            }
        }
    }

    Item {Layout.fillHeight: true}
}
