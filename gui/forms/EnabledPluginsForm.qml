/****************************************************************************}
{ EnabledPluginsForm.qml - plugin selector                                   }
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
import net.alkatrazstudio.SimpleMessageDialog 1.0
import '../components'

DialogWindow {
    title: qsTr('Enabled plugins')
    minWidth: 350

    property App app

    property var enabledPlugins: []

    SimpleMessageDialog {
        id: dlg
    }

    function reset()
    {
        // clone
        enabledPlugins = app.enabledPlugins.map(s => s)
    }

    function save()
    {
        app.enabledPlugins = enabledPlugins
    }

    function onSaveClick()
    {
        if(!enabledPlugins.length)
        {
            dlg.warning(qsTr('No plugins selected'), qsTr('You need to select at least one plugin.'))
            return
        }

        var cats = {}
        for(var a=0; a<enabledPlugins.length; a++)
        {
            var p = enabledPlugins[a]
            var cat = p.split('-')[0]
            if(cats[cat])
            {
                dlg.warning(
                    qsTr('Mutually exclusive plugins'),
                    qsTr("You can't use the following plugins simultaneously because they're from the same category:")
                        +`\n\n${cats[cat]}\n${p}`)
                return
            }
            cats[cat] = p
        }

        save()
        close()
    }

    onVisibleChanged: if(visible) reset()

    Label {
        text: qsTr('Enable plugins that will be loaded at startup.')
        visible: app.availablePlugins.length
    }

    Label {
        text: qsTr('No plugins found.')
        visible: !app.availablePlugins.length
    }

    Repeater {
        model: app.availablePlugins

        CheckBox {
            text: modelData
            checked: enabledPlugins.indexOf(text) !== -1
            onCheckedChanged: {
                // this event is generated right after the component is created
                // so avoid adding duplicates in the list
                const i = enabledPlugins.indexOf(modelData)
                if(checked)
                {
                    if(i == -1)
                        enabledPlugins.push(text)
                }
                else
                {
                    if(i !== -1)
                        enabledPlugins.splice(i, 1)
                }
            }
        }
    }

    Label {
        text: qsTr('The application needs to be restarted to apply the changes.')
        visible: app.availablePlugins.length
    }

    RowLayout {
        Button {
            text: qsTr('Help')
            onClicked: app.openManual('user/gui/enabledPlugins')
        }

        Item {
            Layout.fillWidth: true
        }

        Button {
            text: qsTr('Cancel')
            onClicked: close()
        }

        Button {
            text: qsTr('Save')
            onClicked: onSaveClick()
        }
    }
}
