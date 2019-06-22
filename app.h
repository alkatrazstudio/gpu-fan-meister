/****************************************************************************}
{ app.h - main application                                                   }
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

#include "coreapp.h"
#include "gpudevicelist.h"
#include <QQmlApplicationEngine>

class App : public CoreApp
{
    Q_OBJECT

public:
    enum class Err {
        noPlugins = 1,
        noDevicesFound,
        noDevicesLeft,
        noDoc,
        noTray
    };
    Q_ENUM(Err)

    App(int& argc, char** argv);

    Q_PROPERTY(GpuDeviceList* devList MEMBER devList CONSTANT)
    Q_PROPERTY(QStringList enabledPlugins READ enabledPluginNames WRITE setEnabledPlugins NOTIFY onEnabledPluginsChange)
    Q_PROPERTY(QStringList availablePlugins READ availablePluginNames CONSTANT)

public:
    Q_INVOKABLE void openManual(const QString& uri = QString());
    Q_INVOKABLE void setMainScriptObject(QObject* obj){mainScriptObject = obj;}

protected:
    QQmlApplicationEngine *getEngine();
    void showDevTab(GpuDevice *dev);
    void showDevOptions(GpuDevice *dev);

    GpuDevicePluginEntries availablePlugins(){return GpuDevicePlugin::availableList();}
    GpuDevicePluginEntries enabledPlugins(){return GpuDevicePlugin::enabledList();}

    QStringList availablePluginNames(){return availablePlugins().names();}
    QStringList enabledPluginNames(){return enabledPlugins().names();}

    void setEnabledPlugins(const QStringList& list);

    static QString errorCodeToString(Err code);

    static void registerInQml() {
        qmlRegisterUncreatableType<App>("net.alkatrazstudio.gpufanmeister", 1, 0, "App", "");
    }

    static QString getManualRootDir();

    void openEnabledPluginsForm();

    int main() override;
    void onQuit() override;

    QObject* mainScriptObject {};
    GpuDeviceList* devList {};
    QQmlApplicationEngine* engine {};
    QMutex mutex;
    bool devsFound {};

    void onError(const ErrorManager::ErrorStruct &err) override;

signals:
    void onShow();
    void onNeedShowDevTab(GpuDevice* dev);
    void onNeedShowDevOptions(GpuDevice* dev);
    void onEnabledPluginsChange();
};
