/****************************************************************************}
{ gpudeviceinterface.cpp - interface to a single GPU device from a plugin    }
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

#include "gpudeviceinterface.h"
#include <QJSEngine>

GpuDeviceInterface::GpuDeviceInterface(GpuDevicePlugin::CPtr plugin, size_t deviceIndex) :
    plugin_(plugin),
    deviceIndex(deviceIndex)
{
    clearLastError();
    plugin_->raw()->initDevice(deviceIndex);
    throwLastError();
}

GpuDeviceInterface::~GpuDeviceInterface()
{
    clearLastError();
    plugin_->raw()->deinitDevice(deviceIndex);
    //throwLastError();
}

QString GpuDeviceInterface::id() const
{
    clearLastError();
    const char* result = plugin_->raw()->getId(deviceIndex);
    throwLastError();
    return result;
}

QString GpuDeviceInterface::name() const
{
    clearLastError();
    const char* result = plugin_->raw()->getName(deviceIndex);
    throwLastError();
    return result;
}

unsigned int GpuDeviceInterface::temperature() const
{
    clearLastError();
    unsigned int temp = plugin_->raw()->getTemperature(deviceIndex);
    throwLastError();
    return temp;
}

unsigned int GpuDeviceInterface::fanSpeed()
{
    clearLastError();
    unsigned int speed = plugin_->raw()->getFanSpeed(deviceIndex);
    throwLastError();
    return speed;
}

void GpuDeviceInterface::setFanSpeed(unsigned int value)
{
    clearLastError();
    plugin_->raw()->setFanSpeed(deviceIndex, value);
    throwLastError();
}

bool GpuDeviceInterface::isFanManualMode() const
{
    clearLastError();
    bool result = plugin_->raw()->isFanManualMode(deviceIndex);
    throwLastError();
    return result;
}

void GpuDeviceInterface::setFanManualMode(bool value)
{
    clearLastError();
    plugin_->raw()->setFanManualMode(deviceIndex, value);
    throwLastError();
}

unsigned int GpuDeviceInterface::gpuRate() const
{
    if(!plugin_->canGetGpuRate())
        return 0;
    clearLastError();
    unsigned int rate = plugin_->raw()->getGpuRate(deviceIndex);
    throwLastError();
    return rate;
}

unsigned int GpuDeviceInterface::memoryRate() const
{
    if(!plugin_->canGetMemoryRate())
        return 0;
    clearLastError();
    unsigned int rate = plugin_->raw()->getMemoryRate(deviceIndex);
    throwLastError();
    return rate;
}

unsigned long long GpuDeviceInterface::totalMemory() const
{
    if(!plugin_->canGetTotalMemory())
        return 0;
    clearLastError();
    unsigned long long mem = plugin_->raw()->getTotalMemory(deviceIndex);
    throwLastError();
    return mem;
}

unsigned long long GpuDeviceInterface::freeMemory() const
{
    if(!plugin_->canGetFreeMemory())
        return 0;
    clearLastError();
    unsigned long long mem = plugin_->raw()->getFreeMemory(deviceIndex);
    throwLastError();
    return mem;
}

QJSValue GpuDeviceInterface::customFunction(const QString &name, const QJSValue &val, QJSEngineEx *engine) const
{
    clearLastError();
    auto f = plugin_->customFunction(name);
    if(!f)
    {
        engine->throwError(QStringLiteral("Custom function not found: ") + name);
        return QJSValue();
    }

    bool ok;
    QJSValue tmpVal = engine->jsonStringify(val, ok);
    if(!ok)
        return tmpVal;

    QByteArray json = tmpVal.toString().toUtf8();
    const char* result = f(deviceIndex, json.constData());
    throwLastError();

    if(!result)
        return QJSValue();

    return engine->jsonParse(result);
}

void GpuDeviceInterface::clearLastError() const
{
    plugin_->raw()->clearLastError();
}

void GpuDeviceInterface::throwLastError() const
{
    const GpuFanMeister_Error* err = plugin_->raw()->getLastError();
    if(err->code)
        throw Error(err, this);
}

GpuDeviceInterface::Error::Error(const GpuFanMeister_Error* err, const GpuDeviceInterface *parent) :
    std::runtime_error(""),
    text_(err->text),
    code_(err->code),
    devId(parent ? parent->id() : ""),
    devName(parent ? parent->name() : "")
{
}

GpuDeviceInterface::Error::Error(const GpuFanMeister_Error &err, const GpuDeviceInterface *parent) :
    std::runtime_error(""),
    text_(err.text),
    code_(err.code),
    devId(parent ? parent->id() : ""),
    devName(parent ? parent->name() : "")
{
}

const char *GpuDeviceInterface::Error::what() const noexcept
{
    static QByteArray errChars;
    if(errChars.isEmpty())
    {
        QString errText;
        if(!devId.isEmpty() || !devName.isEmpty())
        {
            if(!devId.isEmpty())
            {
                errText.append(devId);
                errText.append(" - ");
            }
            if(!devName.isEmpty())
            {
                errText.append(devName);
                errText.append(" - ");
            }
        }
        else
        {
            errText.append("No device - ");
        }

        errText.append("error: ");
        errText.append(QString::number(code_));
        errText.append(" - ");
        errText.append(text_);
        errChars = errText.toUtf8();
    }
    return errChars.constData();
}
