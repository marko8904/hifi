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

#include <QtBluetooth/QLowEnergyController>
#include <QtBluetooth/QBluetoothDeviceInfo>

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
//    checkForConnectedDevices();
    CreateBleController();
    ConnectToHapticVest();
    return true;
}

void HapticVestManager::CreateBleController(){
    //iOS and OSx use UUID instead of address; all others use address
//    QString address = "DA:D2:1B:34:32:0C";
//    QString UUID = "16019906-FB92-400E-8982-3D4FC6FAC492";
    QString UUID = "1555B349-D16E-45A0-BF33-BD4D8F434FC2";
    QString name = "CLEAR";
    qDebug() << "Haptic Vest connecting to: " << name << UUID;
    QBluetoothDeviceInfo* deviceInfo = new QBluetoothDeviceInfo(QBluetoothUuid(UUID),name,0);
    controller = QLowEnergyController::createCentral(*deviceInfo, this);
    
    // Setup slot for connected signal
    connect(controller, SIGNAL(connected()), this, SLOT(connected()));
}

void HapticVestManager::ConnectToHapticVest(){
    if(controller->state()==QLowEnergyController::ControllerState::UnconnectedState){
        controller->connectToDevice();
    }
}

void HapticVestManager::connected(){
    qDebug() << "Haptic Vest connected!";
    DiscoverServices();
}

void HapticVestManager::DiscoverServices(){
    // Setup slot for discoveryFinished signal
    connect(controller, SIGNAL(discoveryFinished()), this, SLOT(discoveryFinished()));

    controller->discoverServices();
}

void HapticVestManager::discoveryFinished(){
    QList<QBluetoothUuid> services = controller->services();

    for(QBluetoothUuid& service : services){
        qDebug() << "Haptic vest service: " << service;
    }
    QString serviceUUID = "713d0000-503e-4c75-ba94-3148f18d941e";
    service = controller->createServiceObject(QBluetoothUuid(serviceUUID));
    qDebug() << "Haptic vest service created: " << service->serviceUuid();
    
    connect(service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)), this, SLOT(stateChanged(QLowEnergyService::ServiceState)));
    
    service->discoverDetails();
}

void HapticVestManager::stateChanged(QLowEnergyService::ServiceState newState){
    if(newState!=QLowEnergyService::ServiceDiscovered){
        return;
    }
    writeCharacteristic = (service->characteristics())[0];
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
    int numberOfMotors = 32;
    int offArray[numberOfMotors];
    for (int i = 0; i < numberOfMotors; i++) {
        offArray[i] = 0;
    }
    SendByteArray(_touch->EncodeVibrationArray(offArray));
}

void HapticVestManager::SendByteArray(QByteArray byteArray){
    if(service){
        service->writeCharacteristic(writeCharacteristic, byteArray, QLowEnergyService::WriteWithoutResponse);
    } else {
        qDebug() << "Haptic: trying to send but no service yet";
    }
}

void HapticVestManager::checkForConnectedDevices() {
    if (_touch) {
        return;
    }
 
    auto userInputMapper = DependencyManager::get<controller::UserInputMapper>();
    _touch = std::make_shared<TouchDevice>(*this);
    if(_touch) {
        qDebug() << "Haptic " << _touch->hapticLocations[0].duration;
        _touch->hapticLocations[0].duration = 1;
        qDebug() << "Haptic " << _touch->hapticLocations[0].duration;
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
    ConnectToHapticVest();
    
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
    
    if(!_parent.service){return;}
    
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

bool HapticVestManager::TouchDevice::triggerHapticPulse(float strength, float duration, controller::Hand location) {
    qDebug() << "Haptic Vest: Trigger Haptic Pulse" << strength << duration << location;
    
    //  TODO: Convert strength (map (0,1?) -> (0,15))
    int convertedStrength = strength;
    convertedStrength = std::min(convertedStrength, 15);
    convertedStrength = std::max(convertedStrength, 0);
    //TODO: convert location (-3);
    int _location = std::rand() % 32;
    qDebug() << "Haptic: location: " << _location;
    // Haptic Vest Locations range from [3,34]
//    if(_location > 2 && _location < 35) {
        hapticLocations[_location].strength = duration > hapticLocations[_location].duration ? convertedStrength : hapticLocations[_location].strength;
        hapticLocations[_location].duration = std::max(hapticLocations[_location].duration, duration);
//    }
    
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



