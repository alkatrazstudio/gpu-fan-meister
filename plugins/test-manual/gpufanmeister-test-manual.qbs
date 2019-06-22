/****************************************************************************}
{ gpufanmeister-test-manual.qbs - main project file for the test plugin      }
{                                                                            }
{ Copyright (c) 2018 Alexey Parfenov <zxed@alkatrazstudio.net>               }
{                                                                            }
{ This file is a part of GPU Fan Meister                                     }
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
{****************************************************************************/

Project {
    property string pluginName: 'test-manual'
    property int nDevs: 1

    name: 'gpufanmeister-'+pluginName

    minimumQbsVersion: '1.13.0'
    qbsSearchPaths: ['../../shared/qbs', '../../common']

    DynamicLibrary {
        name: project.name
        version: '1.0.0'

        Depends {name: 'cpp'}
        Depends {name: 'GpuFanMeisterIncludes'}
        Depends {
            name: 'Qt';
            submodules: ['core', 'gui', 'widgets']
        }
        Depends {name: 'Common'}

        files: name+'.cpp'

        cpp.defines: 'NDEVS='+project.nDevs
        qbs.installPrefix: 'plugins/'+pluginName

        Group {
            name: 'Scripts'
            files: [
                project.pluginName+'-init.sh',
                project.pluginName+'-deinit.sh'
            ]
            qbs.install: true
        }
    }
}
