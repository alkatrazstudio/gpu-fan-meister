/****************************************************************************}
{ PluginInfoContent.qml - reusable part of PluginInfo.qml                    }
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
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import net.alkatrazstudio.gpufanmeister 1.0

GridLayout {
    id: grid

    property GpuDevicePlugin plugin

    columns: 2

    Label {
        text: qsTr('ID:')
        Layout.alignment: Qt.AlignRight
        font.bold: true
    }

    Label {
        text: plugin.id
        textFormat: Text.PlainText
        Layout.fillWidth: true
    }

    Label {
        text: qsTr('Version:')
        Layout.alignment: Qt.AlignRight
        font.bold: true
    }

    Label {
        text: plugin.version
        Layout.fillWidth: true
    }

    Label {
        text: qsTr('Build date:')
        Layout.alignment: Qt.AlignRight
        font.bold: true
        visible: !isNaN(plugin.buildDate)
    }

    Label {
        text: plugin.buildDate.toLocaleDateString()
        Layout.fillWidth: true
        visible: !isNaN(plugin.buildDate)
    }

    Label {
        text: qsTr('Category:')
        Layout.alignment: Qt.AlignRight
        font.bold: true
    }

    Label {
        text: plugin.category
        textFormat: Text.PlainText
        Layout.fillWidth: true
    }

    Label {
        text: qsTr('Description:')
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        font.bold: true
    }

    RichMultiLine {
        richText: plugin.description
    }

    Label {
        text: qsTr('Copyright:')
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        font.bold: true
    }

    RichMultiLine {
        richText: plugin.copyright
    }

    Label {
        text: qsTr('Device count:')
        Layout.alignment: Qt.AlignRight
        font.bold: true
    }

    Label {
        text: plugin.deviceCount
        Layout.fillWidth: true
    }

    Label {
        text: qsTr('Misc. info:')
        Layout.alignment: Qt.AlignRight | Qt.AlignTop
        font.bold: true
        visible: plugin.misc
    }

    RichMultiLine {
        richText: plugin.misc
        visible: plugin.misc
    }
}
