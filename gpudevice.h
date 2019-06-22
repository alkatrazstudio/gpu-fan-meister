/****************************************************************************}
{ gpudevice.h - abstract GPU device                                          }
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

#include "gpudeviceinterface.h"
#include "qjsengineex.h"
#include "qmlsimple.h"
#include <QtQml/qqml.h>
#include <QString>
#include <QVector>
#include <QDataStream>
#include <QSettings>
#include <QColor>
#include <QSystemTrayIcon>
#include <QPainter>
#include <QMetaEnum>

#include <memory>

class GpuDevice : public QObject {
    Q_OBJECT

public:
    enum class Mode {
        GPU,
        CURVE,
        FAN_TOGGLE,
        SCRIPT
    };
    Q_ENUM(Mode)

    enum class TempDir {
        STABLE,
        INC,
        DEC
    };

    enum class Err {
        tray = 1,
        curveInvalidTemp,
        plugin,
        defaultScriptLoad
    };
    Q_ENUM(Err)

    enum class ScriptStatus {
        STOPPED,
        PAUSED,
        RUNNING
    };
    Q_ENUM(ScriptStatus)

    explicit GpuDevice(GpuDevicePlugin::CPtr plugin, size_t deviceIndex, QObject* parent = nullptr);
    ~GpuDevice();

    SIMPLE_QML_PROP(unsigned int, curveMaxIncTempDiff)
    signals: void on_curveMaxIncTempDiff_changed();
    SIMPLE_QML_PROP(unsigned int, curveIncDelay)
    signals: void on_curveIncDelay_changed();
    SIMPLE_QML_PROP(unsigned int, curveMaxDecTempDiff)
    signals: void on_curveMaxDecTempDiff_changed();
    SIMPLE_QML_PROP(unsigned int, curveDecDelay)
    signals: void on_curveDecDelay_changed();

    SIMPLE_QML_PROP(unsigned int, fanToggleEnableFansTemp)
    signals: void on_fanToggleEnableFansTemp_changed();
    SIMPLE_QML_PROP(unsigned int, fanToggleEnableFansTempSecs)
    signals: void on_fanToggleEnableFansTempSecs_changed();
    SIMPLE_QML_PROP(bool, fanToggleUseLowFanSpeed)
    signals: void on_fanToggleUseLowFanSpeed_changed();
    SIMPLE_QML_PROP(unsigned int, fanToggleLowFanSpeed)
    signals: void on_fanToggleLowFanSpeed_changed();
    SIMPLE_QML_PROP(unsigned int, fanToggleLowFanSpeedSecs)
    signals: void on_fanToggleLowFanSpeedSecs_changed();
    SIMPLE_QML_PROP(bool, fanToggleUseLowTemp)
    signals: void on_fanToggleUseLowTemp_changed();
    SIMPLE_QML_PROP(unsigned int, fanToggleLowTemp)
    signals: void on_fanToggleLowTemp_changed();
    SIMPLE_QML_PROP(unsigned int, fanToggleLowTempSecs)
    signals: void on_fanToggleLowTempSecs_changed();

    SIMPLE_QML_OBJ_PROP(QString, scriptText)
    signals: void on_scriptText_changed();
    SIMPLE_QML_OBJ_RO_PROP(QString, runningScriptText)
    signals: void on_runningScriptText_changed();
    SIMPLE_QML_PROP(bool, scriptOverride)
    signals: void on_scriptOverride_changed();
    SIMPLE_QML_OBJ_RO_PROP(QString, scriptError)
    signals: void on_scriptError_changed();

    SIMPLE_QML_RO_PROP(int, targetFanSpeed)
    signals: void on_targetFanSpeed_changed();

    SIMPLE_QML_OBJ_PROP(QColor, colorTempNormal)
    signals: void on_colorTempNormal_changed();
    SIMPLE_QML_OBJ_PROP(QColor, colorTempWarning)
    signals: void on_colorTempWarning_changed();
    SIMPLE_QML_OBJ_PROP(QColor, colorTempDanger)
    signals: void on_colorTempDanger_changed();
    SIMPLE_QML_PROP(unsigned int, tempWarning)
    signals: void on_tempWarning_changed();
    SIMPLE_QML_PROP(unsigned int, tempDanger)
    signals: void on_tempDanger_changed();

    SIMPLE_QML_OBJ_PROP(QColor, colorFanSpeedNormal)
    signals: void on_colorFanSpeedNormal_changed();
    SIMPLE_QML_OBJ_PROP(QColor, colorFanSpeedWarning)
    signals: void on_colorFanSpeedWarning_changed();
    SIMPLE_QML_OBJ_PROP(QColor, colorFanSpeedDanger)
    signals: void on_colorFanSpeedDanger_changed();
    SIMPLE_QML_PROP(unsigned int, fanSpeedWarning)
    signals: void on_fanSpeedWarning_changed();
    SIMPLE_QML_PROP(unsigned int, fanSpeedDanger)
    signals: void on_fanSpeedDanger_changed();

    SIMPLE_QML_OBJ_PROP(QColor, colorBackground)
    signals: void on_colorBackground_changed();

    SIMPLE_QML_OBJ_PROP(QString, customName)
    signals: void on_customName_changed();
    SIMPLE_QML_PROP(unsigned int, trayUpdateInterval)
    signals: void on_trayUpdateInterval_changed();

    SIMPLE_QML_CONST(int, FAN_SPEED_GPU, -1)
    SIMPLE_QML_CONST(int, FAN_SPEED_NO_CHANGE, -2)

    SIMPLE_QML_CONST(int, MIN_TEMP, 0)
    SIMPLE_QML_CONST(int, MAX_TEMP, 100)
    SIMPLE_QML_CONST(int, MIN_FAN_SPEED, 0)
    SIMPLE_QML_CONST(int, MAX_FAN_SPEED, 100)

    SIMPLE_QML_CONST_CALC(QString, MODE_CURVE, modeToString(Mode::CURVE))
    SIMPLE_QML_CONST_CALC(QString, MODE_FAN_TOGGLE, modeToString(Mode::FAN_TOGGLE))
    SIMPLE_QML_CONST_CALC(QString, MODE_SCRIPT, modeToString(Mode::SCRIPT))
    SIMPLE_QML_CONST_CALC(QString, MODE_GPU, modeToString(Mode::GPU))

    SIMPLE_QML_CONST_CALC(QString, SCRIPT_STATUS_STOPPED, scriptStatusToString(ScriptStatus::STOPPED))
    SIMPLE_QML_CONST_CALC(QString, SCRIPT_STATUS_PAUSED, scriptStatusToString(ScriptStatus::PAUSED))
    SIMPLE_QML_CONST_CALC(QString, SCRIPT_STATUS_RUNNING, scriptStatusToString(ScriptStatus::RUNNING))

    SIMPLE_QML_CONST_CALC(GpuDevicePlugin*, plugin, const_cast<GpuDevicePlugin*>(plugin_.get()))

public:
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name NOTIFY onNameChanged)
    Q_PROPERTY(QString realName READ realName CONSTANT)

    Q_PROPERTY(QString mode
               READ modeString WRITE setModeString
               NOTIFY onModeStringChanged)
    Q_PROPERTY(QVariantList curvePoints
               READ qmlCurvePoints WRITE setQmlCurvePoints
               NOTIFY onCurvePointsChanged)

    Q_PROPERTY(unsigned int temp READ temp NOTIFY onTempChanged)
    Q_PROPERTY(unsigned int fanSpeed READ fanSpeed NOTIFY onFanSpeedChanged)
    Q_PROPERTY(bool fanManualMode READ fanManualMode NOTIFY onFanManualModeChanged)
    Q_PROPERTY(unsigned int curveRefTemp READ curveRefTemp NOTIFY onCurveRefTempChanged)

    Q_PROPERTY(QString scriptStatus READ scriptStatusString NOTIFY onScriptStatusChanged)

    Q_PROPERTY(unsigned int gpuRate READ gpuRate)
    Q_PROPERTY(unsigned int memoryRate READ memoryRate)
    Q_PROPERTY(unsigned long long totalMemory READ totalMemory)
    Q_PROPERTY(unsigned long long freeMemory READ freeMemory)

    unsigned int temp() const {return temp_;}
    unsigned int fanSpeed() const {return fanSpeed_;}
    unsigned int curveRefTemp() const {return curveRefTemp_;}
    bool fanManualMode() const {return fanManualMode_;}

    unsigned int gpuRate() const {return iface_.gpuRate();}
    unsigned int memoryRate() const {return iface_.memoryRate();}
    unsigned long long totalMemory() const {return iface_.totalMemory();}
    unsigned long long freeMemory() const {return iface_.freeMemory();}

    unsigned int updateTemp();
    void setFanSpeed(unsigned int speed);
    unsigned int updateFanSpeed();
    void setFanManualMode(bool isManual);
    bool updateFanManualMode();

    const QString& name() const;
    const QString& realName() const {return realName_;}
    const QString& id() const {return id_;}

    inline Mode mode() const {return mode_;}
    inline QString modeString() const {return modeToString(mode_);}
    void setMode(Mode newMode);
    inline void setModeString(const QString& newMode){setMode(stringToMode(newMode));}
    static QString modeToStringTitle(Mode mode_);
    Q_INVOKABLE static QString modeToStringTitle(const QString& mode_);
    static QString modeToString(Mode mode_);
    static Mode stringToMode(const QString& str);

    static QString scriptStatusToString(ScriptStatus status);
    static ScriptStatus stringToScriptStatus(const QString& str);
    QString scriptStatusString() const {return scriptStatusToString(scriptStatus_);}
    bool isScriptPausedInMode(Mode mode) const;

    void setCurvePoints(const QVector<QPoint> &points);
    const QVector<QPoint>& curvePoints() const {return curvePoints_;}

    void setFanToggleManualModeMaxTemp(int val);
    void setFanToggleAutoModeMaxFanIdleSecs(int val);

    Q_INVOKABLE void loadGeneralSettings();
    Q_INVOKABLE void saveGeneralSettings();
    Q_INVOKABLE void loadModeSettings();
    Q_INVOKABLE void saveModeSettings();
    void update();
    void updateTray() {_updateTray();}
    void updateTrayUsingInterval() {_updateTray(true);}

    Q_INVOKABLE void restartScript();
    Q_INVOKABLE void stopScript();
    Q_INVOKABLE void showNotification(const QString& msg, bool isCritical = false) const;

    Q_INVOKABLE QJSValue customFunction(const QJSValue &name, const QJSValue& val = QJSValue()) const;

    static void registerInQml() {
        qmlRegisterUncreatableType<GpuDevice>("net.alkatrazstudio.gpufanmeister", 1, 0, "GpuDevice", "");
    }

protected:
    GpuDevicePlugin::CPtr plugin_;
    GpuDeviceInterface iface_;

    QString id_;
    QString realName_;

    bool fanManualMode_ {};
    unsigned int temp_ {};
    unsigned int fanSpeed_ {};
    QPixmap pixmap_;
    QPainter painter_;
    Mode mode_ {Mode::GPU};

    QSystemTrayIcon* tray_ {};
    qint64 trayLastUpdatedMsecs_ {};
    QColor curTrayFanSpeedColor_;
    QColor curTrayTempColor_;
    QColor curTrayBackgroundColor_;
    unsigned int curTrayTemp_ {};
    QString curTrayTip_;

    TempDir curveTempDir_ {TempDir::STABLE};
    unsigned int prevTempDiff_ {};
    unsigned int curveRefTemp_ {};
    unsigned int curveDelayLeft_ {};
    bool curveRefTempMoving_ {};
    QVector<QPoint> curvePoints_;

    unsigned int fanToggleCurEnableFansTempSecs_ {};
    unsigned int fanToggleCurLowFanSpeedSecs_ {};
    unsigned int fanToggleCurLowTempSecs_ {};

    std::unique_ptr<QJSEngineEx> script_;
    ScriptStatus scriptStatus_ {ScriptStatus::STOPPED};
    QJSValue scriptTickFunc_;

    QVariantList qmlCurvePoints() const;
    void setQmlCurvePoints(const QVariantList& points);

    void pauseScript();
    void unpauseScript();
    void setScriptStatus(ScriptStatus newStatus);

    static QString errorCodeToString(Err errorCode);

    static const QString &defaultScriptText();

    void _updateTray(bool useInterval = false);

    int processModeCurve();
    int processModeFanToggle();
    int processModeScript();
    int processModeOff();

    void initTray();
    void deinitCurrentMode();
    void initCurrentMode();
    int processTickFunc();
    void onScriptError(const QString& msg, bool showMsg);
    bool validateJSValue(const QJSValue& val, bool showMsg);

    void start();

    QString settingsGroupName() const;

signals:
    void onQuitClick();
    void onShowAppClick();
    void onShowDevOptionsClick();
    void onShowManualClick(const QString& uri = QString());

    void onNameChanged();

    void onModeStringChanged();

    void onCurvePointsChanged();

    void onTempChanged();
    void onFanSpeedChanged();
    void onFanManualModeChanged();
    void onCurveRefTempChanged();
    void onScriptStatusChanged();

    void onLoadDone();
    void onSaveDone();
};
