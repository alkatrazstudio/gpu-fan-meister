/****************************************************************************}
{ gpu-fan-meister.qbs - main project file                                    }
{                                                                            }
{ This file is a part of the project                                         }
{   GPU Fan Meister - GPU fan speed controller and indicator                 }
{                                                                            }
{ Copyright (c) 2017 Alkatraz Studio                                         }
{                                                                            }
{ author: Alexey Parfenov a.k.a. ZXED                                        }
{ author's e-mail: zxed@alkatrazstudio.net                                   }
{                                                                            }
{ GPU Fan Meister is free software: you can redistribute it and/or           }
{ modify it under the terms of the GNU General Public License                }
{ as published by the Free Software Foundation, either version 3 of          }
{ the License, or (at your option) any later version.                        }
{                                                                            }
{ GPU Fan Meister is distributed in the hope that it will be useful,         }
{ but WITHOUT ANY WARRANTY; without even the implied warranty of             }
{ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU           }
{ General Public License for more details.                                   }
{                                                                            }
{ You may read the GNU General Public License at:                            }
{   http://www.gnu.org/copyleft/gpl.html                                     }
{                                                                            }
{ project started: 6 May 2017                                                }
{****************************************************************************/

import qbs.FileInfo
import qbs.File
import qbs.TextFile
import qbs.Process

Project {
    minimumQbsVersion: '1.13.0'

    qbsSearchPaths: ['./common', './shared/qbs']

    QtGuiApplication {
        property stringList qmlImportPaths: [
            QmlAppSettings.qmlImportPath,
            CodeEditor.qmlImportPath,
            ColorPicker.qmlImportPath,
            SimpleMessageDialog.qmlImportPath,
            WindowStateManager.qmlImportPath
        ]

        name: project.name
        version: Version.autoVersion

        Group {
            name: 'App'

            files: [
                '*.cpp',
                '*.h'
            ]
        }

        Depends {
            name: 'Qt'
            submodules: ['core', 'gui', 'qml', 'widgets', 'quickcontrols2']
        }

        Depends {name: 'Version'}
        Depends {name: 'Common'}
        Depends {name: 'CoreApp'}
        Depends {name: 'GpuFanMeisterIncludes'}
        Depends {name: 'QtNotify'}
        Depends {name: 'AppSettings'}
        Depends {name: 'CodeHighlighter'}
        Depends {name: 'QmlSimple'}
        Depends {name: 'EnumUtil'}

        Depends {name: 'QmlAppSettings'}
        Depends {name: 'CodeEditor'}
        Depends {name: 'ColorPicker'}
        Depends {name: 'SimpleMessageDialog'}
        Depends {name: 'WindowStateManager'}

        CoreApp.singleInstanceKey: name
        Common.appTitle: 'GPU Fan Meister'

        cpp.defines: 'GPUFANMEISTER_NO_PLUGIN_EXPORT'
        qbs.installPrefix: ''

        Group {
            name: 'Scripts'
            files: product.name+'.py'
            qbs.install: Common.realInstall
        }

        Group {
            name: 'GUI'
            files: 'gui/**/**.qml'
            qbs.installSourceBase: ''
            qbs.install: Common.realInstall
        }

        Group {
            name: 'Script template'
            files: 'script.default.js'
            qbs.install: Common.realInstall
        }

        Group {
            name: 'Assets'
            files: [
                'assets/'+Common.appReverseDomain+'.appdata.xml',
                'assets/'+Common.appReverseDomain+'.desktop',
                'assets/'+product.name+'.png'
            ]
            qbs.installSourceBase: ''
            qbs.install: Common.realInstall
        }

        Group {
            name: 'ThirdParty'
            files: [
                'pynotify2/notify2.py'
            ]
            qbs.installSourceBase: ''
            qbs.install: Common.realInstall
        }

        Probe {
            id: docDirsProbe
            property stringList dirs
            configure: {
                dirs = File.directoryEntries(path+'/docs', File.Dirs | File.NoDotAndDotDot)
                dirs = dirs.filter(function(s){return s != 'src'})
                found = true
            }
        }

        Group {
            name: 'Doc'
            files: docDirsProbe.dirs.map(function(s){return 'docs/'+s+'/**'})
            qbs.installSourceBase: ''
            qbs.install: Common.realInstall
        }
    }
}
