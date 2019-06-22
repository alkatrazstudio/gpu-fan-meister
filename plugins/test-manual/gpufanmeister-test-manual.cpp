/****************************************************************************}
{ gpufanmeister-test-manual.cpp - test plugin                                }
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

#include "gpufanmeister/plugin.h"
#include "common_defines.h"
#include <QSlider>
#include <QScreen>
#include <QGuiApplication>
#include <QFontMetrics>
#include <string>
#include <vector>

//
// TYPES & GLOBALS
//

enum {
    errOk = 0,
    errVersion = 1,
};

static GpuFanMeister_Error lastError;



//
// DEVICE STRUCT
//

struct Device {
    std::string id;
    std::string name;
    unsigned int temp = 50;
    unsigned int fanSpeed = 50;
    bool fanManualMode = false;

    QSlider* tempSlider;

    static std::vector<Device> devs;
};
std::vector<Device> Device::devs(NDEVS);
static auto& devs = Device::devs;


//
// HELPERS
//

static bool setLastError(unsigned int code, const std::string& text)
{
    static std::string lastErrText;
    lastErrText = text;
    lastError.code = code;
    lastError.text = lastErrText.data();
    return false;
}



//
// INTERFACE FUNCTIONS
//

static void clearLastError()
{
    setLastError(errOk, "");
}

static const GpuFanMeister_Error* getLastError()
{
    return &lastError;
}

static void init()
{
}

static void deinit()
{
}

static size_t getDeviceCount()
{
    return NDEVS;
}

static void initDevice(size_t deviceIndex)
{
    devs[deviceIndex].id = std::to_string(deviceIndex);
    devs[deviceIndex].name = "Device "+devs[deviceIndex].id;

    QSlider* slider = new QSlider;
    devs[deviceIndex].tempSlider = slider;

    QPoint c = QGuiApplication::primaryScreen()->geometry().center();
    int w = slider->fontMetrics().width("x") * 10;
    c.setY(c.y() + deviceIndex * 1.25 * w);
    int h = w * 2;
    slider->move(c);
    slider->resize(w, h);
    slider->setRange(0, 100);
    slider->setSingleStep(1);
    slider->show();
    slider->setValue(devs[deviceIndex].temp);
    QObject::connect(slider, &QSlider::valueChanged, [deviceIndex](int val){
        devs[deviceIndex].temp = val;
    });
}

static void deinitDevice(size_t deviceIndex)
{
    devs[deviceIndex].tempSlider->deleteLater();
}

static const char* getId(size_t deviceIndex)
{
    return devs[deviceIndex].id.data();
}

static const char* getName(size_t deviceIndex)
{
    return devs[deviceIndex].name.data();
}

static unsigned int getTemperature(size_t deviceIndex)
{
    return devs[deviceIndex].temp;
}

static unsigned int getFanSpeed(size_t deviceIndex)
{
    return devs[deviceIndex].fanSpeed;
}

static void setFanSpeed(size_t deviceIndex, unsigned int value)
{
    devs[deviceIndex].fanSpeed = value;
}

static bool isFanManualMode(size_t deviceIndex)
{
    return devs[deviceIndex].fanSpeed;
}

static void setFanManualMode(size_t deviceIndex, bool value)
{
    devs[deviceIndex].fanSpeed = value;
}

const GpuFanMeister_Plugin* gpuFanMeister_getPlugin(const GpuFanMeister_AppInfo* appInfo)
{
    // Get all data that was explicitly set by the app and can be read by this plugin.
    // All other data is set to zeroes.
    GpuFanMeister_AppInfo info;
    memset(&info, 0, sizeof(info));
    memcpy(&info, appInfo, std::min(sizeof(info), appInfo->structSize));

    static GpuFanMeister_Plugin plugin = {
        sizeof(GpuFanMeister_Plugin),
        {VER_MAJ, VER_MIN, VER_PAT},
        BUILD_TS,
        "test",
        "Test",
        "Test plugin.",
        "Alkatraz Studio, 2018",
        "",
        &clearLastError,
        &getLastError,
        &init,
        &deinit,
        &getDeviceCount,
        &initDevice,
        &deinitDevice,
        &getId,
        &getName,
        &getTemperature,
        &getFanSpeed,
        &setFanSpeed,
        &isFanManualMode,
        &setFanManualMode,
        nullptr, // getGpuRate
        nullptr, // getMemoryRate
        nullptr, // getTotalMemory
        nullptr, // getFreeMemory
        0,       // nCustomFunctions
        nullptr  // customFunctions
    };

    if(info.version.major != plugin.version.major)
    {
        setLastError(errVersion, "Incompatible versions.");
        return nullptr;
    }

    return &plugin;
}
