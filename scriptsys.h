/****************************************************************************}
{ scriptsys.h - system access for scripts                                    }
{                                                                            }
{ Copyright (c) 2017 Alexey Parfenov <zxed@alkatrazstudio.net>               }
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

#pragma once

#include "qmlsimple.h"
#include <QObject>
#include <QVariantMap>
#include <QProcess>

class ScriptSys : public QObject
{
    Q_OBJECT

public:
    static ScriptSys &instance();

    SIMPLE_QML_CONST_CALC(QString, buildAbi, QSysInfo::buildAbi())
    SIMPLE_QML_CONST_CALC(QString, buildCpuArchitecture, QSysInfo::buildCpuArchitecture())
    SIMPLE_QML_CONST_CALC(QString, currentCpuArchitecture, QSysInfo::currentCpuArchitecture())
    SIMPLE_QML_CONST_CALC(QString, kernelType, QSysInfo::kernelType())
    SIMPLE_QML_CONST_CALC(QString, kernelVersion, QSysInfo::kernelVersion())
    SIMPLE_QML_CONST_CALC(QString, prettyProductName, QSysInfo::prettyProductName())
    SIMPLE_QML_CONST_CALC(QString, productType, QSysInfo::productType())
    SIMPLE_QML_CONST_CALC(QString, productVersion, QSysInfo::productVersion())

    SIMPLE_QML_CONST(const char*, FAILED_AT_START, "start")
    SIMPLE_QML_CONST(const char*, FAILED_AT_WRITE, "write")
    SIMPLE_QML_CONST(const char*, FAILED_AT_FINISH, "finish")

public:
    Q_INVOKABLE QVariantMap exec(QStringList parts, const QVariantMap& opts = QVariantMap()) const;

protected:
    static QString processErrorToStringId(QProcess::ProcessError err);

private:
    Q_DISABLE_COPY(ScriptSys)
    ScriptSys() : QObject(nullptr){}
};
