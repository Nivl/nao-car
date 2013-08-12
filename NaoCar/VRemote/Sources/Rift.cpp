//
// Rift.cpp
// NaoCar Remote
//

#include <string>
#include <iostream>
#include "Rift.hpp"

Rift::Rift(void)
: _system(), _manager(), _HMD(), _sensor() {

    _manager = *OVR::DeviceManager::Create();
    _HMD = *_manager->EnumerateDevices<HMDDevice>().CreateDevice();
   
    if (_HMD) {
        _sensor = *_HMD->GetSensor();
        _sensorFusion.AttachToSensor(_sensor);
    } else
        std::cout << "Error on enumerate devices" << std::endl;
}

Rift::~Rift() {
}

Vector3f Rift::getOrientation(void) {
    // We extract Yaw, Pitch, Roll instead of directly using the orientation
    Quatf hmdOrient = _sensorFusion.GetOrientation();
    float yaw = 0.0f, pitch = 0.0f, roll = 0.0f;
    hmdOrient.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);
    return Vector3f(yaw, pitch, roll);
}
