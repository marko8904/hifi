//
//  Created by Bradley Austin Davis on 2016/01/08
//  Copyright 2013-2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_ScriptEngines_h
#define hifi_ScriptEngines_h

#include <functional>
#include <atomic>
#include <memory>

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QReadWriteLock>

#include <SettingHandle.h>
#include <DependencyManager.h>

#include "ScriptEngine.h"
#include "ScriptsModel.h"
#include "ScriptsModelFilter.h"

class ScriptEngine;

class ScriptEngines : public QObject, public Dependency {
    Q_OBJECT

    Q_PROPERTY(ScriptsModel* scriptsModel READ scriptsModel CONSTANT)
    Q_PROPERTY(ScriptsModelFilter* scriptsModelFilter READ scriptsModelFilter CONSTANT)
    Q_PROPERTY(QString debugScriptUrl READ getDebugScriptUrl WRITE setDebugScriptUrl)

public:
    using ScriptInitializer = std::function<void(ScriptEnginePointer)>;

    ScriptEngines(ScriptEngine::Context context);
    void registerScriptInitializer(ScriptInitializer initializer);

    void loadScripts();
    void saveScripts();

    QString getDebugScriptUrl() { return _debugScriptUrl; };
    void setDebugScriptUrl(const QString& url) { _debugScriptUrl = url; };

    void loadDefaultScripts();
    void reloadLocalFiles();

    QStringList getRunningScripts();
    ScriptEnginePointer getScriptEngine(const QUrl& scriptHash);

    ScriptsModel* scriptsModel() { return &_scriptsModel; };
    ScriptsModelFilter* scriptsModelFilter() { return &_scriptsModelFilter; };

    QString getDefaultScriptsLocation() const;

    Q_INVOKABLE void loadOneScript(const QString& scriptFilename);
    Q_INVOKABLE ScriptEnginePointer loadScript(const QUrl& scriptFilename = QString(),
        bool isUserLoaded = true, bool loadScriptFromEditor = false, bool activateMainWindow = false, bool reload = false);
    Q_INVOKABLE bool stopScript(const QString& scriptHash, bool restart = false);

    Q_INVOKABLE void reloadAllScripts();
    Q_INVOKABLE void stopAllScripts(bool restart = false);

    Q_INVOKABLE QVariantList getRunning();
    Q_INVOKABLE QVariantList getPublic();
    Q_INVOKABLE QVariantList getLocal();

    Q_PROPERTY(QString defaultScriptsPath READ getDefaultScriptsLocation)

    void defaultScriptsLocationOverridden(bool overridden) { _defaultScriptsLocationOverridden = overridden; };

    // Called at shutdown time
    void shutdownScripting();
    bool isStopped() const { return _isStopped; }

    void addScriptEngine(ScriptEnginePointer);

signals:
    void scriptCountChanged();
    void scriptsReloading();
    void scriptLoadError(const QString& filename, const QString& error);
    void printedMessage(const QString& message, const QString& engineName);
    void errorMessage(const QString& message, const QString& engineName);
    void warningMessage(const QString& message, const QString& engineName);
    void infoMessage(const QString& message, const QString& engineName);
    void errorLoadingScript(const QString& url);
    void clearDebugWindow();

public slots:
    void onPrintedMessage(const QString& message, const QString& scriptName);
    void onErrorMessage(const QString& message, const QString& scriptName);
    void onWarningMessage(const QString& message, const QString& scriptName);
    void onInfoMessage(const QString& message, const QString& scriptName);
    void onErrorLoadingScript(const QString& url);
    void onClearDebugWindow();

protected slots:
    void onScriptFinished(const QString& fileNameString, ScriptEnginePointer engine);

protected:
    friend class ScriptEngine;

    void reloadScript(const QString& scriptName) { loadScript(scriptName, true, false, false, true); }
    void removeScriptEngine(ScriptEnginePointer);
    void onScriptEngineLoaded(const QString& scriptFilename);
    void onScriptEngineError(const QString& scriptFilename);
    void launchScriptEngine(ScriptEnginePointer);

    ScriptEngine::Context _context;
    QReadWriteLock _scriptEnginesHashLock;
    QHash<QUrl, ScriptEnginePointer> _scriptEnginesHash;
    QSet<ScriptEnginePointer> _allKnownScriptEngines;
    QMutex _allScriptsMutex;
    std::list<ScriptInitializer> _scriptInitializers;
    ScriptsModel _scriptsModel;
    ScriptsModelFilter _scriptsModelFilter;
    std::atomic<bool> _isStopped { false };
    std::atomic<bool> _isReloading { false };
    bool _defaultScriptsLocationOverridden { false };
    QString _debugScriptUrl;
};

QUrl normalizeScriptURL(const QUrl& rawScriptURL);
QString expandScriptPath(const QString& rawPath);
QUrl expandScriptUrl(const QUrl& rawScriptURL);

#endif // hifi_ScriptEngine_h
