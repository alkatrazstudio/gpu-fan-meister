/****************************************************************************}
{ CurveMode.qml - settings for "curve" mode                                  }
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
import '../../../components'

RowLayout {
    property GpuDevice dev

    InteractiveLineChart {
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.margins: 0

        pts: dev.curvePoints
        temp: dev.temp
        fanSpeed: dev.fanSpeed
        refTemp: dev.curveRefTemp

        onNewPts: dev.curvePoints = newPts
    }

    GroupBox {
        Layout.fillHeight: true
        Layout.rightMargin: 10

        clip: true

        label: Label {
            text: qsTr('Hysteresis')
            font.bold: true
        }

        ColumnLayout {
            anchors.fill: parent

            Label {
                text: qsTr('Max inc. temp. diff.')
            }

            SpinBox {
                from: 0
                to: 999

                value: dev.curveMaxIncTempDiff
                onValueChanged: dev.curveMaxIncTempDiff = value
            }

            Label {
                text: qsTr('Inc. delay')
            }

            SpinBox {
                from: 0
                to: 999

                value: dev.curveIncDelay
                onValueChanged: dev.curveIncDelay = value
            }

            Label {
                text: qsTr('Max dec. temp. diff.')
            }

            SpinBox {
                from: 0
                to: 999

                value: dev.curveMaxDecTempDiff
                onValueChanged: dev.curveMaxDecTempDiff = value
            }

            Label {
                text: qsTr('Dec. delay')
            }

            SpinBox {
                from: 0
                to: 999

                value: dev.curveDecDelay
                onValueChanged: dev.curveDecDelay = value
            }

            Item {Layout.fillHeight: true}
        }
    }
}


