//
// Rift.cpp
// NaoCar Remote
//

#include <string>
#include <iostream>
#include "Rift.hpp"

Rift::Rift(void)
:
_delegate(NULL),
_system(), _manager(), _HMD(), _sensor(),
_timer(),
_view(NULL) {
    
    _manager = *OVR::DeviceManager::Create();
    _HMD = *_manager->EnumerateDevices<HMDDevice>().CreateDevice();
    
    if (!_HMD) {
        std::cout << "Error: cannot find any connected Oculus Rift" << std::endl;
        return;
    }
    
    _sensor = *_HMD->GetSensor();
    _sensorFusion.AttachToSensor(_sensor);
    
    // Configure timer
    QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(_update()));
    _timer.setInterval(50);
    _timer.start();
    
    // Create the VR view
    _view = new VRView(_HMD);
}

Rift::~Rift() {
    _timer.stop();
    if (_view) {
        delete _view;
    }
}

void Rift::setDelegate(RiftDelegate *delegate) {
    _delegate = delegate;
}

Vector3f Rift::getOrientation(void) {
    // We extract Yaw, Pitch, Roll from the sensor orientation
    Quatf hmdOrient = _sensorFusion.GetOrientation();
    float yaw = 0.0f, pitch = 0.0f, roll = 0.0f;
    hmdOrient.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&yaw, &pitch, &roll);
    return Vector3f(yaw, pitch, roll);
}

void Rift::setViewImage(const QImage& image) {
    if (_view) {
        _view->setViewImage(image);  
    }
}

QWidget* Rift::getView(void) {
    return _view;
}

void Rift::_update(void) {
    if (_delegate) {
        _delegate->riftOrientationUpdate(getOrientation());
    }
    if (_view) {
        _view->repaint();
    }
}
