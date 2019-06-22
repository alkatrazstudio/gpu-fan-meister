/****************************************************************************}
{ PluginInfo.qml - information about a GPU plugin                            }
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
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import net.alkatrazstudio.gpufanmeister 1.0

GroupBox {
    property GpuDevicePlugin plugin
    property bool scrollable: false
    property int maxDisplayedHeight: 300

    id: root

    ColumnLayout {
        anchors.fill: parent

        Label {
            Layout.fillWidth: true
            text: plugin.name
            font.pointSize: root.font.pointSize * 1.1
            font.bold: true
        }

        ScrollView {
            id: scroll

            Layout.fillWidth: true
            Layout.maximumHeight: scrollable ? maxDisplayedHeight : -1
            clip: true
            visible: scrollable

            // default AsNeeded still hides the scrollbar when the ScrollView is not in focus
            ScrollBar.vertical.policy: grid.height > scroll.height
                                           ? ScrollBar.AlwaysOn
                                           : ScrollBar.AlwaysOff
            // vertical scrollbar obscures the text and prevents links to be clickable
            // even if the actual scrolling is not needed
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            PluginInfoContent {
                id: grid
                plugin: root.plugin
                width: scroll.width - scroll.ScrollBar.vertical.width
            }
        }

        // can't disable ScrollView when scrolling is not needed
        PluginInfoContent {
            visible: !scrollable
            plugin: root.plugin
            Layout.fillWidth: true
        }

    }
}
