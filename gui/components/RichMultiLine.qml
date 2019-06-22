/****************************************************************************}
{ RichMultiLine.qml - multiline text with email/web links support            }
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
import QtQuick 2.11
import QtQuick.Window 2.2

Label {
    id: richLabel

    property string richText

    Layout.fillWidth: true

    text: richText
        .replace(/</g, '&lt;')
        .replace(/&/g, '&amp;')
        .replace(/\w+:\/\/([^":@\s\.\(\)\[\]\{\}]|(\.[\w\d]))+/g, '<a href="$&">$&</a>')
        .replace(/[\w\d][^":@\s]*@([^":@\s\.\(\)\[\]\{\}]|(\.[\w\d]))+/g, '<a href="mailto:$&">$&</a>')
        .replace(/\n/g, '<br>')
    textFormat: Text.StyledText
    wrapMode: Text.Wrap
    width: Layout.implicitWidth

    onLinkActivated: Qt.openUrlExternally(link)

    MouseArea {
        anchors.fill: parent
        cursorShape: richLabel.hoveredLink
                     ? Qt.PointingHandCursor
                     : Qt.ArrowCursor

        // pass clicks to the parent
        onPressed: mouse.accepted = false
    }
}
