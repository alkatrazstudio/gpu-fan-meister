/****************************************************************************}
{ contdowntimer.cpp - timer that returns remaining time                      }
{                                                                            }
{ Copyright (c) 2017 Alexey Parfenov <zxed@alkatrazstudio.net>               }
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

#include "countdowntimer.h"
#include <QDateTime>

CountdownTimer::CountdownTimer(qint64 timeoutMsecs, qint64 min) :
    min(min),
    end(QDateTime::currentMSecsSinceEpoch() + timeoutMsecs)
{
}

qint64 CountdownTimer::msecsLeft() const
{
    qint64 left = end - QDateTime::currentMSecsSinceEpoch();
    return left < min ? min : left;
}
