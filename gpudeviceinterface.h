/****************************************************************************}
{ gpudeviceinterface.h - interface to a single GPU device from a plugin      }
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

#include "gpudeviceplugin.h"
#include "qjsengineex.h"

class GpuDeviceInterface {
private:
    Q_DISABLE_COPY(GpuDeviceInterface)

public:
    using Ptr = std::shared_ptr<GpuDeviceInterface> ;
    using CPtr =std::shared_ptr<const GpuDeviceInterface>;

    class Error : public std::runtime_error {
    private:
        Q_DISABLE_COPY(Error)

    public:
        explicit Error(const GpuFanMeister_Error* err, const GpuDeviceInterface* parent);
        explicit Error(const GpuFanMeister_Error& err, const GpuDeviceInterface* parent);
        const QString& text() const {return text_;}
        int code() const {return code_;}

        const char* what() const noexcept override;

    protected:
        QString getMsg(const GpuFanMeister_Error* err, const GpuDeviceInterface* parent) const;
        QString text_;
        int code_;
        QString devId;
        QString devName;
    };

    GpuDeviceInterface(GpuDevicePlugin::CPtr plugin, size_t deviceIndex);
    ~GpuDeviceInterface();

    const GpuDevicePlugin::CPtr plugin() const {return plugin_;}
    QString id() const;
    QString name() const;
    unsigned int temperature() const;
    unsigned int fanSpeed();
    void setFanSpeed(unsigned int value);
    bool isFanManualMode() const;
    void setFanManualMode(bool value);
    unsigned int gpuRate() const;
    unsigned int memoryRate() const;
    unsigned long long totalMemory() const;
    unsigned long long freeMemory() const;
    QJSValue customFunction(const QString& name, const QJSValue& val, QJSEngineEx* engine) const;

protected:
    void clearLastError() const;
    void throwLastError() const;

    const GpuDevicePlugin::CPtr plugin_;
    const size_t deviceIndex;
};
