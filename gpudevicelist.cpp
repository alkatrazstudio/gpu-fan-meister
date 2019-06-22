/****************************************************************************}
{ gpudevicelist.cpp - GPU device list                                        }
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

#include "gpudevicelist.h"
#include "errormanager.h"
#include "appsettings.h"
#include <QDebug>

static const char* GPU_DEVICE_LIST_CONF_GROUP = "GpuDeviceList";
static const int TIMER_UPDATE_INTERVAL = 1000;

GpuDeviceList::GpuDeviceList(QObject* parent) :
    QObject(parent)
{
}

GpuDeviceList::GpuDeviceList(const QStringList &plugins, QObject* parent) :
    QObject(parent)
{
    loadSettings();

    for(const QString& name : plugins)
    {
        try{
            GpuDevicePlugin::Ptr plugin(new GpuDevicePlugin(name));
            size_t nDevices = plugin->deviceCount();
            for(size_t deviceIndex=0; deviceIndex<nDevices; deviceIndex++)
            {
                try{
                    GpuDevice* dev = new GpuDevice(plugin, deviceIndex, this);
                    QObject::connect(dev, &GpuDevice::onShowAppClick, [this, dev]{
                        emit onShowAppClick(dev);
                    });
                    QObject::connect(dev, &GpuDevice::onShowDevOptionsClick, [this, dev]{
                        emit onShowDevOptionsClick(dev);
                    });
                    QObject::connect(dev, &GpuDevice::onShowManualClick, [this, dev](const QString& uri){
                        emit onShowManualClick(uri, dev);
                    });
                    QObject::connect(dev, &GpuDevice::onQuitClick, this, &GpuDeviceList::onQuitClick);
                    devices_.append(dev);
                }catch(const ErrorManager::Exception& e){
                    Q_UNUSED(e);
                }
            }
        }catch(GpuDevicePlugin::Error& e){
            qDebug() << name << ":" << e.what();
        }
    }

    if(devices_.empty())
        return;

    timerId_ = startTimer(TIMER_UPDATE_INTERVAL);
    CHECKE(timerId_, Err::timer);
}

GpuDeviceList::~GpuDeviceList()
{
    if(timerId_)
        killTimer(timerId_);
}

void GpuDeviceList::saveDeviceModeSettings() const
{
    for(GpuDevice* dev : qAsConst(devices_))
        dev->saveModeSettings();
}

void GpuDeviceList::loadDeviceModeSettings()
{
    for(GpuDevice* dev : qAsConst(devices_))
        dev->loadModeSettings();
}

void GpuDeviceList::loadSettings()
{
    AppSettings conf;
    conf.beginGroup(GPU_DEVICE_LIST_CONF_GROUP);
    set_trayUpdateSecs(conf.getUInt("trayUpdateSecs", 10));
    conf.endGroup();
}

void GpuDeviceList::saveSettings()
{
    AppSettings conf;
    conf.beginGroup(GPU_DEVICE_LIST_CONF_GROUP);
    conf.setValue("trayUpdateSecs", trayUpdateSecs_);
    conf.endGroup();
}

void GpuDeviceList::timerEvent(QTimerEvent *event)
{
    if(event->timerId() != timerId_)
        return;

    bool listUpdated = false;
    for(GpuDevice* dev : qAsConst(devices_))
    {
        try{
            dev->update();
        }catch(ErrorManager::Exception&){
            devices_.removeOne(dev);
            dev->deleteLater();
            listUpdated = true;
        }
    }
    if(listUpdated)
        emit onListChanged();
}

QString GpuDeviceList::errorCodeToString(Err errorCode)
{
    switch(errorCode)
    {
        case Err::timer: return "Cannot init timer";
    }
    return QString();
}
