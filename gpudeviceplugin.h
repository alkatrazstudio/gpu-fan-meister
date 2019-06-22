/****************************************************************************}
{ gpudeviceplugin.h - bridge between a device plugin and the application     }
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

#pragma once

#include "gpufanmeister/plugin.h"
#include "errormanager.h"
#include <QLibrary>
#include <QtQml/qqml.h>
#include <memory>

struct GpuDevicePluginEntry {
    QString name;
    QString dir;
};

class GpuDevicePluginEntries : public QList<GpuDevicePluginEntry> {
public:
    QStringList names() const;
};

class GpuDevicePlugin: public QObject {
    Q_OBJECT

public:
    enum class Err {
        libNotFound,
        funcNotFound,
        init,
        deviceCount
    };
    Q_ENUM(Err)

    using Ptr = std::shared_ptr<GpuDevicePlugin>;
    using CPtr = std::shared_ptr<const GpuDevicePlugin>;
    using Error = std::runtime_error;

    explicit GpuDevicePlugin(const QString& name);
    ~GpuDevicePlugin();

    Q_PROPERTY(int deviceCount READ deviceCount CONSTANT)
    Q_PROPERTY(QString category READ category CONSTANT)
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString copyright READ copyright CONSTANT)
    Q_PROPERTY(QString misc READ misc CONSTANT)
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(unsigned int majorVersion READ majorVersion CONSTANT)
    Q_PROPERTY(unsigned int minorVersion READ minorVersion CONSTANT)
    Q_PROPERTY(unsigned int patchVersion READ patchVersion CONSTANT)
    Q_PROPERTY(QDateTime buildDate READ buildDate CONSTANT)
    Q_PROPERTY(bool canGetGpuRate READ canGetGpuRate CONSTANT)
    Q_PROPERTY(bool canGetMemoryRate READ canGetMemoryRate CONSTANT)
    Q_PROPERTY(bool canGetTotalMemory READ canGetTotalMemory CONSTANT)
    Q_PROPERTY(bool canGetFreeMemory READ canGetFreeMemory CONSTANT)
    Q_PROPERTY(QStringList customFunctions READ customFunctions CONSTANT)

    size_t deviceCount() const;
    const QString& category() const {return cat;}
    QString id() const {return plugin.id;}
    QString name() const {return plugin.name;}
    QString description() const {return plugin.description;}
    QString copyright() const {return plugin.copyright;}
    QString misc() const {return plugin.misc;}
    QString version() const;
    unsigned int majorVersion() const {return plugin.version.major;}
    unsigned int minorVersion() const {return plugin.version.minor;}
    unsigned int patchVersion() const {return plugin.version.patch;}
    const QDateTime& buildDate() const {return buildDate_;}
    const QStringList &customFunctions() const {return customFunctionNames;}
    GpuFanMeister_CustomFunctionPtr customFunction(const QString& name) const
        {return customFunctionPtrs.value(name, nullptr);}

    bool canGetGpuRate() const {return plugin.getGpuRate;}
    bool canGetMemoryRate() const {return plugin.getMemoryRate;}
    bool canGetTotalMemory() const {return plugin.getTotalMemory;}
    bool canGetFreeMemory() const {return plugin.getFreeMemory;}

    const GpuFanMeister_Plugin* raw() const {return &plugin;}

    static void registerInQml() {
        qmlRegisterUncreatableType<GpuDevicePlugin>("net.alkatrazstudio.gpufanmeister", 1, 0, "GpuDevicePlugin", "");
    }

    static QStringList availableNames();
    static GpuDevicePluginEntries availableList();
    static GpuDevicePluginEntries enabledList();
    static void setEnabledList(const QStringList& list);

    static QString errorCodeToString(Err errorCode);

protected:
    static const QStringList& rootDirs();
    static GpuDevicePluginEntries filterAvailable(const QStringList& names);
    static GpuDevicePluginEntries filterAvailable(const GpuDevicePluginEntries& list);

    QLibrary lib;
    GpuFanMeister_Plugin plugin {};
    const QString cat;
    QStringList customFunctionNames;
    QHash<QString, GpuFanMeister_CustomFunctionPtr> customFunctionPtrs;
    QDateTime buildDate_;
};
