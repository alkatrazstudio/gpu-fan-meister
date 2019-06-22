/****************************************************************************}
{ InteractiveLineChart.qml - interactive fan curve graph                     }
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

import QtQuick 2.10
import QtCharts 2.2
import QtQuick.Controls 2.0

ChartView {
    id: chart

    antialiasing: true
    legend.visible: false

    margins {
        top: 0
        bottom: 0
        left: 0
        right: 0
    }

    readonly property int _MAX_TEMP: 100
    readonly property int _MAX_FAN: 100
    readonly property int _STEP: 5
    readonly property int _MAX_CATCH_RADIUS: _STEP * 0.75

    readonly property int _X_STEPS: Math.round(_MAX_TEMP / _STEP)
    readonly property int _Y_STEPS: Math.round(_MAX_FAN / _STEP)

    property int selectedIndex: -1
    property var pts

    property int temp : 0
    property int fanSpeed : 0
    property int refTemp : 0

    onTempChanged: {
        curState.clear()
        curState.append(temp, fanSpeed)
    }
    onFanSpeedChanged: {
        curState.clear()
        curState.append(temp, fanSpeed)
        refState.clear()
        refState.append(refTemp, fanSpeed)
    }
    onRefTempChanged: {
        refState.clear()
        refState.append(refTemp, fanSpeed)
    }

    signal newPts(var newPts)

    Timer {
        id: defaultPointsTimer
        interval: 0
        repeat: false
        onTriggered: {
            emitNewPoints([Qt.point(0, 0), Qt.point(_MAX_TEMP, _MAX_FAN)])
        }
    }

    onPtsChanged: {
        if(pts.length < 2)
            return defaultPointsTimer.start() // avoid binding loop

        lineSeries.clear()
        for(var a=0; a<pts.length; a++)
            lineSeries.append(pts[a].x, pts[a].y)
        setSelectedIndex(selectedIndex)
    }

    function getCurPoints()
    {
        const curPts = []
        for(var i=0; i<lineSeries.count; i++)
            curPts.push(lineSeries.at(i))
        return curPts
    }

    function emitNewPoints(_pts)
    {
        newPts(_pts)
    }

    function cursorToPoint(mouse)
    {
        return mapToValue(Qt.point(mouse.x, mouse.y), lineSeries)
    }

    function pointToIndex(p)
    {
        for(var i=0; i<pts.length; i++)
        {
            const sp = pts[i]
            const dist = Math.sqrt(Math.pow((sp.x - p.x), 2) + Math.pow((sp.y - p.y), 2))
            if(dist < _MAX_CATCH_RADIUS)
                return i
        }

        return -1
    }

    function cursorToPointIndex(mouse)
    {
        const p = cursorToPoint(mouse)
        return pointToIndex(p)
    }

    function canPlaceX(x, skipIndex)
    {
        let c = 2
        for(let i=0; i<pts.length; i++)
        {
            if(i === skipIndex)
                continue
            var gp = pts[i]
            if(gp.x === x)
            {
                c--
                if(!c)
                    return false
            }
        }
        return true
    }

    function posToGrid(p, allowOOB)
    {
        let x = Math.round(p.x/_STEP)*_STEP
        let y = Math.round(p.y/_STEP)*_STEP

        if(allowOOB)
        {
            if(x < lineSeries.axisX.min)
                x = lineSeries.axisX.min
            else if(x > lineSeries.axisX.max)
                x = lineSeries.axisX.max
            if(y < lineSeries.axisY.min)
                y = lineSeries.axisY.min
            else if(y > lineSeries.axisY.max)
                y = lineSeries.axisY.max
        }
        else
        {
            if(
                x < lineSeries.axisX.min
                || x > lineSeries.axisX.max
                || y < lineSeries.axisY.min
                || y > lineSeries.axisY.max
            ){
                return false
            }
        }

        return Qt.point(x, y)
    }

    function cursorToGridPoint(mouse, allowOOB)
    {
        const p = cursorToPoint(mouse)
        return posToGrid(p, allowOOB)
    }

    function addOrRemovePointAtCursor(mouse)
    {
        const i = cursorToPointIndex(mouse)
        if(i === -1)
        {
            // point does not exist, try to create it
            const gp = cursorToGridPoint(mouse, false)
            if(!gp)
                return
            if(!canPlaceX(gp.x))
                return

            for(let a=0; a<pts.length; a++)
            {
                const p = pts[a]
                if(gp.x > p.x)
                    continue
                if(gp.x === p.x)
                {
                    if(gp.y === p.y)
                        break
                    if(gp.y > p.y)
                        continue
                }
                const curPts = getCurPoints()
                curPts.splice(a, 0, gp)
                emitNewPoints(curPts)
                break
            }
        }
        else
        {
            // point exists, try to remove it
            if(i <= 0 || i >= lineSeries.count-1)
                return

            const curPts = getCurPoints()
            curPts.splice(i, 1)
            emitNewPoints(curPts)
        }

        updatePosAtCursor(mouse)
    }

    function setSelectedIndex(i)
    {
        scatterSeries.clear()
        if(i >= 0)
        {
            const p = lineSeries.at(i)
            scatterSeries.append(p.x, p.y)
        }
        selectedIndex = i
    }

    function updatePosAtCursor(mouse)
    {
        const i = cursorToPointIndex(mouse)
        if(i === selectedIndex)
            return
        setSelectedIndex(i)
    }

    function onDrag(mouse)
    {
        if(selectedIndex < 0)
            return

        const gp = cursorToGridPoint(mouse, true)
        if(pointToIndex(gp) >= 0)
            return

        const p = pts[selectedIndex]

        const ptPrev = pts[selectedIndex - 1]
        const ptNext = pts[selectedIndex + 1]

        if(
            !ptPrev // first point
            || !ptNext // last point
            || gp.x > ptNext.x // moving past next point
            || gp.x < ptPrev.x // moving past prev point
            || !canPlaceX(gp.x, selectedIndex)
        )
        {
            gp.x = p.x // do not move on x axis
            if(pointToIndex(gp) >= 0)
                return
        }

        const curPts = getCurPoints()
        curPts[selectedIndex] = gp
        emitNewPoints(curPts)
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton

        onDoubleClicked: addOrRemovePointAtCursor(mouse)
        onPositionChanged: {
            if(pressed)
                onDrag(mouse)
            else
                updatePosAtCursor(mouse)
        }
    }

    ValueAxis {
        id: xAxis
        labelFormat: '%dC'
        tickCount: 11
        min: 0
        max: _MAX_TEMP
        minorGridVisible: true
        titleVisible: false
    }

    ValueAxis {
        id: yAxis
        labelFormat: '%d%'
        tickCount: 11
        min: 0
        max: _MAX_FAN
        titleVisible: false
    }

    LineSeries {
        id: lineSeries
        color: 'dodgerblue'
        width: 3
        axisX: xAxis
        axisY: yAxis
    }

    ScatterSeries {
        id: scatterSeries
        color: 'yellowgreen'
        markerSize: 20
        axisX: xAxis
        axisY: yAxis
    }

    ScatterSeries {
        id: curState
        markerSize: 20
        color: 'red'
        axisX: xAxis
        axisY: yAxis
    }

    ScatterSeries {
        id: refState
        markerSize: 15
        color: 'purple'
        axisX: xAxis
        axisY: yAxis
        visible: false
    }
}
