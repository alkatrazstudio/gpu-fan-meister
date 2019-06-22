/****************************************************************************}
{ scriptsys.cpp - system access for scripts                                  }
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

#include "scriptsys.h"
#include "countdowntimer.h"
#include <QProcess>
#include <QQmlEngine>

static constexpr int DEFAULT_TIMEOUT = 3;

ScriptSys &ScriptSys::instance()
{
    static ScriptSys inst;
    QQmlEngine::setObjectOwnership(&inst, QQmlEngine::CppOwnership);
    return inst;
}

QVariantMap ScriptSys::exec(QStringList parts, const QVariantMap& opts) const
{
    QProcess p;
    p.setProgram(parts.takeFirst());
    p.setArguments(parts);

    if(opts.contains("env"))
    {
        const QVariantMap& envMap = opts["env"].toMap();
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        auto i = envMap.constBegin();
        while (i != envMap.constEnd()) {
            env.insert(i.key(), i.value().toString());
            ++i;
        }
        p.setProcessEnvironment(env);
    }

    QVariantMap result;
    if(opts["detached"].toBool())
    {
        if(p.startDetached())
        {
            result["started"] = true;
        }
        else
        {
            result["started"] = false;
            result["failedAt"] = FAILED_AT_START;
        }
        return result;
    }

    bool ok;
    qint64 timeout = opts["timeout"].toUInt(&ok);
    if(!ok)
        timeout = DEFAULT_TIMEOUT;
    timeout = timeout * 1000;

    const QString& _stdin = opts["stdin"].toString();
    QIODevice::OpenMode mode = _stdin.isEmpty()
                               ? QIODevice::ReadOnly
                               : QIODevice::ReadWrite;

    p.start(mode);

    CountdownTimer t(timeout);
    if(!p.waitForStarted(t.msecsLeft()))
    {
        result["failedAt"] = FAILED_AT_START;
        result["crashed"] = p.exitStatus() == QProcess::CrashExit;
        result["started"] = false;
        return result;
    }

    result["started"] = true;

    if(!_stdin.isEmpty())
    {
        p.write(_stdin.toUtf8());
        if(!p.waitForBytesWritten(t.msecsLeft()))
        {
            result["crashed"] = p.exitStatus() == QProcess::CrashExit;
            result["failedAt"] = FAILED_AT_WRITE;
            p.terminate();
            return result;
        }
        p.closeWriteChannel();
    }

    if(p.state() == QProcess::NotRunning)
    {
        result["crashed"] = p.exitStatus() == QProcess::CrashExit;
    }
    else
    {
        if(!p.waitForFinished(t.msecsLeft()))
        {
            result["failedAt"] = FAILED_AT_FINISH;
            result["crashed"] = p.exitStatus() == QProcess::CrashExit;
            p.terminate();
        }
    }

    result["stdout"] = QString::fromUtf8(p.readAllStandardOutput());
    result["stderr"] = QString::fromUtf8(p.readAllStandardError());
    result["exitCode"] = p.exitCode();

    return result;
}
