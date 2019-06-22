/****************************************************************************}
{ gpudeviceplugin.cpp - bridge between a device plugin and the application   }
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

#include "gpudeviceplugin.h"
#include "appsettings.h"
#include "coreapp.h"
#include <QApplication>
#include <QDir>
#include <QRegularExpression>

static constexpr const char* SETTINGS_GROUP = "plugins";

GpuDevicePlugin::GpuDevicePlugin(const QString &name) :
    cat(name.split('-').first())
{
    QString libFilename = name+"/"+qApp->applicationName().replace("-", "")+"-"+name;
    QStringList errs;
    for(auto& rootDir : rootDirs())
    {
        QString fullFilename = rootDir+"/"+libFilename;
        qDebug() << "Searching for" << libFilename << "in" << rootDir;
        lib.setFileNameAndVersion(fullFilename, qCoreApp->majorVersion());
        if(lib.load())
        {
            qDebug() << "Loaded:" << lib.fileName();
            break;
        }
        errs.append(lib.errorString());
    }
    CHECKE(lib.isLoaded(), Err::libNotFound, errs.join("\n"));

    const GpuFanMeister_GetPlugin getPlugin =
        reinterpret_cast<GpuFanMeister_GetPlugin>(
            lib.resolve(GPUFANMEISTER_GETPLUGIN_FUNC_NAME));
    if(!getPlugin)
    {
        QString err = lib.errorString();
        lib.unload();
        SETERROR_E(Err::funcNotFound, QString(GPUFANMEISTER_GETPLUGIN_FUNC_NAME)+": "+err);
    }

    GpuFanMeister_AppInfo appInfo = {
        sizeof(GpuFanMeister_AppInfo),
        {
            qCoreApp->majorVersion(),
            qCoreApp->minorVersion(),
            qCoreApp->patchVersion()
        }
    };

    // Read all data that can be understood.
    // All other data will be zeroes.
    const GpuFanMeister_Plugin* pluginPtr = getPlugin(&appInfo);
    memset(&plugin, 0, sizeof(plugin));
    memcpy(&plugin, pluginPtr, std::min(sizeof(plugin), pluginPtr->structSize));

    plugin.clearLastError();
    plugin.init();
    if(plugin.getLastError()->code)
    {
        lib.unload();
        SETERROR_E(Err::init, plugin.getLastError()->text);
    }

    if(plugin.customFunctions)
    {
        for(unsigned int a=0; a<plugin.nCustomFunctions; a++)
        {
            QString name = plugin.customFunctions[a].name;
            customFunctionNames.append(name);
            customFunctionPtrs[name] = plugin.customFunctions[a].func;
        }
    }

    if(plugin.buildTimestamp)
        buildDate_ = QDateTime::fromSecsSinceEpoch(plugin.buildTimestamp);
}

GpuDevicePlugin::~GpuDevicePlugin()
{
    lib.unload();
}

size_t GpuDevicePlugin::deviceCount() const
{
    plugin.clearLastError();
    size_t nDevices = plugin.getDeviceCount();
    const GpuFanMeister_Error* err = plugin.getLastError();
    if(err->code)
        SETERROR_E(Err::deviceCount, plugin.getLastError()->text);
    return nDevices;
}

QString GpuDevicePlugin::version() const
{
    return  QString::number(majorVersion())+"."
           +QString::number(minorVersion())+"."
            +QString::number(patchVersion());
}

QStringList GpuDevicePlugin::availableNames()
{
    auto list = availableList();
    QStringList names;
    for(auto& item : qAsConst(list))
        names.append(item.name);
    return names;
}

const QStringList &GpuDevicePlugin::rootDirs()
{
    static bool isFilled = false;
    static QStringList dirs;
    if(!isFilled)
    {
        for(const QString& dir : qAsConst(CoreApp::sysDataDirs()))
            dirs.append(dir+"/plugins");
        dirs.append(qApp->applicationDirPath()+"/plugins");
        isFilled = true;
    }
    return dirs;
}

GpuDevicePluginEntries GpuDevicePlugin::availableList()
{
    GpuDevicePluginEntries entries;
    QStringList names;
    for(const QString& rootDir : rootDirs())
    {
        QDir root(rootDir);
        QStringList dirs = root.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);
        QStringList curDirs = dirs.filter(QRegularExpression("^[a-z]((-[a-z])?[a-z0-9])+$"));
        for(const QString& dir : qAsConst(curDirs))
        {
            if(!names.contains(dir))
            {
                names.append(dir);
                entries.append({dir, rootDir+"/"+dir});
            }
        }
    }
    return entries;
}

GpuDevicePluginEntries GpuDevicePlugin::enabledList()
{
    AppSettings conf;
    conf.beginGroup(SETTINGS_GROUP);
    auto enabled = conf.getString("enabled").split(",", QString::SkipEmptyParts);
    return filterAvailable(enabled);
}

void GpuDevicePlugin::setEnabledList(const QStringList &list)
{
    auto enabled = filterAvailable(list);
    QStringList newList;
    for(auto& entry : qAsConst(enabled))
        newList.append(entry.name);
    QString newListStr = newList.join(",");
    AppSettings conf;
    conf.beginGroup(SETTINGS_GROUP);
    conf.setValue("enabled", newListStr);
}

QString GpuDevicePlugin::errorCodeToString(GpuDevicePlugin::Err errorCode)
{
    switch(errorCode)
    {
        case Err::libNotFound: return "Library not found";
        case Err::funcNotFound: return "Function not found";
        case Err::init: return "Cannot init";
        case Err::deviceCount: return "Cannot get device count";
    }
    return QString();
}

GpuDevicePluginEntries GpuDevicePlugin::filterAvailable(const QStringList &names)
{
    GpuDevicePluginEntries validEnabled;
    auto available = availableList();
    QStringList appended;
    for(const auto& name : qAsConst(names))
    {
        if(!appended.contains(name))
        {
            for(auto& entry : qAsConst(available))
            {
                if(entry.name == name)
                {
                    validEnabled.append(entry);
                    appended.append(name);
                    break;
                }
            }
        }
    }
    return validEnabled;
}

GpuDevicePluginEntries GpuDevicePlugin::filterAvailable(const GpuDevicePluginEntries &list)
{
    GpuDevicePluginEntries validEnabled;
    auto available = availableNames();
    QStringList appended;
    for(const auto& item : qAsConst(list))
    {
        if(available.contains(item.name) && !appended.contains(item.name))
        {
            validEnabled.append(item);
            appended.append(item.name);
        }
    }
    return validEnabled;
}

QStringList GpuDevicePluginEntries::names() const
{
    QStringList list;
    for(auto& item : qAsConst(*this))
        list.append(item.name);
    return list;
}
