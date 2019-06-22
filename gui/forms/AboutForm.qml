/****************************************************************************}
{ AboutForm.qml - general information about application                      }
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
import net.alkatrazstudio.gpufanmeister 1.0
import '../components'

DialogWindow {
    property App app

    title: app.applicationDisplayName

    Label {
        text: app.applicationDisplayName
        Layout.alignment: Qt.AlignHCenter
        font.pointSize: normalLabel.font.pointSize * 1.5
        font.bold: true
    }

    Label  {
        id: normalLabel
        Layout.alignment: Qt.AlignHCenter
        text: 'v'+app.version+' ('+qsTr('built:')+' '+app.buildDate.toLocaleDateString()+')'
    }

    Label {
        text: qsTr('Project site:')+' <a href="https://gpufanmeister.alkatrazstudio.net">https://gpufanmeister.alkatrazstudio.net</a>'
        onLinkActivated: Qt.openUrlExternally(link)
    }

    Label {
        text: qsTr('Author site:')+' <a href="https://alkatrazstudio.net">https://alkatrazstudio.net</a>'
        onLinkActivated: Qt.openUrlExternally(link)
    }

    Button {
        Layout.alignment: Qt.AlignRight
        text: qsTr('Close')
        onClicked: close()
    }
}
