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
    return true;	//  TODO: return if BLE is supported
}

bool HapticVestManager::activate() {
    InputPlugin::activate();
    //checkForConnectedDevices();
    ConnectToHapticVest();
    //CreateBleController();
    //ConnectToHapticVest();
    return true;
}

void HapticVestManager::ConnectToHapticVest(){
    ListOfPorts = QSerialPortInfo::availablePorts();
    for (int i = 0; i < ListOfPorts.size(); i++) {
        qDebug() << "Haptic; port:" << ListOfPorts[i].portName();
    }
    serialPort = new QSerialPort(ListOfPorts[0]);
    serialPort->open(QIODevice::WriteOnly);
}


QByteArray HapticVestManager::TouchDevice::EncodeVibrationArray(int inputArray[]){
    //TODO: ensure that inputArray is of length numberOfMotors
    int numberOfMotors = 32;
    int bytesPerFrame = 20;
    QByteArray encodedByteArray;
    encodedByteArray.fill(0,bytesPerFrame);
    encodedByteArray[0] = 0x00;

    for (int i = 0; i < numberOfMotors; i += 2) {
        // Process two channels at a time
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
    SendByteArray(_touch->EncodeVibrationArray(offArray));
}

void HapticVestManager::SendByteArray(QByteArray byteArray){
    serialPort->write(byteArray);
}

void HapticVestManager::checkForConnectedDevices() {
    if (_touch) {
        return;
    }
 
    auto userInputMapper = DependencyManager::get<controller::UserInputMapper>();
    _touch = std::make_shared<TouchDevice>(*this);
    if(_touch) {
        _touch->initiateHapticLocations();
    }
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

//    if (_session) {
//        _session = nullptr;
//    }

    // unregister with UserInputMapper
    auto userInputMapper = DependencyManager::get<controller::UserInputMapper>();
    if (_touch) {
        userInputMapper->removeDevice(_touch->getDeviceID());
    }
}

void HapticVestManager::pluginUpdate(float deltaTime, const controller::InputCalibrationData& inputCalibrationData) {
    
    PerformanceTimer perfTimer("HapticVestManager::TouchDevice::update");
    //ConnectToHapticVest();
    
    checkForConnectedDevices();

    if (_touch) {
        // if (OVR_SUCCESS(ovr_GetInputState(_session, ovrControllerType_Touch, &_inputState))) {
             _touch->update(deltaTime, inputCalibrationData);
        // } else {
        //     qCWarning(oculus) << "Unable to read Oculus touch input state";
        // }
    }
}

void HapticVestManager::pluginFocusOutEvent() {
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
//        qDebug() << "Haptic Vest: Sending new haptic array (encoded): " << EncodeVibrationArray(hapticInputArray);
        for (int i = 0; i < 32; i++) {
            previousHapticArray[i] = hapticInputArray[i];
        }
    }
}

void HapticVestManager::TouchDevice::focusOutEvent() {
};

bool HapticVestManager::TouchDevice::triggerHapticPulse(float strength, float duration, int location) {
    qDebug() << "Haptic Vest: Trigger Haptic Pulse" << strength << duration << location;
    
    //  TODO: Convert strength (map (0,1?) -> (0,15))
    int convertedStrength = strength;
    convertedStrength = std::min(convertedStrength, 15);
    convertedStrength = std::max(convertedStrength, 0);
    //TODO: convert location (-3);
    // Haptic Vest Locations range from [3,34]
    if(location > 2 && location < 35) {
        location = location - 3;
        hapticLocations[location].strength = duration > hapticLocations[location].duration ? convertedStrength : hapticLocations[location].strength;
        hapticLocations[location].duration = std::max(hapticLocations[location].duration, duration);
    }
    
    return false;
}

//void HapticVestManager::TouchDevice::stopHapticPulse(bool leftHand) {
//    // ovr_SetControllerVibration(_parent._session, handType, 0.0f, 0.0f);
//}

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



