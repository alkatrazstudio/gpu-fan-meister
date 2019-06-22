/****************************************************************************}
{ Main.qml - GUI entry point                                                 }
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
import net.alkatrazstudio.gpufanmeister 1.0
import './forms'
import './forms/MainForm'
import './settings'

QtObject {
    id: root
    property App app

    property bool quitAfterEnabledPluginsFormClosed

    property MainForm mainForm: MainForm {
        app: root.app

        onMenuEnabledPluginsFormClicked: root.openEnabledPluginsForm()
        onMenuCurrentPluginsFormClicked: currentPluginsForm.show()
        onMenuSettingsFormClicked: settingsForm.show()
        onMenuAboutFormClicked: aboutForm.show()
    }

    property AppConf appConf: AppConf {}

    property SettingsForm settingsForm: SettingsForm {
        conf: appConf
    }

    property AboutForm aboutForm: AboutForm {
        app: root.app
    }

    property EnabledPluginsForm enabledPluginsForm: EnabledPluginsForm {
        app: root.app

        onVisibilityChanged: {
            if(!visible)
            {
                if(quitAfterEnabledPluginsFormClosed)
                    app.quit()
            }
        }
    }

    property CurrentPluginsForm currentPluginsForm: CurrentPluginsForm {
        devs: app.devList.devices
    }

    function openEnabledPluginsForm(quitAfter)
    {
        if(quitAfter)
            quitAfterEnabledPluginsFormClosed = true
        enabledPluginsForm.show()
    }

    Component.onCompleted: app.setMainScriptObject(root)
}
