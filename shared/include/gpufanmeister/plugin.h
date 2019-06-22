/****************************************************************************}
{ plugin.h - GPU plugin interface                                            }
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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/// Version information.
typedef struct {
    unsigned int major; ///< major version number: "x" in x.y.z
    unsigned int minor; ///< minor version number: "y" in x.y.z
    unsigned int patch; ///< patch version number: "z" in x.y.z
} GpuFanMeister_Version;

/// Application information.
/// This struct is passed to the plugin by the application.
typedef struct {
    /// Struct size in bytes (for ABI compatibility).
    /// The plugin must read no more than this many bytes from this struct.
    /// All other data must be zeroes.
    size_t structSize;

    /// Application version.
    GpuFanMeister_Version version;
} GpuFanMeister_AppInfo;

/// Error data.
typedef struct {
    /// Error code.
    /// 0 - no error.
    /// Negative error codes are reserved for internal use.
    int code;

    /// Human-readable description of an error.
    /// The pointer must be valid until the call to GpuFanMeister_Plugin::clearLastError.
    const char* text;
} GpuFanMeister_Error;

/// Pointer to a custom function.
typedef const char* (*GpuFanMeister_CustomFunctionPtr)(size_t deviceIndex, const char*);

/// Custom function.
typedef struct {
    /// Function name.
    const char* name;

    /// The function.
    /// It takes a device index and a JSON string as as input
    /// and should return JSON string as a result.
    /// The pointer to the result string must remain valid
    /// until the next call of GpuFanMeister_Plugin::getLastError function.
    /// In case there is no result or there was an error,
    /// the function may return null.
    GpuFanMeister_CustomFunctionPtr func;
} GpuFanMeister_CustomFunction;


/// All plugin data and functions that will be visible to the application.
/// This struct will be passed from the plugin back to the application.
/// Most of the device-level functions (which accept deviceIndex) will be
/// called each second, and it's the plugin's responsibility to maintain
/// any cached values for a speed-up if needed.
typedef struct {
    /// Struct size in bytes (for ABI compatibility).
    /// The application will read no more than this many bytes from this struct.
    /// All other data will be considered zeroes.
    size_t structSize;

    /// Version.
    GpuFanMeister_Version version;

    /// Timestamp of plugin's build/compile date.
    /// Set to zero if the plugin can't provide such info.
    unsigned long long buildTimestamp;

    /// Unique identifier of the plugin.
    /// Must be the same as a plugin's directory name, e.g. "amd-mesa".
    const char* id;

    /// Human readable plugin name, e.g. "AMD (Mesa)".
    const char* name;

    /// Description of a plugin.
    /// Can be multi-line and contain HTTP links and email adresses.
    const char* description;

    /// Copyright information.
    /// Can be multi-line and contain HTTP links and email adresses.
    const char* copyright;

    /// Miscellaneous information, can be anything or empty.
    /// Can be multi-line and contain HTTP links and email adresses.
    const char* misc;

    /// Reset the error state,
    /// so that a next call to getLastError() will return GpuFanMeister_Error::code = 0.
    /// This function will be called before any other function except getLastError().
    /// The plugin must clear the last error only when the aplication calls this function.
    void (*clearLastError)();

    /// Get the last error.
    /// The plugin must not throw any exceptions.
    /// Instead, the last error should be remembered internally
    /// and then returned from this function.
    /// If an error occurs in any device-level functions
    /// then the device will be unloaded.
    /// If an error occurs in any plugin-level functions
    /// then the entire plugin will be unloaded.
    const GpuFanMeister_Error* (*getLastError)();

    /// Plugin initialization.
    /// Will be called once after gpuFanMeister_getPlugin().
    void (*init)();

    /// Plugin deactivation.
    /// Will be called once before the plugin is unloaded from memory.
    /// This function will be called even if getLastError() returns
    /// an error for a previously called function.
    void (*deinit)();

    /// Return the number of supported devices found by this plugin.
    /// All below functions will take deviceIndex between 0 and that number.
    /// After a call to this function all devices must keep
    /// their indexes to the very end.
    size_t (*getDeviceCount)();

    /// Device initialization.
    /// Will be called once for each device
    /// before calling any other function for this device.
    void (*initDevice)(size_t deviceIndex);

    /// Device deactivation.
    /// Will be called when the device is no longer needed.
    /// After a call to this function no other device-level
    /// functions will be called for this device.
    /// All other devices' indexes must remain the same after deactivating a device.
    void (*deinitDevice)(size_t deviceIndex);

    /// Get device ID.
    /// Device ID must be a string that uniquely identifies a device,
    /// e.g. a serial number.
    const char* (*getId)(size_t deviceIndex);

    /// Get human-readable device name.
    /// This name will displayed to end user as a default name for a device.
    /// Needs not to be unique.
    const char* (*getName)(size_t deviceIndex);

    /// Return a current device temperature in degrees Celcium.
    /// The returned value will be capped at 99 by the application.
    unsigned int (*getTemperature)(size_t deviceIndex);

    /// Return a current device fan speed in percents.
    /// 0 - fans are stopped
    /// 100 - fans are at full speed
    /// The returned value will be capped at 100 by the application.
    unsigned int (*getFanSpeed)(size_t deviceIndex);

    /// Set a desired fan speed (the value in a range from 0 to 100).
    /// If a desired speed value is explicitly prohibited,
    /// then the plugin should not set an error, but instead it should
    /// try to set the fan speed to a closest available value.
    /// However if the value cannot be set because of some internal device driver error
    /// then this function must set a last error.
    void (*setFanSpeed)(size_t deviceIndex, unsigned int value);

    /// Return true if the application is in control of the fan speed.
    /// Return false if the device (or plugin) controls the fans by itself.
    bool (*isFanManualMode)(size_t deviceIndex);

    /// If value = false then the application needs to obtain
    /// a control of the fan speed.
    /// If value = true then the fan speed needs to be controlled
    /// by the device itself. If the device can't control the fans by itself
    /// then the plugin must take control of the fans.
    void (*setFanManualMode)(size_t deviceIndex, bool value);

    /// Get GPU utilization rate in percents.
    /// This function pointer may be nullptr if the plugin does not support this functionality.
    unsigned int (*getGpuRate)(size_t deviceIndex);

    /// Get memory utilization rate in percents.
    /// This function pointer may be nullptr if the plugin does not support this functionality.
    unsigned int (*getMemoryRate)(size_t deviceIndex);

    /// Get total memory in bytes.
    /// This function pointer may be nullptr if the plugin does not support this functionality.
    unsigned long long (*getTotalMemory)(size_t deviceIndex);

    /// Get free memory in bytes.
    /// This function pointer may be nullptr if the plugin does not support this functionality.
    unsigned long long (*getFreeMemory)(size_t deviceIndex);

    /// The number of custom functions.
    unsigned int nCustomFunctions;

    /// The array of custom functions.
    /// May be null if there are no functions (in this case nCustomFunctions will be ignored).
    const GpuFanMeister_CustomFunction* customFunctions;
} GpuFanMeister_Plugin;

/// The type of a function pointer to the main exported plugin function.
using GpuFanMeister_GetPlugin = const GpuFanMeister_Plugin*(*)(const GpuFanMeister_AppInfo*);

/// The name of main exported plugin function.
static constexpr const char* GPUFANMEISTER_GETPLUGIN_FUNC_NAME = "gpuFanMeister_getPlugin";

/// Plugins must export this main plugin function declared below.
/// The function accepts the application information and returns the plugin information.
/// Define GPUFANMEISTER_NO_PLUGIN_EXPORT to not automatically export this function from a plugin.
#ifndef GPUFANMEISTER_NO_PLUGIN_EXPORT
    __attribute__ ((visibility("default")))
    const GpuFanMeister_Plugin* gpuFanMeister_getPlugin(const GpuFanMeister_AppInfo* appVersion);
#endif

#ifdef __cplusplus
}
#endif
