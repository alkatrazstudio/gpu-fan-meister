// %HEADER%

// %INITIALIZATION%
function getTimestamp()
{
    return Math.round(Date.now() / 1000)
}

const tempThreshold = 60
const maxCoolSecs = 15
let coolSecsLeft = 0
const refFanSpeed = 50
const tsStart = getTimestamp()

// %GLOBAL_OBJECT%

// %CONSOLE%
// %DEV_PLUGIN_NAME%
console.log(`Device: ${$.dev.name}; plugin: ${$.dev.plugin.name}`)

// %MAIN_FUNCTION%
;()=>{
    // %EXAMPLE%
    const secsPassed = getTimestamp() - tsStart
    if(secsPassed && secsPassed % 10 == 0)
        console.log(`The script is running for ${secsPassed} seconds`)

    // %DEV_TEMP%
    if($.dev.temp > tempThreshold)
    {
        // %IF_TEMP_HIGH%
        coolSecsLeft = maxCoolSecs
    }
    else
    {
        // %IF_TEMP_LOW%
        if(coolSecsLeft)
            coolSecsLeft--
    }

    // %IF_NEED_COOLING%
    if(coolSecsLeft)
    {
        // %RETURN_FAN_SPEED%
        return refFanSpeed + Math.max(0, ($.dev.temp - tempThreshold) / 2)
    }

    // %LET_GPU_DECIDE%
    return $.dev.FAN_SPEED_GPU
}
