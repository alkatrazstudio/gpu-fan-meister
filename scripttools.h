/****************************************************************************}
{ scripttool.h - misc tools for scripts                                      }
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

#pragma once

#include "qmlsimple.h"
#include <QObject>
#include <QVariantMap>
#include <QJSValue>
#include <QColor>

class ScriptTools : public QObject
{
    Q_OBJECT

public:
    static ScriptTools &instance();

    Q_INVOKABLE QVariantMap fromColor(const QColor& color) const;
    Q_INVOKABLE QVariantMap fromColorF(const QColor& color) const;
    Q_INVOKABLE QColor toColor(const QVariantMap& obj) const;
    Q_INVOKABLE QColor toColorF(const QVariantMap& obj) const;
    Q_INVOKABLE QJSValue clone(const QJSValue& obj, bool deep = false) const;

private:
    Q_DISABLE_COPY(ScriptTools)
    ScriptTools() : QObject(nullptr){}
};
