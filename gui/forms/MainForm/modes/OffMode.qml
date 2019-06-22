/****************************************************************************}
{ OffMode.qml - "GPU controlled" mode                                        }
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

        text: qsTr('The fans are entirely controlled by the GPU.')
        wrapMode: Text.WordWrap

        padding: 10

        background: Rectangle {
            color: palette.light
        }
    }

    Item {Layout.fillHeight: true}
}
