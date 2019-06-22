/****************************************************************************}
{ ModeSettings.qml - GPU modes container                                     }
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
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import net.alkatrazstudio.gpufanmeister 1.0
import './modes'

StackLayout {
    id: root

    property GpuDevice dev
    property var modes
    property var conf

    Layout.fillHeight: true
    Layout.fillWidth: true

    currentIndex: modes.findIndex(item => item.mode === dev.mode)

    CurveMode {dev: parent.dev}
    FanToggleMode {dev: parent.dev}

    ScriptMode {
        id: modeScript
        dev: parent.dev
        conf: root.conf
    }

    OffMode {dev: parent.dev}

    function requestClose(y, n)
    {
        modeScript.requestClose(y, n)
    }
}
