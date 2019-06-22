/****************************************************************************}
{ SettingsForm.qml - application settings form                               }
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
import QtQuick.Window 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import net.alkatrazstudio.gpufanmeister 1.0
import net.alkatrazstudio.WindowStateManager 1.0
import '../settings'
import '../components'

DialogWindow {
    id: settingsWindow
    title: qsTr('Application settings')

    property AppConf conf

    function cancel()
    {
        conf.load()
        unsavedChanges.reset()
        close()
    }

    function save()
    {
        conf.save()
        unsavedChanges.reset()
        close()
    }

    UnsavedChanges {
        id: unsavedChanges

        obj: conf
        props: [
            'closeOnSave',
            'closeOnLoad'
        ]

        onSave: settingsWindow.save()
        onCancel: settingsWindow.cancel()
    }

    onClosing: close.accepted = unsavedChanges.check()
    onVisibleChanged: if(visible) unsavedChanges.reset()

    CheckBox {
        id: chkCloseOnSave
        Layout.fillWidth: true
        text: qsTr('Close the main window when "Save" is pressed')
        checked: conf.closeOnSave
        onCheckedChanged: conf.closeOnSave = checked
    }

    CheckBox {
        id: chkCloseOnCancel
        Layout.fillWidth: true
        text: qsTr('Close the main window when "Cancel" is pressed')
        checked: conf.closeOnLoad
        onCheckedChanged: conf.closeOnLoad = checked
    }

    Item {
        Layout.fillHeight: true
    }

    RowLayout {
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignRight

        Button {
            text: qsTr('Cancel')
            onClicked: cancel()
        }

        Button {
            text: qsTr('Save')
            onClicked: save()
        }
    }

    Component.onCompleted: conf.load()
}
