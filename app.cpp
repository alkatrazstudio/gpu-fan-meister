/****************************************************************************}
{ app.cpp - main application                                                 }
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

#include "app.h"
#include "common_defines.h"
#include "appsettings.h"
#include "codehighlighter.h"
#include "qtnotify.h"
#include <QDir>
#include <QQmlContext>
#include <QQuickStyle>
#include <QTextStream>
#include <QMetaObject>
#include <QDesktopServices>
#include <iostream>

static const char* QML_MAIN_FILE = "gui/loader.qml";

App::App(int &argc, char **argv) :
    CoreApp(argc, argv, false),
    mainScriptObject(nullptr),
    devList(nullptr),
    engine(nullptr),
    devsFound(false)
{
    singleInstance = false; // manually call the validation
    setQuitOnLastWindowClosed(false);
    QDir::setCurrent(applicationDirPath());
}

void App::openManual(const QString& uri)
{
    QString baseDir = getManualRootDir();
    if(baseDir.isEmpty())
        return;

    int hashPos = uri.indexOf('#');
    QString base = hashPos == -1 ? uri : uri.left(hashPos);
    if(base.isEmpty())
        base = "index";
    base.append(".html");
    QUrl url = QUrl::fromLocalFile(baseDir + base);
    if(hashPos >= 0)
        url.setFragment(uri.mid(hashPos));

    QDesktopServices::openUrl(url);
}

QString App::errorCodeToString(Err code)
{
    switch(code)
    {
        case Err::noPlugins: return "No plugins specified";
        case Err::noDevicesFound: return "No supported devices found";
        case Err::noDevicesLeft: return "No supported devices left";
        case Err::noDoc: return "Documentation not found";
        case Err::noTray: return "System tray is not available";
    }
    return QString();
}

QString App::getManualRootDir()
{
    static QString baseDir;
    static bool processed = false;

    if(!processed)
    {
        processed = true;

        QStringList roots;
        for(const QString& dir : qAsConst(sysDataDirs()))
            roots.append(dir + "/docs");
        roots.append(applicationDirPath() + "/docs");

        QStringList locales = QLocale::system().uiLanguages();
        QStringList tags;
        for(QString locale : qAsConst(locales))
        {
            forever
            {
                tags.append(locale);
                int p = locale.indexOf('-');
                if(p == -1)
                    break;
                locale = locale.left(p);
            }
        }
        if(!tags.contains("en"))
            tags.append("en"); // fallback

        for(const QString& tag : qAsConst(tags))
        {
            for(const QString& root : qAsConst(roots))
            {
                QFileInfo i(root + "/" + tag);
                qDebug() << "Searching for" << tag << "in" << root;
                if(i.isDir())
                {
                    baseDir = i.canonicalFilePath() + "/";
                    break;
                }
            }
            if(!baseDir.isEmpty())
                break;
        }

        if(baseDir.isEmpty())
            SETERROR_S(App, Err::noDoc);
        qDebug() << "Found docs:" << baseDir;
    }
    return baseDir;
}

void App::openEnabledPluginsForm()
{
    getEngine();
    QMetaObject::invokeMethod(
                mainScriptObject,
                "openEnabledPluginsForm",
                Q_ARG(QVariant, true));
}

QQmlApplicationEngine *App::getEngine()
{
    if(engine)
        return engine;

    // Can't specify a parent for QQmlApplicationEngine because
    // automatically removing QQmlApplicationEngine child in destructor
    // causes a futex deadlock for some reason.
    // Now the engine is manually removed in onQuit().
    QQmlApplicationEngine *_engine = new QQmlApplicationEngine();
    QQuickStyle::setStyle("Material");

    App::registerInQml();
    GpuDeviceList::registerInQml();
    GpuDevice::registerInQml();
    GpuDevicePlugin::registerInQml();
    CodeHighlighter::registerInQml();
    AppSettings::registerInQml();

    _engine->rootContext()->setContextProperty("APP", this);
    _engine->load(QML_MAIN_FILE);

    engine = _engine;
    return engine;
}

void App::showDevTab(GpuDevice *dev)
{
    QMutexLocker lock(&mutex);
    if(isQuitting())
        return;
    getEngine();
    emit onNeedShowDevTab(dev);
}

void App::showDevOptions(GpuDevice *dev)
{
    QMutexLocker lock(&mutex);
    if(isQuitting())
        return;
    getEngine();
    emit onNeedShowDevOptions(dev);
}

void App::setEnabledPlugins(const QStringList &list)
{
   GpuDevicePlugin::setEnabledList(list);
   emit onEnabledPluginsChange();
}

int App::main()
{
    loadTranslations();

    QCommandLineParser parser;
    parser.addOptions({{
        "plugins",
        tr("Specifies comma-separated list of plugins to use."),
        tr("plugins")
    },{
        "available-plugins",
        tr("Lists available plugins and their paths.")
    },{
        "enabled-plugins",
        tr("Lists enabled plugins and their paths.")
    },{
        "plugin-selector",
        tr("Shows a dialog for selecting plugins.")
    },{
        "html-help",
        tr("Shows the full manual in a web-browser.")
    },{
        "load-qml",
        tr("Loads all QML files then exits (for internal use only).")
    },{
        "dirs",
        tr("Lists all search directories.")
    }});
    parser.addVersionOption();
    parser.addHelpOption();
    parser.setApplicationDescription(
        tr("Control your video card fan speed using various rules and algorithms."));
    parser.process(*this);

    if(parser.isSet("html-help"))
    {
        openManual();
        quit();
        return 0;
    }

    if(parser.isSet("available-plugins"))
    {
        auto plugins = availablePlugins();
        for(auto& plugin : plugins)
            std::cout << plugin.name.toUtf8().constData() << ":" << plugin.dir.toUtf8().constData() << std::endl;
        quit();
        return 0;
    }

    if(parser.isSet("enabled-plugins"))
    {
        auto plugins = enabledPlugins();
        for(auto& plugin : plugins)
            std::cout << plugin.name.toUtf8().constData() << ":" << plugin.dir.toUtf8().constData() << std::endl;
        quit();
        return 0;
    }

    if(parser.isSet("dirs"))
    {
        for(auto& dir : sysDataDirs())
            std::cout << dir.toUtf8().constData() << std::endl;
        if(!isAppImage())
            std::cout << applicationDirPath().toUtf8().constData() << std::endl;
        quit();
        return 0;
    }

    setWindowIcon(QIcon(QStringLiteral("assets/")+applicationName()+".png"));

    if(parser.isSet("plugin-selector"))
    {
        // create a dummy devices list to avoid multiple checks in QML
        devList = new GpuDeviceList(this);
        openEnabledPluginsForm();
        return 0;
    }

    if(parser.isSet("load-qml"))
    {
        // create a dummy devices list to load all QML code
        devList = new GpuDeviceList(this);
        getEngine();
        quit();
        return 0;
    }

    int exitCode;
    if(!validateSingleInstance(&exitCode))
        return exitCode;

    QStringList plugins = parser.value("plugins").split(',', QString::SkipEmptyParts);
    CHECKE(!plugins.isEmpty(), Err::noPlugins);

    // ensure that tray icons can be created
    // do not check for message support though
    int nTrayTries = 5;
    forever
    {
        if(QSystemTrayIcon::isSystemTrayAvailable())
            break;
        nTrayTries--;
        CHECKE(nTrayTries, Err::noTray);
        qDebug() << "Waiting for a system tray to become available:" << nTrayTries << "tries left";
        QEventLoop loop;
        QTimer::singleShot(5000, &loop, &QEventLoop::quit);
        loop.exec();
    }

    devList = new GpuDeviceList(plugins, this);

    connect(devList, &GpuDeviceList::onListChanged, [this]{
        try{
            CHECKE(!devList->devices()->empty(),
                   devsFound ? Err::noDevicesLeft : Err::noDevicesFound);
        }catch(...){
            openEnabledPluginsForm();
        }
    });

    devsFound = !devList->devices()->empty();
    emit devList->onListChanged();

    connect(devList, &GpuDeviceList::onShowAppClick, this, &App::showDevTab);
    connect(devList, &GpuDeviceList::onShowDevOptionsClick, this, &App::showDevOptions);
    connect(devList, &GpuDeviceList::onShowManualClick, [this](const QString& uri, GpuDevice* dev){
        Q_UNUSED(dev);
        App::openManual(uri);
    });
    connect(devList, &GpuDeviceList::onQuitClick, this, &App::quit);

    return 0;
}

void App::onQuit()
{
    delete engine;
    engine = nullptr;
}

void App::onError(const ErrorManager::ErrorStruct &err)
{
    CoreApp::onError(err);
    if(!err.codeIs(CoreApp::Err::addTranslation))
        Notification::showDetached(err.logLine(), true);
}
