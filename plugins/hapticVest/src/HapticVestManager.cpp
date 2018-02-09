//
//  OculusControllerManager.cpp
//  input-plugins/src/input-plugins
//
//  Created by Bradley Austin Davis 2016/03/04.
//  Copyright 2013-2016 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include "HapticVestManager.h"

#include <QtCore/QLoggingCategory>

#include <ui-plugins/PluginContainer.h>
#include <controllers/UserInputMapper.h>
#include <controllers/StandardControls.h>

#include <PerfStat.h>
#include <PathUtils.h>
#include <NumericalConstants.h>
#include <StreamUtils.h>


// #include "HapticVestHelpers.h"

Q_DECLARE_LOGGING_CATEGORY(hapticVest)


//static const char* MENU_PARENT = "Avatar";
//static const char* MENU_NAME = "Haptic Vest";
//static const char* MENU_PATH = "Avatar" ">" "Haptic Vest";

const char* HapticVestManager::NAME = "Haptic Vest";

//const quint64 LOST_TRACKING_DELAY = 3000000;

bool HapticVestManager::isSupported() const {
    return true;
}

bool HapticVestManager::activate() {
    InputPlugin::activate();
    ConnectToHapticVest();
    return true;
}

void HapticVestManager::ConnectToHapticVest(){
    ListOfPorts = QSerialPortInfo::availablePorts();
    //TODO: Pick appropriate port (rather than picking first available port)
    if (ListOfPorts.size() > 0) {
        serialPort = new QSerialPort(ListOfPorts[0]);
        if (serialPort) {
            serialPort->open(QIODevice::WriteOnly);
            connect(serialPort, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(serialPortErrorOccurred(QSerialPort::SerialPortError)));
        }
    }
}

void HapticVestManager::serialPortErrorOccurred(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError && serialPort) {
        serialPort->close();
        serialPort->open(QIODevice::WriteOnly);
    }
    //serialPort = nullptr;
}

QByteArray HapticVestManager::TouchDevice::EncodeVibrationArray(int inputArray[]){
    //TODO: ensure that inputArray is of length numberOfMotors
    int numberOfMotors = 32;
    int bytesPerFrame = 20;
    QByteArray encodedByteArray;
    encodedByteArray.fill(0,bytesPerFrame);
    encodedByteArray[0] = 0x00;

    for (int i = 0; i < numberOfMotors; i += 2) {
        // Combine two motor intensities into one byte per protocol
        encodedByteArray [i / 2 + 1] = ((inputArray [i]) << 4) | (inputArray [i + 1] & 0x0F);
    }
    return encodedByteArray;
}

void HapticVestManager::TurnOffAllMotors(){
    const int numberOfMotors = 32;
    int offArray[numberOfMotors];
    for (int i = 0; i < numberOfMotors; i++) {
        offArray[i] = 0;
    }
    if (_touch) {
        SendByteArray(_touch->EncodeVibrationArray(offArray));
    }
}

void HapticVestManager::SendByteArray(QByteArray byteArray){
    if (serialPort) {
        if (serialPort->isOpen()) {
            serialPort->write(byteArray);
        }
        else {
            serialPort->open(QIODevice::WriteOnly);
        }
    }
}

void HapticVestManager::checkForConnectedDevices() {
    if (!serialPort) {
        ConnectToHapticVest();
    }

    if (_touch) {
        return;
    }

 
    auto userInputMapper = DependencyManager::get<controller::UserInputMapper>();
    _touch = std::make_shared<TouchDevice>(*this);
    _touch->initiateHapticLocations();
    userInputMapper->registerDevice(_touch);
}

void HapticVestManager::TouchDevice::initiateHapticLocations(){
    for (int i = 0; i < 32; i++) {
        hapticLocations[i].duration = 0;
        hapticLocations[i].strength = 0;
    }
}

void HapticVestManager::deactivate() {
    InputPlugin::deactivate();

    // unregister with UserInputMapper
    auto userInputMapper = DependencyManager::get<controller::UserInputMapper>();
    if (_touch) {
        userInputMapper->removeDevice(_touch->getDeviceID());
    }
    if (serialPort) {
        serialPort->close();
    }
}

void HapticVestManager::pluginUpdate(float deltaTime, const controller::InputCalibrationData& inputCalibrationData) {
    
    PerformanceTimer perfTimer("HapticVestManager::TouchDevice::update");
    //ConnectToHapticVest();
    
    checkForConnectedDevices();

    if (_touch) {
        _touch->update(deltaTime, inputCalibrationData);
    }
}

void HapticVestManager::pluginFocusOutEvent() {
    TurnOffAllMotors();
}

QStringList HapticVestManager::getSubdeviceNames() {
    QStringList devices;
    if (_touch) {
        devices << _touch->getName();
    }
    return devices;
}

void HapticVestManager::TouchDevice::update(float deltaTime,
                                                  const controller::InputCalibrationData& inputCalibrationData) {
    
    
    bool isDifferentThanPreviousArray = false;
    int hapticInputArray[32];
    for (int i = 0; i < 32; i++) {
        if (hapticLocations[i].duration > 0) {
            hapticLocations[i].duration -= deltaTime;
        }
        hapticInputArray[i] = hapticLocations[i].duration > 0 ? hapticLocations[i].strength : 0;
        if(hapticInputArray[i] != previousHapticArray[i]) {
            isDifferentThanPreviousArray = true;
        }
    }
    if(isDifferentThanPreviousArray){
        _parent.SendByteArray(EncodeVibrationArray(hapticInputArray));
        for (int i = 0; i < 32; i++) {
            previousHapticArray[i] = hapticInputArray[i];
        }
    }
}

void HapticVestManager::TouchDevice::focusOutEvent() {
};

//TODO: this should be decoupled from haptic pulse; although it is a haptic pulse, it is for the haptic vest specifically and does not need to be an override of the haptic pulse method that generalizes to any controller
bool HapticVestManager::TouchDevice::triggerHapticPulse(float strength, float duration, int location) {
    
    int convertedStrength = std::round(strength * 15);
    convertedStrength = std::min(convertedStrength, 15);
    convertedStrength = std::max(convertedStrength, 0);

    // Haptic Vest Locations range from [3,34]
    if(location > 2 && location < 35) {
        location -= 3;
        hapticLocations[location].strength = duration > hapticLocations[location].duration ? convertedStrength : hapticLocations[location].strength;
        hapticLocations[location].duration = std::max(hapticLocations[location].duration, duration);
    }
    
    return false;
}

controller::Input::NamedVector HapticVestManager::TouchDevice::getAvailableInputs() const {
    using namespace controller;
    QVector<Input::NamedPair> availableInputs{
    };
    return availableInputs;
}

QString HapticVestManager::TouchDevice::getDefaultMappingConfig() const {
    static const QString MAPPING_JSON = "";
    return MAPPING_JSON;
}



