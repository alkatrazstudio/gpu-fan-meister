/****************************************************************************}
{ scripttool.cpp - misc tools for scripts                                    }
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

#include "scripttools.h"
#include <QQmlEngine>
#include <QColor>
#include <QJSValueIterator>

ScriptTools &ScriptTools::instance()
{
    static ScriptTools inst;
    QQmlEngine::setObjectOwnership(&inst, QQmlEngine::CppOwnership);
    return inst;
}

QVariantMap ScriptTools::fromColor(const QColor &color) const
{
    return {
        {"r", color.red()},
        {"g", color.green()},
        {"b", color.blue()},
        {"a", color.alpha()}
    };
}

QVariantMap ScriptTools::fromColorF(const QColor &color) const
{
    return {
        {"r", color.redF()},
        {"g", color.greenF()},
        {"b", color.blueF()},
        {"a", color.alphaF()}
    };
}

QColor ScriptTools::toColor(const QVariantMap &obj) const
{
    return QColor(
        obj["r"].toInt(),
        obj["g"].toInt(),
        obj["b"].toInt(),
        obj["a"].toInt());
}

QColor ScriptTools::toColorF(const QVariantMap &obj) const
{
    return QColor::fromRgbF(
        obj["r"].toReal(),
        obj["g"].toReal(),
        obj["b"].toReal(),
        obj["a"].toReal());
}

QJSValue ScriptTools::clone(const QJSValue &obj, bool deep) const
{
    if(obj.isQMetaObject() || obj.isQObject())
        return obj;

    QJSValue newVal;
    if(obj.isArray())
        newVal = obj.engine()->newArray();
    else if(obj.isObject())
        newVal = obj.engine()->newObject();
    else
        return obj;

    QJSValueIterator it(obj);
    while(it.next())
    {
        newVal.setProperty(
            it.name(),
            deep ? clone(it.value()) : it.value());
    }
    return newVal;
}
