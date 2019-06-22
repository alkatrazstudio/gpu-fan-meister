/****************************************************************************}
{ qjsengineex.h - JS engine helper functions                                 }
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
#include <QObject>
#include <QJSEngine>
#include <memory>

class QJSEngineEx : public QJSEngine
{
    Q_OBJECT
public:
    QJSValue jsonParse(const QString& s, bool &ok);
    QJSValue jsonParse(const QString& s);
    QJSValue jsonStringify(const QJSValue &s, bool &ok);
    QJSValue jsonStringify(const QJSValue &s);

protected:
    std::unique_ptr<QJSValue> jsonParseFunc;
    std::unique_ptr<QJSValue> jsonStringifyFunc;
};
