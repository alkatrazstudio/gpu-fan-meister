/****************************************************************************}
{ ScriptMode.qml - settings for "script" mode                                }
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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import net.alkatrazstudio.gpufanmeister 1.0
import net.alkatrazstudio.CodeEditor 1.0
import QtQuick.Controls.Material 2.1
import QtQuick.Dialogs 1.3
import '../../../settings'
import net.alkatrazstudio.SimpleMessageDialog 1.0

ColumnLayout {
    property GpuDevice dev
    property MainConf conf

    readonly property bool isRunningCurrent: dev.runningScriptText == editor.text

    spacing: 0

    Rectangle {
        Layout.fillWidth: true
        height: rowLayout.implicitHeight
                + rowLayout.anchors.topMargin
                + rowLayout.anchors.bottomMargin

        color: palette.light

        RowLayout {
            id: rowLayout
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            anchors.topMargin: 5
            anchors.bottomMargin: 5

            Label {
                property string scriptStatus: {
                    return [
                        dev.scriptStatus == dev.SCRIPT_STATUS_RUNNING
                        ? (isRunningCurrent
                            ? qsTr('running', 'script')
                            : qsTr('running old script'))
                        : (dev.scriptStatus == dev.SCRIPT_STATUS_PAUSED
                            ? (isRunningCurrent
                               ? qsTr('paused', 'script')
                               : qsTr('paused old script'))
                            : qsTr('stopped', 'script')),
                        editor.isSaved ? null : qsTr('not saved', 'script')
                    ].filter(s => s).join(', ')
                }

                textFormat: Text.StyledText
                text: qsTr('Script status')
                    + ': <strong>'+scriptStatus+'</strong>'
                wrapMode: Text.WordWrap
            }

            Item {
                Layout.fillWidth: true
            }

            CheckBox {
                text: qsTr('Override', 'script')
                checked: dev.scriptOverride
                onCheckedChanged: dev.scriptOverride = checked
                Layout.rightMargin: 50
            }

            Button {
                Layout.alignment: Qt.AlignRight
                text: qsTr('Apply && restart')

                onClicked: dev.restartScript()
            }

            Button {
                Layout.alignment: Qt.AlignRight
                text: qsTr('Stop')

                onClicked: dev.stopScript()
            }
        }
    }

    CodeEditor {
        id: editor

        property string savedText: ''
        readonly property bool isSaved: savedText === text

        Layout.fillHeight: true
        Layout.fillWidth: true

        syntax: 'JavaScript'
        text: dev.scriptText
        onTextChanged: dev.scriptText = text
        font.pointSize: conf.editorPtSize
        onNewPtSizeRequest: conf.editorPtSize = newPtSize

        function saveText()
        {
            savedText = text
        }
    }

    Label {
        Layout.fillWidth: true
        visible: dev.scriptError !== ''
        text: dev.scriptError
        color: palette.highlightedText
        textFormat: Text.PlainText
        background: Rectangle {
            color: palette.highlight
        }
        padding: 10
    }

    Component.onCompleted: {
        dev.onSaveDone.connect(() => editor.saveText())
        dev.onLoadDone.connect(() => editor.saveText())
        editor.saveText()
    }

    SimpleMessageDialog {id: dlg}

    function checkUnsaved(y, n)
    {
        if(editor.isSaved)
            return y()

        dlg.confirm(
            dev.name,
            qsTr('The script has unsaved changes. Really close the window?'),
            y, n)
    }

    function checkOld(y, n)
    {
        if(dev.scriptStatus !== dev.SCRIPT_STATUS_RUNNING || isRunningCurrent)
            return y()

        dlg.confirm(
            dev.name,
            qsTr('The old script is still running. Really close the window?'),
            y, n)
    }

    function requestClose(y, n)
    {
        checkUnsaved(() => checkOld(y, n), n)
    }
}
