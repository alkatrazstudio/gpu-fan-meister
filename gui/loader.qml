/****************************************************************************}
{ loader.qml - GUI entry point loader                                        }
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
import net.alkatrazstudio.gpufanmeister 1.0

Loader {
    id: root
    property App app: APP

    sourceComponent: Main {
        app: root.app
    }
    active: root.app

    onAppChanged: {
        if(app)
            app.aboutToQuit.connect(() => app = null)
    }
}
