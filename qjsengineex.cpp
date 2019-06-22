/****************************************************************************}
{ qjsengineex.cpp - JS engine helper functions                               }
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

#include "qjsengineex.h"
#include <memory>

QJSValue QJSEngineEx::jsonParse(const QString &s, bool& ok)
{
    if(!jsonParseFunc)
        jsonParseFunc = std::make_unique<QJSValue>(evaluate("JSON.parse"));
    QJSValue tmpVal = jsonParseFunc->call({s});
    if(tmpVal.isError())
    {
        ok = false;
        throwError(tmpVal.toString());
        return QJSValue();
    }
    ok = true;
    return tmpVal;
}

QJSValue QJSEngineEx::jsonParse(const QString &s)
{
    bool ok;
    return jsonParse(s, ok);
}

QJSValue QJSEngineEx::jsonStringify(const QJSValue& s, bool &ok)
{
    if(!jsonStringifyFunc)
        jsonStringifyFunc = std::make_unique<QJSValue>(evaluate("JSON.stringify"));
    QJSValue tmpVal = jsonStringifyFunc->call({s});
    if(tmpVal.isError())
    {
        ok = false;
        throwError(tmpVal.toString());
        return QJSValue();
    }
    ok = true;
    return tmpVal;
}

QJSValue QJSEngineEx::jsonStringify(const QJSValue &s)
{
    bool ok;
    return jsonStringify(s, ok);
}
