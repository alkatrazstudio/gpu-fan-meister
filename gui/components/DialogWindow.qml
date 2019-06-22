/****************************************************************************}
{ DialogWindow.qml - auto-sized modal window                                 }
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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Window 2.11

ApplicationWindow {
    default property alias contents: mainLayout.data

    property int minWidth: 0
    property int maxWidth: 0
    property int minHeight: 0
    property int maxHeight: 0

    flags: Qt.Dialog
           | Qt.CustomizeWindowHint
           | Qt.WindowTitleHint
           | Qt.WindowSystemMenuHint
           | Qt.WindowCloseButtonHint
    modality: Qt.ApplicationModal

    Material.theme: Material.System

    readonly property int calcWidth: {
        let w = mainLayout.implicitWidth
            + mainLayout.anchors.leftMargin
            + mainLayout.anchors.rightMargin
            + 1 // some glitch may truncate the text otherwise

        if(minWidth)
            w = Math.max(minWidth, w)
        if(maxWidth)
            w = Math.min(maxWidth, w)
        w = Math.min(w, Screen.desktopAvailableWidth)

        return w
    }

    readonly property int calcHeight: {
        let h = mainLayout.implicitHeight
            + mainLayout.anchors.topMargin
            + mainLayout.anchors.bottomMargin

        if(minHeight)
            h = Math.max(minHeight, h)
        if(maxHeight)
            h = Math.min(maxHeight, h)
        h = Math.min(h, Screen.desktopAvailableHeight)

        return h
    }

    minimumWidth: calcWidth
    minimumHeight: calcHeight

    maximumWidth: minimumWidth
    maximumHeight: minimumHeight

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 10
    }
}
