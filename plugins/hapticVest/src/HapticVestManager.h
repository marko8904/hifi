//
//  Created by Bradley Austin Davis on 2016/03/04
//  Copyright 2013-2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi__HapticVestManager
#define hifi__HapticVestManager

#include <QObject>
#include <unordered_set>
#include <map>

#include <GLMHelpers.h>

#include <controllers/InputDevice.h>
#include <plugins/InputPlugin.h>

#include <QtBluetooth/QLowEnergyController>

class HapticVestManager : public InputPlugin {
    Q_OBJECT
public:
    // Plugin functions
    bool isSupported() const override;
    const QString getName() const override { return NAME; }

    QStringList getSubdeviceNames() override;

    bool activate() override;
    void deactivate() override;

    void pluginFocusOutEvent() override;
    void pluginUpdate(float deltaTime, const controller::InputCalibrationData& inputCalibrationData) override;

private slots:
    void connected();
    void discoveryFinished();
    void stateChanged(QLowEnergyService::ServiceState newState);
    
private:
    class HapticVestInputDevice : public controller::InputDevice {
    public:
        HapticVestInputDevice(HapticVestManager& parent, const QString& name) : controller::InputDevice(name), _parent(parent) {}

        HapticVestManager& _parent;
        friend class HapticVestManager;
    };

    class TouchDevice : public HapticVestInputDevice {
    public:
        using Pointer = std::shared_ptr<TouchDevice>;
        TouchDevice(HapticVestManager& parent) : HapticVestInputDevice(parent, "HapticVestTouch") {}

        controller::Input::NamedVector getAvailableInputs() const override;
        QString getDefaultMappingConfig() const override;
        void update(float deltaTime, const controller::InputCalibrationData& inputCalibrationData) override;
        void focusOutEvent() override;

        bool triggerHapticPulse(float strength, float duration, controller::Hand location) override;

    private:
        void stopHapticPulse(bool leftHand);
        
        // perform an action when the TouchDevice mutex is acquired.
        using Locker = std::unique_lock<std::recursive_mutex>;
        template <typename F>
        void withLock(F&& f) { Locker locker(_lock); f(); }

        class HapticLocation {
        public:
            float strength;
            float duration;
        };
        HapticLocation hapticLocations[32];
        int previousHapticArray[32];
        void initiateHapticLocations();
        QByteArray EncodeVibrationArray(int inputArray[]);
        
        mutable std::recursive_mutex _lock;

        friend class HapticVestManager;
    };
    
    
    
    void CreateBleController();
    void ConnectToHapticVest();
    QLowEnergyController* controller;
    void DiscoverServices();
    QLowEnergyService* service;
    QLowEnergyCharacteristic writeCharacteristic;
    void TurnOffAllMotors();
    void SendByteArray(QByteArray byteArray);
    
    void checkForConnectedDevices();
    TouchDevice::Pointer _touch;
    static const char* NAME;
};

#endif // hifi__HapticVestManager
