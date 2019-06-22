/****************************************************************************}
{ gpudevicelist.h - GPU device list                                          }
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

#include "gpudevice.h"
#include <QtQml/qqml.h>

class GpuDeviceList : public QObject {
    Q_OBJECT

public:
    enum class Err {
        timer = 1
    };
    Q_ENUM(Err)

    explicit GpuDeviceList(QObject* parent = nullptr);
    explicit GpuDeviceList(const QStringList& plugins, QObject* parent = nullptr);
    ~GpuDeviceList();

    Q_PROPERTY(QList<QObject*> devices READ qmlList NOTIFY onListChanged)
    signals: void onListChanged();

    SIMPLE_QML_PROP(unsigned int, trayUpdateSecs)
    signals: void on_trayUpdateSecs_changed();

public:
    const QList<GpuDevice*>* devices() const {return &devices_;}
    Q_INVOKABLE void saveDeviceModeSettings() const;
    Q_INVOKABLE void loadDeviceModeSettings();

    Q_INVOKABLE void loadSettings();
    Q_INVOKABLE void saveSettings();

    static void registerInQml() {
        qmlRegisterUncreatableType<GpuDeviceList>("net.alkatrazstudio.gpufanmeister", 1, 0, "GpuDeviceList", "");
    }

protected:
    const QList<QObject*>& qmlList() { // QML only supports QList<QObject*>
        return *reinterpret_cast<const QList<QObject*>*>(&devices_);}

    QList<GpuDevice*> devices_;
    int timerId_ {};

    void timerEvent(QTimerEvent* event) override;

    static QString errorCodeToString(Err errorCode);

signals:
    void onQuitClick();
    void onShowAppClick(GpuDevice* dev);
    void onShowDevOptionsClick(GpuDevice* dev);
    void onShowManualClick(const QString& uri, GpuDevice* dev);
};
