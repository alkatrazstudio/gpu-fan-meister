/****************************************************************************}
{ DevTab.qml - device pane                                                   }
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

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import net.alkatrazstudio.gpufanmeister 1.0

ColumnLayout {
    id: devTab

    property GpuDevice dev
    property var conf

    Layout.fillWidth: true
    Layout.fillHeight: true

    readonly property var modes : [
        dev.MODE_CURVE,
        dev.MODE_FAN_TOGGLE,
        dev.MODE_SCRIPT,
        dev.MODE_GPU
    ].map(m => ({
        mode: m,
        text: dev.modeToStringTitle(m)
    }))

    RowLayout {
        Layout.fillWidth: true
        Layout.leftMargin: spacing

        Repeater {
            model: modes

            delegate: RadioButton {
                text: modelData.text

                checked: dev.mode === modelData.mode

                onCheckedChanged: {
                    if(checked && dev.mode !== modelData.mode)
                        dev.mode = modelData.mode
                }
            }
        }

        Item {Layout.fillWidth: true}
    }

    ModeSettings {
        id: modeSettings

        dev: devTab.dev
        modes: devTab.modes
        conf: devTab.conf
    }

    function requestClose(y, n)
    {
        modeSettings.requestClose(y, n)
    }
}
