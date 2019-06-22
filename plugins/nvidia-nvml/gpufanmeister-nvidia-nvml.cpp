/****************************************************************************}
{ gpufanmeister-test-manual.cpp - NVIDIA NVML plugin                         }
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
#include <nvml.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <stdexcept>
#include <array>
#include <memory>
#include <regex>


//
// TYPES & GLOBALS
//

enum {
    errOk = 0,
    errVersion,
    errNoDevices,
    errExec,
    errRead,
    errParse,
    errSet,
    errGet,
    errJson,
    errNVMLBase = 1000
};

static constexpr const char* SETTINGS_TYPE_FAN = "fan";
static constexpr const char* SETTINGS_TYPE_GPU = "gpu";
static constexpr const char* SETTINGS_NAME_FAN_SPEED = "GPUTargetFanSpeed";
static constexpr const char* SETTINGS_NAME_CONTROL_STATE = "GPUFanControlState";

#define NV_CHECK(x, failResult) {   \
    nvmlReturn_t result = (x);      \
    if(result != NVML_SUCCESS){     \
        setLastError(               \
            errNVMLBase + result,   \
            nvmlErrorString(result) \
        );                          \
        return failResult;          \
    }                               \
}
#define NV_CHECKI(x) NV_CHECK(x, 0)
#define NV_CHECKP(x) NV_CHECK(x, nullptr)
#define NV_CHECKV(x) NV_CHECK(x,)
#define NV_CHECKB(x) NV_CHECK(x, false)

static GpuFanMeister_Error lastError;
static bool inited = false;



//
// HELPERS
//

static bool setLastError(int code, const std::string& text)
{
    // error text string must persist between calls
    static std::string lastErrText;
    lastErrText = text;
    lastError.code = code;
    lastError.text = lastErrText.data();
    return false;
}

static const char* jsonToString(const nlohmann::json& json)
{
    // resulting string must persist between calls
    static std::string str;
    try{
        str = json.dump();
    }catch(const std::exception& e){
        setLastError(errJson, e.what());
        return nullptr;
    }catch(...){
        setLastError(errJson, "");
        return nullptr;
    }
    return str.data();
}

static std::string exec(const std::string& cmd)
{
    std::array<char, BUFSIZ> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd.data(), "r"), pclose);

    if(!pipe)
    {
        result = "Cannot start: ";
        result.append(cmd);
        setLastError(errExec, result);
        return "";
    }

    try{
        while(!feof(pipe.get()))
        {
            if(fgets(buffer.data(), buffer.size(), pipe.get()))
                result += buffer.data();
            else
                break;
        }
    }catch(...){
        result = "Cannot read: ";
        result.append(cmd);
        setLastError(errRead, result);
    }

    return result;
}

static bool setSetting(const char* targetType, size_t targetIndex, const char* name, unsigned int val)
{
    std::string cmd = std::string("nvidia-settings -a [")+targetType+":"+std::to_string(targetIndex)+"]/"+name+"="+std::to_string(val);
    std::string out = exec(cmd);
    if(out.empty())
        return false;
    std::string rxStr = std::string("Attribute '")+name+"' .* assigned value (\\-?\\d+)";
    std::regex rx(rxStr);
    std::smatch m;
    if(!std::regex_search(out, m, rx))
        return setLastError(errParse, std::string("Cannot parse: ")+cmd+": "+out);
    if(val != std::stoul(m[1]))
        return setLastError(errSet, std::string("Cannot set: ")+cmd+": "+out);
    return true;
}

static int getSetting(const char* targetType, size_t targetIndex, const char* name)
{
    std::string cmd = std::string("nvidia-settings -t -q [")+targetType+":"+std::to_string(targetIndex)+"]/"+name;
    std::string out = exec(cmd);
    if(out.empty())
        return setLastError(errGet, std::string("Cannot get: ")+cmd+": "+out);
    int val = std::stoi(out);
    if(val < 0)
        return setLastError(errGet, std::string("Cannot get: ")+cmd+": "+out);
    return val;
}



//
// DEVICE CLASS
//

class Device {
public:
    explicit Device(size_t index) :
        index(index),
        dev(nullptr),
        fanManualModeDirty(true)
    {
    }

    void init()
    {
        NV_CHECKV(nvmlDeviceGetHandleByIndex(static_cast<unsigned int>(index), &dev));
    }

    const char* getId()
    {
        NV_CHECKP(nvmlDeviceGetUUID(dev, id, NVML_DEVICE_UUID_BUFFER_SIZE));
        return id;
    }

    const char* getName()
    {
        NV_CHECKP(nvmlDeviceGetName(dev, name, NVML_DEVICE_NAME_BUFFER_SIZE));
        return name;
    }

    void deinit()
    {
    }

    unsigned int getTemperature()
    {
        unsigned int temp;
        NV_CHECKI(nvmlDeviceGetTemperature(dev, NVML_TEMPERATURE_GPU, &temp));
        return temp;
    }

    unsigned int getFanSpeed()
    {
        unsigned int fanSpeed;
        NV_CHECKI(nvmlDeviceGetFanSpeed(dev, &fanSpeed));
        return fanSpeed;
    }

    void setFanSpeed(unsigned int value)
    {
        setSetting(SETTINGS_TYPE_FAN, index, SETTINGS_NAME_FAN_SPEED, value);
    }

    bool isFanManualMode()
    {
        if(fanManualModeDirty)
        {
            fanManualMode = getSetting(SETTINGS_TYPE_GPU, index, SETTINGS_NAME_CONTROL_STATE);
            fanManualModeDirty = false;
        }
        return fanManualMode;
    }

    void setFanManualMode(bool value)
    {
        setSetting(SETTINGS_TYPE_GPU, index, SETTINGS_NAME_CONTROL_STATE, value ? 1 : 0);
        fanManualModeDirty = true;
    }

    unsigned int getGpuRate()
    {
        nvmlUtilization_t util;
        NV_CHECKI(nvmlDeviceGetUtilizationRates(dev, &util));
        return util.gpu;
    }

    unsigned int getMemoryRate()
    {
        nvmlUtilization_t util;
        NV_CHECKI(nvmlDeviceGetUtilizationRates(dev, &util));
        return util.memory;
    }

    unsigned long long getTotalMemory()
    {
        nvmlMemory_t mem;
        NV_CHECKI(nvmlDeviceGetMemoryInfo(dev, &mem));
        return mem.total;
    }

    unsigned long long getFreeMemory()
    {
        nvmlMemory_t mem;
        NV_CHECKI(nvmlDeviceGetMemoryInfo(dev, &mem));
        return mem.free;
    }

    bool getThrottleReasons(nlohmann::json& json)
    {
        unsigned long long bits;
        NV_CHECKB(nvmlDeviceGetCurrentClocksThrottleReasons(dev, &bits));

        json.emplace("gpuIdle", bits & nvmlClocksThrottleReasonGpuIdle);
        json.emplace("applicationsClockSetting", bits & nvmlClocksThrottleReasonApplicationsClocksSetting);
        json.emplace("swPowerCap", bits & nvmlClocksThrottleReasonSwPowerCap);
        json.emplace("hwSlowdown", bits & nvmlClocksThrottleReasonHwSlowdown);
        json.emplace("syncBoost", bits & nvmlClocksThrottleReasonSyncBoost);
        json.emplace("swThermalSlowdown", bits & nvmlClocksThrottleReasonSwThermalSlowdown);
        json.emplace("hwThermalSlowdown", bits & nvmlClocksThrottleReasonHwThermalSlowdown);
        json.emplace("hwPowerBrakeSlowdown", bits & nvmlClocksThrottleReasonHwPowerBrakeSlowdown);

        unsigned long long allBits = 0 \
            | nvmlClocksThrottleReasonGpuIdle \
            | nvmlClocksThrottleReasonApplicationsClocksSetting \
            | nvmlClocksThrottleReasonSwPowerCap \
            | nvmlClocksThrottleReasonHwSlowdown \
            | nvmlClocksThrottleReasonSyncBoost \
            | nvmlClocksThrottleReasonSwThermalSlowdown \
            | nvmlClocksThrottleReasonHwThermalSlowdown \
            | nvmlClocksThrottleReasonHwPowerBrakeSlowdown;
        json.emplace("unknown", bits & ~allBits);
        json.emplace("code", bits);

        return true;
    }

    static std::vector<Device>& devs()
    {
        static std::vector<Device> devs;
        return devs;
    }

protected:
    size_t index;
    nvmlDevice_t dev;
    char id[NVML_DEVICE_UUID_BUFFER_SIZE];
    char name[NVML_DEVICE_NAME_BUFFER_SIZE];
    bool fanManualMode;
    bool fanManualModeDirty;
};
static std::vector<Device>& devs = Device::devs();



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
    NV_CHECKV(nvmlInit());
    inited = true;
    unsigned int nDevices;
    NV_CHECKV(nvmlDeviceGetCount(&nDevices));

    devs.reserve(nDevices);
    for(unsigned int i=0; i<nDevices; i++)
        devs.emplace_back(i);
}

static void deinit()
{
    devs.clear();
    if(inited)
        NV_CHECKV(nvmlShutdown());
}

static const char* getId(size_t deviceIndex)
{
    return devs[deviceIndex].getId();
}

static const char* getName(size_t deviceIndex)
{
    return devs[deviceIndex].getName();
}

static size_t getDeviceCount()
{
    return devs.size();
}

static void initDevice(size_t deviceIndex)
{
    devs[deviceIndex].init();
}

static void deinitDevice(size_t deviceIndex)
{
    devs[deviceIndex].deinit();
}

static unsigned int getTemperature(size_t deviceIndex)
{
    return devs[deviceIndex].getTemperature();
}

static unsigned int getFanSpeed(size_t deviceIndex)
{
    return devs[deviceIndex].getFanSpeed();
}

static void setFanSpeed(size_t deviceIndex, unsigned int value)
{
    devs[deviceIndex].setFanSpeed(value);
}

static bool isFanManualMode(size_t deviceIndex)
{
    return devs[deviceIndex].isFanManualMode();
}

static void setFanManualMode(size_t deviceIndex, bool value)
{
    devs[deviceIndex].setFanManualMode(value);
}

static unsigned int getGpuRate(size_t deviceIndex)
{
    return devs[deviceIndex].getGpuRate();
}

static unsigned int getMemoryRate(size_t deviceIndex)
{
    return devs[deviceIndex].getMemoryRate();
}

static unsigned long long getTotalMemory(size_t deviceIndex)
{
    return devs[deviceIndex].getTotalMemory();
}

static unsigned long long getFreeMemory(size_t deviceIndex)
{
    return devs[deviceIndex].getFreeMemory();
}

static const char* getThrottleReasons(size_t deviceIndex, const char* json)
{
    (void)json;
    nlohmann::json obj;
    if(!devs[deviceIndex].getThrottleReasons(obj))
        return nullptr;
    return jsonToString(obj);
}



//
// ENTRY POINT
//

const GpuFanMeister_Plugin* gpuFanMeister_getPlugin(const GpuFanMeister_AppInfo* appInfo)
{
    // Get all data that was explicitly set by the app and can be read by this plugin.
    // All other data is set to zeroes.
    GpuFanMeister_AppInfo info;
    memset(&info, 0, sizeof(info));
    memcpy(&info, appInfo, std::min(sizeof(info), appInfo->structSize));

    static GpuFanMeister_CustomFunction customFunctions[] = {{
        "getThrottleReasons",
        &getThrottleReasons
    }};

    static GpuFanMeister_Plugin plugin = {
        sizeof(GpuFanMeister_Plugin),
        {VER_MAJ, VER_MIN, VER_PAT},
        BUILD_TS,
        "nvidia-nvml",
        "NVIDIA (NVML)",
        "NVIDIA plugin that uses NVML (https://developer.nvidia.com/nvidia-management-library-nvml).",
        "Alkatraz Studio, 2018",
        "Project website: https://gpufanmeister.alkatrazstudio.net\n"
        "Author's website: https://alkatrazstudio.net",
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
        &getGpuRate,
        &getMemoryRate,
        &getTotalMemory,
        &getFreeMemory,
        sizeof(customFunctions) / sizeof(customFunctions[0]),
        customFunctions
    };

    if(info.version.major != plugin.version.major)
    {
        setLastError(errVersion, "Incompatible versions.");
        return nullptr;
    }

    return &plugin;
}
