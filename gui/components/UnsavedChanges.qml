/****************************************************************************}
{ UnsavedChanges.qml - checks for unsaved changes and show a dialog          }
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
import net.alkatrazstudio.SimpleMessageDialog 1.0

QtObject {
    property var obj
    property var props

    property string dialogTitle: qsTr('Save changes?')
    property string dialogText: qsTr('Do you want to save the changes you made in this window?')

    property bool answered: false
    property var savedObj

    signal save
    signal cancel

    readonly property SimpleMessageDialog dlg: SimpleMessageDialog {}

    function clone(obj)
    {
        if(!obj || typeof obj != 'object')
            return obj
        var copy = obj.constructor()
        Object.keys(obj).forEach(p => copy[p] = clone(obj[p]))
        return copy
    }

    function compare(a, b)
    {
        if(typeof a != typeof b)
            return false
        if(!a || typeof a != 'object' || !b || typeof b != 'object')
            return a === b
        if(!Object.keys(a).every(p => compare(a[p], b[p])))
            return false
        if(!Object.keys(b).every(p => p in a))
            return false
        return true
    }

    function reset()
    {
        answered = false
        savedObj = {}
        props.forEach(p => savedObj[p] = clone(obj[p]))
    }

    function check()
    {
        if(answered)
            return true

        if(props.every(p => compare(obj[p], savedObj[p])))
            return true

        dlg.confirm(dialogTitle, dialogText, () => {
            answered = true
            save()
        }, () => {
            answered = true
            cancel()
        })

        return false
    }
}
