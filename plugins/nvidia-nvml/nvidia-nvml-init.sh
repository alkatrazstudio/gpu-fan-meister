#!/usr/bin/env bash
cd "$(dirname -- "$(readlink -f -- "$0")")"

function err
{
    echo "$@" 1>&2
    exit 1
}

VERSION_FILE=/proc/driver/nvidia/version

if [[ ! -f $VERSION_FILE ]]
then
    err "Nvidia driver not found: $VERSION_FILE does not exist. Install the appropriate nvidia-* package." >&2
fi

VERSION_OUTPUT="$(<$VERSION_FILE)"

MODULE_VERSION=$(echo "$VERSION_OUTPUT" | grep -oiP "Kernel Module\s+\K(\d+)")
if [[ -z $MODULE_VERSION ]]
then
    err "Cannot parse the version number: $VERSION_OUTPUT"
fi

if [[ -z $(ldconfig -p | grep "^\s*libnvidia-ml.so\s") ]]
then
    err "libnvidia-ml not found. Install \"libnvidia-compute-$MODULE_VERSION\" package."
fi

if [[ -z $(which nvidia-xconfig) ]]
then
    err "nvidia-xconfig not found. Install \"nvidia-utils-$MODULE_VERSION\" package."
fi

if [[ -z $(nvidia-xconfig -t | grep -iP ' Option "CoolBits" "(4|5|6|7|12|13|14|15|20|21|22|23|28|29|30|31)"$') ]]
then
    err "CoolBits not set. You need to set the bit 2 of CoolBits option on Xorg.conf. For example, run \"sudo nvidia-xconfig --cool-bits=4\"."
fi

if [[ -z $(which nvidia-settings) ]]
then
    err "nvidia-settings not found. Install \"nvidia-settings\" package."
fi

nvidia-settings -a GPUFanControlState=0 1> /dev/null
if [[ $? != 0 ]]
then
    err "nvidia-settings -a GPUFanControlState=0: failed."
fi

# set environment variables
echo "NVML_MODULE_VERSION = $MODULE_VERSION"
