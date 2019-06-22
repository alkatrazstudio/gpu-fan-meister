/****************************************************************************}
{ gpudevice.cpp - abstract GPU device                                        }
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

#include "gpudevice.h"
#include "scriptsys.h"
#include "scripttools.h"
#include "errormanager.h"
#include "appsettings.h"
#include "enumutil.h"
#include "qtnotify.h"
#include "coreapp.h"
#include <QVariant>
#include <QMenu>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QRegularExpression>

static constexpr int TRAY_PIXMAP_SIZE = 64;

#define PLUGIN_CHECK(t, v, x)              \
    t v;                                   \
    try{                                   \
        v = x;                             \
    }catch(GpuDeviceInterface::Error& e){  \
        SETERROR_E(Err::plugin, e.text()); \
    }catch(std::exception& e){             \
        SETERROR_E(Err::plugin, e.what()); \
    }catch(...){                           \
        SETERROR_E(Err::plugin);           \
    }

#define PLUGIN_CHECKV(x)                   \
    try{                                   \
        x;                                 \
    }catch(GpuDeviceInterface::Error& e){  \
        SETERROR_E(Err::plugin, e.text()); \
    }catch(std::exception& e){             \
        SETERROR_E(Err::plugin, e.what()); \
    }catch(...){                           \
        SETERROR_E(Err::plugin);           \
    }

GpuDevice::GpuDevice(GpuDevicePlugin::CPtr plugin, size_t deviceIndex, QObject *parent) :
    QObject(parent),
    targetFanSpeed_(FAN_SPEED_GPU),
    plugin_(plugin),
    iface_(plugin, deviceIndex),
    id_(iface_.id()),
    realName_(iface_.name()),
    pixmap_(TRAY_PIXMAP_SIZE, TRAY_PIXMAP_SIZE)
{
    connect(this, &GpuDevice::on_customName_changed, this, &GpuDevice::onNameChanged);
    initTray();
    loadGeneralSettings();
    loadModeSettings();
    if(!isScriptPausedInMode(mode_))
        restartScript();
    start();
}

GpuDevice::~GpuDevice()
{
    if(fanManualMode_)
    {
        try{
            setFanManualMode(false);
        }catch(ErrorManager::Exception& e){
            Q_UNUSED(e);
        }
    }

    if(tray_)
    {
        QMenu* trayMenu = tray_->contextMenu();
        tray_->setContextMenu(nullptr);
        delete trayMenu;
    }

    // need to explicitly release the pointer
    // because of some bug causing some mutex
    // to indefinitely lock the destructor
    script_.release();
}

void GpuDevice::loadGeneralSettings()
{
    AppSettings conf;
    conf.beginGroup(settingsGroupName());

    set_customName(conf.getString("customName"));

    QPalette p;
    const QColor& colorNormal = p.color(QPalette::Foreground);

    set_colorTempNormal(conf.getColor("colorTempNormal", colorNormal));
    set_colorTempWarning(conf.getColor("colorTempWarning", colorNormal.valueF() > 0.5 ? "gold" : "saddlebrown"));
    set_colorTempDanger(conf.getColor("colorTempDanger", QColor(Qt::red)));
    set_tempWarning(conf.getUInt("tempWarning", 60));
    set_tempDanger(conf.getUInt("tempDanger", 75));
    set_colorFanSpeedNormal(conf.getColor("colorFanSpeedNormal", colorNormal));
    set_colorFanSpeedWarning(conf.getColor("colorFanSpeedWarning", colorNormal.valueF() > 0.5 ? "lightskyblue" : "deepskyblue"));
    set_colorFanSpeedDanger(conf.getColor("colorFanSpeedDanger", QColor(Qt::red)));
    set_fanSpeedWarning(conf.getUInt("fanSpeedWarning", 1));
    set_fanSpeedDanger(conf.getUInt("fanSpeedDanger", 80));
    set_colorBackground(conf.getColor("colorBackground", QColor(Qt::transparent)));
    set_trayUpdateInterval(conf.getUInt("trayUpdateInterval", 5));
}

void GpuDevice::saveGeneralSettings()
{
    AppSettings conf;
    conf.beginGroup(settingsGroupName());

    conf.setValue("customName", customName_);
    conf.setColor("colorTempNormal", colorTempNormal_);
    conf.setColor("colorTempWarning", colorTempWarning_);
    conf.setColor("colorTempDanger", colorTempDanger_);
    conf.setValue("tempWarning", tempWarning_);
    conf.setValue("tempDanger", tempDanger_);
    conf.setColor("colorFanSpeedNormal", colorFanSpeedNormal_);
    conf.setColor("colorFanSpeedWarning", colorFanSpeedWarning_);
    conf.setColor("colorFanSpeedDanger", colorFanSpeedDanger_);
    conf.setValue("fanSpeedWarning", fanSpeedWarning_);
    conf.setValue("fanSpeedDanger", fanSpeedDanger_);
    conf.setValue("colorBackground", colorBackground_);
}

void GpuDevice::loadModeSettings()
{
    AppSettings conf;
    conf.beginGroup(settingsGroupName());

    set_curveMaxIncTempDiff(conf.getUInt("curveMaxIncTempDiff", 5));
    set_curveIncDelay(conf.getUInt("curveIncDelay", 5));
    set_curveMaxDecTempDiff(conf.getUInt("curveMaxDecTempDiff", 5));
    set_curveDecDelay(conf.getUInt("curveDecDelay", 5));

    set_fanToggleEnableFansTemp(conf.getUInt("fanToggleEnableFansTemp", 60));
    set_fanToggleEnableFansTempSecs(conf.getUInt("fanToggleEnableFansTempSecs", 0));
    set_fanToggleUseLowFanSpeed(conf.getBool("fanToggleUseLowFanSpeed", true));
    set_fanToggleLowFanSpeed(conf.getUInt("fanToggleLowFanSpeed", 0));
    set_fanToggleLowFanSpeedSecs(conf.getUInt("fanToggleLowFanSpeedSecs", 5));
    set_fanToggleUseLowTemp(conf.getBool("fanToggleUseLowTemp", true));
    set_fanToggleLowTemp(conf.getUInt("fanToggleLowTemp", 50));
    set_fanToggleLowTempSecs(conf.getUInt("fanToggleLowTempSecs", 5));

    QString scriptText = conf.getString("scriptText", "");
    if(scriptText.isEmpty())
    {
        scriptText = defaultScriptText();
        if(scriptText.isEmpty())
            SETERROR(Err::defaultScriptLoad);
    }
    set_scriptText(scriptText);
    set_scriptOverride(conf.getBool("scriptOverride", false));

    int nPoints = conf.beginReadArray("curvePoints");
    QVector<QPoint> points;
    points.reserve(nPoints);
    for(int i=0; i<nPoints; i++)
    {
        conf.setArrayIndex(i);
        int x = conf.getInt("x");
        int y = conf.getInt("y");
        QPoint p(x, y);
        points.append(p);
    }
    conf.endArray();

    setCurvePoints(points);

    // set mode only after everything else is loaded
    setModeString(conf.getString("mode"));

    emit onLoadDone();
}

void GpuDevice::saveModeSettings()
{
    AppSettings conf;
    conf.beginGroup(settingsGroupName());

    conf.setValue("mode", modeToString(mode_));

    conf.setValue("curveMaxIncTempDiff", curveMaxIncTempDiff_);
    conf.setValue("curveIncDelay", curveIncDelay_);
    conf.setValue("curveMaxDecTempDiff", curveMaxDecTempDiff_);
    conf.setValue("curveDecDelay", curveDecDelay_);

    conf.setValue("fanToggleEnableFansTemp", fanToggleEnableFansTemp_);
    conf.setValue("fanToggleEnableFansTempSecs", fanToggleEnableFansTempSecs_);
    conf.setValue("fanToggleUseLowFanSpeed", fanToggleUseLowFanSpeed_);
    conf.setValue("fanToggleLowFanSpeed", fanToggleLowFanSpeed_);
    conf.setValue("fanToggleLowFanSpeedSecs", fanToggleLowFanSpeedSecs_);
    conf.setValue("fanToggleUseLowTemp", fanToggleUseLowTemp_);
    conf.setValue("fanToggleLowTemp", fanToggleLowTemp_);
    conf.setValue("fanToggleLowTempSecs", fanToggleLowTempSecs_);

    conf.setValue("scriptText", scriptText_);
    conf.setValue("scriptOverride", scriptOverride_);

    conf.beginWriteArray("curvePoints", curvePoints_.size());
    int i = 0;
    for(const QPoint& p : qAsConst(curvePoints_))
    {
        conf.setArrayIndex(i);
        conf.setValue("x", p.x());
        conf.setValue("y", p.y());
        i++;
    }
    conf.endArray();

    emit onSaveDone();
}

QVariantList GpuDevice::qmlCurvePoints() const
{
    QVariantList points;
    points.reserve(curvePoints_.size());
    for(const QPoint &p : qAsConst(curvePoints_))
        points.append(p);
    return points;
}

void GpuDevice::setQmlCurvePoints(const QVariantList &points)
{
    QVector<QPoint> pts;
    pts.reserve(points.size());
    for(const QVariant& v : qAsConst(points))
    {
        if(v.canConvert(QMetaType::QPoint))
            pts.append(v.toPoint());
        else if(v.canConvert(QMetaType::QVariantMap))
        {
            QVariantMap obj = v.toMap();
            pts.append(QPoint(obj["x"].toInt(), obj["y"].toInt()));
        }
    }
    setCurvePoints(pts);
}

void GpuDevice::pauseScript()
{
    if(scriptStatus_ == ScriptStatus::RUNNING)
        setScriptStatus(ScriptStatus::PAUSED);
}

void GpuDevice::unpauseScript()
{
    if(scriptStatus_ == ScriptStatus::PAUSED)
        setScriptStatus(ScriptStatus::RUNNING);
}

void GpuDevice::setScriptStatus(GpuDevice::ScriptStatus newStatus)
{
    scriptStatus_ = newStatus;
    emit onScriptStatusChanged();
}

void GpuDevice::start()
{
    setFanManualMode(true);
    setFanManualMode(false);
    updateTray();
    tray_->show();

    connect(this, &GpuDevice::on_colorTempNormal_changed, &GpuDevice::updateTray);
    connect(this, &GpuDevice::on_colorTempWarning_changed, &GpuDevice::updateTray);
    connect(this, &GpuDevice::on_colorTempDanger_changed, &GpuDevice::updateTray);
    connect(this, &GpuDevice::on_tempWarning_changed, &GpuDevice::updateTray);
    connect(this, &GpuDevice::on_tempDanger_changed, &GpuDevice::updateTray);
    connect(this, &GpuDevice::on_colorFanSpeedNormal_changed, &GpuDevice::updateTray);
    connect(this, &GpuDevice::on_colorFanSpeedWarning_changed, &GpuDevice::updateTray);
    connect(this, &GpuDevice::on_colorFanSpeedDanger_changed, &GpuDevice::updateTray);
    connect(this, &GpuDevice::on_fanSpeedWarning_changed, &GpuDevice::updateTray);
    connect(this, &GpuDevice::on_fanSpeedDanger_changed, &GpuDevice::updateTray);
    connect(this, &GpuDevice::on_colorBackground_changed, &GpuDevice::updateTray);

    connect(this, &GpuDevice::on_scriptOverride_changed, [this]{
        if(isScriptPausedInMode(mode_))
            pauseScript();
        else
            unpauseScript();
    });
}

QString GpuDevice::settingsGroupName() const
{
    return iface_.plugin()->id() + "-DEVICE-" + id_;
}

QString GpuDevice::modeToString(Mode mode)
{
    QString s = EnumUtil::toString(mode, Mode::GPU);
    return s;
}

QString GpuDevice::modeToStringTitle(Mode mode)
{
    switch(mode)
    {
        case Mode::CURVE: return tr("Curve", "mode");
        case Mode::FAN_TOGGLE: return tr("Fan toggle", "mode");
        case Mode::SCRIPT: return tr("Scripted", "mode");
        default: return tr("GPU controlled", "mode");
    }
}

QString GpuDevice::modeToStringTitle(const QString &mode)
{
    return modeToStringTitle(stringToMode(mode));
}

GpuDevice::Mode GpuDevice::stringToMode(const QString &str)
{
    return EnumUtil::fromString(str, Mode::GPU);
}

QString GpuDevice::scriptStatusToString(GpuDevice::ScriptStatus status)
{
    return EnumUtil::toString(status, ScriptStatus::STOPPED);
}

GpuDevice::ScriptStatus GpuDevice::stringToScriptStatus(const QString &str)
{
    return EnumUtil::fromString(str, ScriptStatus::STOPPED);
}

bool GpuDevice::isScriptPausedInMode(GpuDevice::Mode mode) const
{
    switch(mode)
    {
        case Mode::GPU: return true;
        case Mode::SCRIPT: return scriptOverride_;
        default: return !scriptOverride_;
    }
}

void GpuDevice::setCurvePoints(const QVector<QPoint>& points)
{
    curvePoints_ = points;
    if(curvePoints_.size() < 2)
    {
        curvePoints_ = {
            {MIN_TEMP, MIN_FAN_SPEED},
            {MAX_TEMP, MAX_FAN_SPEED}
        };
    }
    else
    {
        curvePoints_.first().setX(MIN_TEMP);
        curvePoints_.last().setX(MAX_TEMP);
    }

    for(auto &p : curvePoints_)
    {
        if(p.x() > MAX_TEMP)
            p.setX(MAX_TEMP);
        else if(p.x() < MIN_TEMP)
            p.setX(MIN_TEMP);
        if(p.y() > MAX_FAN_SPEED)
            p.setY(MAX_FAN_SPEED);
        else if(p.y() < MIN_FAN_SPEED)
            p.setY(MIN_FAN_SPEED);
    }

    emit onCurvePointsChanged();
}

unsigned int GpuDevice::updateTemp()
{
    PLUGIN_CHECK(unsigned int, curTemp, iface_.temperature());
    if(curTemp >= MAX_TEMP)
        curTemp = MAX_TEMP - 1;
    if(curTemp != temp_)
    {
        temp_ = curTemp;
        emit onTempChanged();
    }
    return temp_;
}

void GpuDevice::setFanManualMode(bool isManual)
{
    PLUGIN_CHECKV(iface_.setFanManualMode(isManual));
    updateFanManualMode();
    updateTemp();
    updateFanSpeed();
}

bool GpuDevice::updateFanManualMode()
{
    PLUGIN_CHECK(bool, isManual, iface_.isFanManualMode());
    if(isManual != fanManualMode_)
    {
        fanManualMode_ = isManual;
        emit onFanManualModeChanged();
    }
    return fanManualMode_;
}

const QString &GpuDevice::name() const
{
    if(!customName_.isEmpty())
        return customName_;
    return realName_;
}

void GpuDevice::setMode(Mode newMode)
{
    if(mode_ == newMode)
        return;

    mode_ = newMode;

    switch(mode_)
    {
        case Mode::CURVE:
            setFanManualMode(true);
            curveTempDir_ = TempDir::STABLE;
            curveRefTemp_ = temp_;
            emit onCurveRefTempChanged();
            curveDelayLeft_ = 0;
            prevTempDiff_ = 0;
            curveRefTempMoving_ = false;
            break;

        case Mode::FAN_TOGGLE:
            setFanManualMode(false);
            fanToggleCurEnableFansTempSecs_ = 0;
            fanToggleCurLowFanSpeedSecs_ = 0;
            fanToggleCurLowTempSecs_ = 0;
            break;

        case Mode::SCRIPT:
            setFanManualMode(false);
            break;

        default:
            setFanManualMode(false);
            pauseScript();
            break;
    }

    if(isScriptPausedInMode(mode_))
        pauseScript();
    else
        unpauseScript();

    update();
    updateTray();
    emit onModeStringChanged();
}

unsigned int GpuDevice::updateFanSpeed()
{
    PLUGIN_CHECK(unsigned int, curFanSpeed, iface_.fanSpeed());
    if(curFanSpeed > MAX_FAN_SPEED)
        curFanSpeed = MAX_FAN_SPEED;
    if(fanSpeed_ != curFanSpeed)
    {
        fanSpeed_ = curFanSpeed;
        emit onFanSpeedChanged();
    }
    return fanSpeed_;
}

void GpuDevice::setFanSpeed(unsigned int speed)
{
    PLUGIN_CHECKV(iface_.setFanSpeed(speed));
    updateFanSpeed();
}

void GpuDevice::update()
{
    updateFanSpeed();
    updateTemp();

    int targetFanSpeedTmp;

    switch(mode_)
    {
        case Mode::CURVE:
            targetFanSpeedTmp = processModeCurve();
            break;

        case Mode::FAN_TOGGLE:
            targetFanSpeedTmp = processModeFanToggle();
            break;

        case Mode::SCRIPT:
            targetFanSpeedTmp = FAN_SPEED_GPU;
            break;

        case Mode::GPU:
        default:
            targetFanSpeedTmp = processModeOff();
            break;
    }

    set_targetFanSpeed(targetFanSpeedTmp);
    targetFanSpeedTmp = processModeScript();
    if(targetFanSpeedTmp != FAN_SPEED_NO_CHANGE)
        set_targetFanSpeed(targetFanSpeedTmp);

    if(targetFanSpeed_ >= 0)
    {
        setFanManualMode(true);
        setFanSpeed(static_cast<unsigned int>(targetFanSpeed_));
    }
    else
    {
        switch(targetFanSpeed_)
        {
            case FAN_SPEED_NO_CHANGE:
                break;

            case FAN_SPEED_GPU:
            default:
                setFanManualMode(false);
        }
    }

    updateTrayUsingInterval();
}

int GpuDevice::processModeCurve()
{
    unsigned oldCurveRefTemp = 0;

    if(temp_ == curveRefTemp_)
    {
        curveTempDir_ = TempDir::STABLE;
        curveRefTempMoving_ = false;
    }
    else
    {
        TempDir curDir = (temp_ > curveRefTemp_) ? TempDir::INC : TempDir::DEC;
        if(curveTempDir_ != curDir)
        {
            curveTempDir_ = curDir;
            prevTempDiff_ = 0;
            curveRefTempMoving_ = false;
            if(curDir == TempDir::INC)
                curveDelayLeft_ = curveIncDelay_;
            else
                curveDelayLeft_ = curveDecDelay_;
        }

        // if we're not moving
        if(!curveRefTempMoving_)
        {
            // check if we've waited long enough
            curveDelayLeft_--;
            if(!curveDelayLeft_)
                curveRefTempMoving_ = true;
        }

        // if it's rising
        if(curveTempDir_ == TempDir::INC)
        {
            // if it's rising too fast
            if(temp_ - curveRefTemp_ > curveMaxIncTempDiff_)
            {
                // keep up with it
                curveRefTemp_ = temp_ - curveMaxIncTempDiff_;
            }
            else
            {
                // if we're moving
                if(curveRefTempMoving_)
                {
                    // move to the target
                    curveRefTemp_++;
                }
            }
        }
        else // if it's lowering
        {
            // if it's lowering too fast
            if(curveRefTemp_ - temp_ > curveMaxDecTempDiff_)
            {
                // keep up with it
                curveRefTemp_ = temp_ + curveMaxDecTempDiff_;
            }
            else
            {
                // if we're moving
                if(curveRefTempMoving_)
                {
                    // move to the target
                    curveRefTemp_--;
                }
            }
        }
    }

    bool ok = false;
    unsigned int destFanSpeed;
    if(curveTempDir_ == TempDir::INC)
    {
        for(int a=curvePoints_.size()-1; a>=0; a--)
        {
            const QPoint& p = curvePoints_[a];
            if(p.x() > curveRefTemp_)
                continue;

            if(p.x() == curveRefTemp_ || a == curvePoints_.size() - 1)
            {
                destFanSpeed = p.y();
                ok = true;
                break;
            }

            int diffX = curvePoints_[a+1].x() - p.x();
            int diffY = curvePoints_[a+1].y() - p.y();
            int diffTargetX = curveRefTemp_ - p.x();
            destFanSpeed = diffY * diffTargetX / diffX + p.y();
            ok = true;
            break;
        }
    }
    else
    {
        for(int a=0; a<curvePoints_.size(); a++)
        {
            const QPoint& p = curvePoints_[a];
            if(p.x() < curveRefTemp_)
                continue;

            if(curveRefTemp_ == p.x() || a == 0)
            {
                destFanSpeed = p.y();
                ok = true;
                break;
            }

            int diffX = curvePoints_[a-1].x() - p.x();
            int diffY = curvePoints_[a-1].y() - p.y();
            int diffTargetX = curveRefTemp_ - p.x();
            destFanSpeed = diffY * diffTargetX / diffX + p.y();
            ok = true;
            break;
        }
    }

    CHECKE(ok, Err::curveInvalidTemp);

    if(oldCurveRefTemp != curveRefTemp_)
        emit onCurveRefTempChanged();

    return destFanSpeed;
}

int GpuDevice::processModeFanToggle()
{
    if(fanManualMode_)
    {
        // if the temperature is high enough
        // then enable the fans
        if(temp_ >= fanToggleEnableFansTemp_)
        {
            fanToggleCurEnableFansTempSecs_++;
            if(fanToggleCurEnableFansTempSecs_ > fanToggleEnableFansTempSecs_)
            {
                fanToggleCurEnableFansTempSecs_ = 0;
                return FAN_SPEED_GPU;
            }
        }
    }
    else
    {
        if(fanToggleUseLowFanSpeed_ && fanSpeed_ <= fanToggleLowFanSpeed_)
        {
            fanToggleCurLowFanSpeedSecs_++;
            // if the fan speed is low enough for a certain time
            // then disable the fans
            if(fanToggleCurLowFanSpeedSecs_ > fanToggleLowFanSpeedSecs_)
            {
                fanToggleCurLowFanSpeedSecs_ = 0;
                return 0;
            }
        }
        else
        {
            fanToggleCurLowFanSpeedSecs_ = 0;
        }

        if(fanToggleUseLowTemp_ && temp_ <= fanToggleLowTemp_)
        {
            fanToggleCurLowTempSecs_++;
            // if the temperature is low enough for a certain time
            // then disable the fans
            if(fanToggleCurLowTempSecs_ > fanToggleLowTempSecs_)
            {
                fanToggleCurLowTempSecs_ = 0;
                return 0;
            }
        }
        else
        {
            fanToggleCurLowTempSecs_ = 0;
        }
    }
    return FAN_SPEED_NO_CHANGE;
}

int GpuDevice::processModeScript()
{
    if(scriptStatus_ != ScriptStatus::RUNNING)
        return FAN_SPEED_NO_CHANGE;
    return processTickFunc();
}

int GpuDevice::processModeOff()
{
    return FAN_SPEED_GPU;
}

void GpuDevice::initTray()
{
    CHECKE(QSystemTrayIcon::isSystemTrayAvailable(), Err::tray);

    tray_ = new QSystemTrayIcon(this);
    tray_->setContextMenu(new QMenu);

    QAction* actionShowMainForm = tray_->contextMenu()->addAction(tr("Open the main window"));
    connect(actionShowMainForm, &QAction::triggered, this, &GpuDevice::onShowAppClick);

    QAction* actionShowDevOptions = tray_->contextMenu()->addAction(tr("Change this icon's colors"));
    connect(actionShowDevOptions, &QAction::triggered, this, &GpuDevice::onShowDevOptionsClick);

    QAction* actionReadManual = tray_->contextMenu()->addAction(tr("Read the manual"));
    connect(actionReadManual, &QAction::triggered, [this]{
        emit GpuDevice::onShowManualClick();
    });

    QAction* actionQuit = tray_->contextMenu()->addAction(tr("Quit"));
    connect(actionQuit, &QAction::triggered, this, &GpuDevice::onQuitClick);

    pixmap_.fill(Qt::transparent);
    painter_.begin(&pixmap_);
    QFont f = painter_.font();
    f.setPixelSize(42);
    f.setBold(true);
    f.setStyleHint(QFont::Monospace);
    f.setFamily("Noto Mono");
    f.setBold(true);
    painter_.setFont(f);

    connect(tray_, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason){
        if(reason == QSystemTrayIcon::Trigger)
            emit onShowAppClick();
    });
}

void GpuDevice::restartScript()
{
    stopScript();

    std::unique_ptr<QJSEngineEx> newScript(new QJSEngineEx());
    newScript->installExtensions(QJSEngine::ConsoleExtension);
    QJSValue obj = newScript->newObject();

    QJSValue info = newScript->newObject();
    info.setProperty("id", qGuiApp->applicationName());
    info.setProperty("title", qGuiApp->applicationDisplayName());
    info.setProperty("author", qGuiApp->organizationName());
    info.setProperty("version", qCoreApp->applicationVersion());
    info.setProperty("majorVersion", qCoreApp->majorVersion());
    info.setProperty("minorVersion", qCoreApp->minorVersion());
    info.setProperty("patchVersion", qCoreApp->patchVersion());
    obj.setProperty("app", info);

    obj.setProperty("dev", newScript->newQObject(this));
    obj.setProperty("sys", newScript->newQObject(&ScriptSys::instance()));
    obj.setProperty("tools", newScript->newQObject(&ScriptTools::instance()));

    newScript->globalObject().setProperty("$", obj);

    QJSValue tickFunc = newScript->evaluate(scriptText_);
    if(!validateJSValue(tickFunc, true))
        return;
    if(!tickFunc.isCallable())
    {
        onScriptError(tr("The script did not return a function."), false);
        return;
    }

    script_.swap(newScript);
    scriptTickFunc_ = std::move(tickFunc);
    set_scriptError("");

    if(isScriptPausedInMode(mode_))
        setScriptStatus(ScriptStatus::PAUSED);
    else
        setScriptStatus(ScriptStatus::RUNNING);
    set_runningScriptText(scriptText_);
}

void GpuDevice::stopScript()
{
    setScriptStatus(ScriptStatus::STOPPED);
}

void GpuDevice::showNotification(const QString &msg, bool isCritical) const
{
    Notification::showDetached(name(), msg, isCritical);
}

QJSValue GpuDevice::customFunction(const QJSValue &name, const QJSValue &val) const
{
    return iface_.customFunction(name.toString(), val, static_cast<QJSEngineEx*>(name.engine()));
}

int GpuDevice::processTickFunc()
{
    QJSValue ret = scriptTickFunc_.call();
    if(!validateJSValue(ret, true))
        return FAN_SPEED_GPU;
    if(!ret.isNumber())
    {
        onScriptError("The script did not return a Number, got "+ret.toString(), true);
        return FAN_SPEED_GPU;
    }

    int val = qBound(-1, ret.toInt(), 100);
    if(val < 0)
    {
        if(val != FAN_SPEED_NO_CHANGE)
            return FAN_SPEED_GPU;
    }
    return val;
}

void GpuDevice::onScriptError(const QString &msg, bool showMsg)
{
    set_scriptError(msg);
    stopScript();
    if(showMsg)
        showNotification(msg, true);
}

bool GpuDevice::validateJSValue(const QJSValue &val, bool showMsg)
{
    if(val.isError())
    {
        QString msg = QString("At line %1: %2")
                      .arg(val.property("lineNumber").toInt())
                      .arg(val.toString());
        onScriptError(msg, showMsg);
        return false;
    }
    return true;
}

void GpuDevice::_updateTray(bool useInterval)
{
    QString tempString = QString::number(temp_);
    QString devName = name();
    QString tip =
        QStringLiteral("<u>") % devName.replace("&", "&amp;").replace("<", "&lt;") % "</u>"
        % "<br>" % tr("Mode", "tray") % "</b>: <b>" % modeToStringTitle(mode_) % "</b>"
        % "<br>" % tr("Fan control", "tray") % ": <b>" % (fanManualMode_ ? tr("app", "tray") : tr("GPU", "tray")) % "</b>"
        % "<br>" % tr("Temp", "tray") % ": <b>" % tempString % "\u00B0C</b>"
        % "<br>" % tr("Fans", "tray") % ": <b>" % QString::number(fanSpeed_) % "%</b>";
    if(tip != curTrayTip_)
        tray_->setToolTip(tip);

    const QColor& fanSpeedColor = fanSpeed_ >= fanSpeedDanger_
                                  ? colorFanSpeedDanger_
                                  : (fanSpeed_ >= fanSpeedWarning_
                                    ? colorFanSpeedWarning_
                                    : colorFanSpeedNormal_);
    const QColor& tempColor = temp_ >= tempDanger_
                              ? colorTempDanger_
                              : (temp_ >= tempWarning_
                                 ? colorTempWarning_
                                 : colorTempNormal_);

    if(fanSpeedColor == curTrayFanSpeedColor_
       && tempColor == curTrayTempColor_
       && temp_ == curTrayTemp_
       && colorBackground_ == curTrayBackgroundColor_)
    {
        return;
    }

    qint64 now = QDateTime::currentMSecsSinceEpoch();
    if(useInterval)
    {
        qint64 secsPassed = qRound((now - trayLastUpdatedMsecs_) / 1000.0);
        if(secsPassed < trayUpdateInterval_)
            return;
    }

    pixmap_.fill(colorBackground_);
    painter_.setPen(QPen(fanSpeedColor, 6));
    painter_.drawRect(pixmap_.rect());

    painter_.setPen(QPen(tempColor,1));
    painter_.drawText(pixmap_.rect(), Qt::AlignCenter, tempString);

    tray_->setIcon(QIcon(pixmap_));

    curTrayFanSpeedColor_ = fanSpeedColor;
    curTrayTempColor_ = tempColor;
    curTrayTemp_ = temp_;
    trayLastUpdatedMsecs_ = now;
}

QString GpuDevice::errorCodeToString(Err errorCode)
{
    switch(errorCode)
    {
        case Err::plugin: return "Plugin error";
        case Err::tray: return "Cannot create tray";
        case Err::curveInvalidTemp: return "Cannot get temperature by curve";
        case Err::defaultScriptLoad: return "Cannot load the default script text";
    }
    return QString();
}

const QString &GpuDevice::defaultScriptText()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    static QString text;
    if(!text.isEmpty())
        return text;

    QFile file(qApp->applicationDirPath() + "/script.default.js");
    if(!file.open(QIODevice::ReadOnly))
        return text;

    QByteArray arr = file.readAll();
    text = QString::fromUtf8(arr);
    QStringList lines = text.split("\n");
    QStringList finalLines;
    finalLines.reserve(lines.size());
    QRegularExpression rx("^(\\s*//\\s+)%([^%]+)%\\s*$");

    QHash<QString, QString> trMap = {{
        "HEADER",
        QCoreApplication::translate("Default script",
            "Script language: ECMAScript 6")
    }, {
        "INITIALIZATION",
        QCoreApplication::translate("Default script",
            "top-level code will be run each time the script is (re)initialized")
    }, {
        "GLOBAL_OBJECT",
        QCoreApplication::translate("Default script",
            "A global object $ is available in the script.\n"
            "This object  exposes most of the application's functionality to this script.\n"
            "Below are several usage examples.\n"
            "For the full documentation press the \"Help\" button.")
    }, {
        "CONSOLE",
        QCoreApplication::translate("Default script",
            "console.log() will print to stdout\n"
            "console.error() will print to stderr")
    }, {
        "DEV_PLUGIN_NAME",
        QCoreApplication::translate("Default script",
            "$.dev.name holds the device name, e.g. \"GeForce GTX 1060 6GB\"\n"
            "$.dev.plugin.name holds the GPU plugin name, e.g. \"NVIDIA (NVML)\"")
    }, {
        "MAIN_FUNCTION",
        QCoreApplication::translate("Default script",
            "The below function will be invoked each second.\n"
            "It must return the desired fan speed (Number in range [0;100])\n"
            "or one of special constants (see the example below or the full documentation.\n"
            "The script code must \"end\" with this function using one of this methods:\n"
            "1) The function must be an arrow function and no statements should come after it:\n"
            "    ()=>{}\n"
            "2) Script code must end with the main function name:\n"
            "    function func(){}\n"
            "    func\n"
            "Press the \"Help\" button for more info.\n"
            "This function is the only required thing in the script,\n"
            "everything else is optional.")
    }, {
        "EXAMPLE",
        QCoreApplication::translate("Default script",
            "an example algorithm")
    }, {
        "DEV_TEMP",
        QCoreApplication::translate("Default script",
            "$.dev.temp holds the current temperature in degrees Celsius")
    }, {
        "IF_TEMP_HIGH",
        QCoreApplication::translate("Default script",
            "if the temperature is high enough\n"
            "then start an aggresive cooling\n"
            "for at least maxCoolSecs seconds")
    }, {
        "IF_TEMP_LOW",
        QCoreApplication::translate("Default script",
            "if the temperature is low enough\n"
            "then decrement the cooling timer")
    }, {
        "IF_NEED_COOLING",
        QCoreApplication::translate("Default script",
            "if the cooling is still needed")
    }, {
        "RETURN_FAN_SPEED",
        QCoreApplication::translate("Default script",
            "return the desired fan speed\n"
            "(this value will be capped to 100 and truncated to integer)")
    }, {
        "LET_GPU_DECIDE",
        QCoreApplication::translate("Default script",
            "if the cooling is no longer needed\n"
            "then let the GPU decide the speed\n"
            "using a special constant $.dev.FAN_SPEED_GPU")
    }};

    for(const QString& line : qAsConst(lines))
    {
        QRegularExpressionMatch m = rx.match(line);
        if(!m.hasMatch())
        {
            finalLines.append(line);
            continue;
        }

        QString trLine = m.captured(1) + trMap[m.captured(2)].replace("\n", QString("\n")+m.captured(1));
        finalLines.append(trLine);
    }

    text = finalLines.join("\n");
    return text;
}
