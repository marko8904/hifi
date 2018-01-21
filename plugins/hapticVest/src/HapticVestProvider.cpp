//
//  Created by Michael Perrotta on 2018/01/05
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include <mutex>

#include <QtCore/QObject>
#include <QtCore/QtPlugin>
#include <QtCore/QStringList>

#include <plugins/RuntimePlugin.h>
#include <plugins/InputPlugin.h>

#include "HapticVestManager.h"

class HapticVestProvider : public QObject, public InputProvider
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID InputProvider_iid FILE "hapticVest.json")
    Q_INTERFACES(InputProvider)

public:
    HapticVestProvider(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~HapticVestProvider() {}

    virtual InputPluginList getInputPlugins() override {
        static std::once_flag once;
        std::call_once(once, [&] {
            InputPluginPointer plugin(new HapticVestManager());
            if (plugin->isSupported()) {
                _inputPlugins.push_back(plugin);
            }
        });
        return _inputPlugins;
    }

    virtual void destroyInputPlugins() override {
        _inputPlugins.clear();
    }


private:
    InputPluginList _inputPlugins;
};

#include "HapticVestProvider.moc"
