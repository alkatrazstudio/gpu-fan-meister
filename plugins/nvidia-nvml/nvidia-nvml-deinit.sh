#!/usr/bin/env bash
cd "$(dirname -- "$(readlink -f -- "$0")")"

function err
{
    echo "$@" 1>&2
    exit 1
}

nvidia-settings -a GPUFanControlState=0 &> /dev/null
if [[ $? != 0 ]]
then
    err "IMPORTANT! nvidia-settings has failed to reset the GPU fan control state. The driver may have no control over the fans now. Run nvidia-settings manually and disable \"Thermal Settings > Enable GPU Fan Settings\" option ASAP!"
fi
