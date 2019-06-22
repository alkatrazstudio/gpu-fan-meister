/****************************************************************************}
{ CurrentPluginsForm.qml - list of currenly active plugins                   }
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
    property var devs

    title: qsTr('Current plugins')
    maxWidth: 999
    maxHeight: 666

    Label {
        text: qsTr('The list of currently loaded plugins')
    }

    ScrollView {
        id: scroll
        Layout.fillWidth: true
        Layout.fillHeight: true
        clip: true

        // see PluginInfo.qml
        ScrollBar.vertical.policy: column.height > scroll.height
                                       ? ScrollBar.AlwaysOn
                                       : ScrollBar.AlwaysOff
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        ColumnLayout {
            id: column
            width: scroll.width - scroll.ScrollBar.vertical.width

            Repeater {
                model: {
                    var arr = []
                    for(let a=0; a<devs.length; a++)
                    {
                        var p = devs[a].plugin
                        if(arr.indexOf(p) == -1)
                            arr.push(p)
                    }
                    return arr
                }

                PluginInfo {
                    Layout.fillWidth: true
                    plugin: modelData
                }
            }
        }
    }
}
