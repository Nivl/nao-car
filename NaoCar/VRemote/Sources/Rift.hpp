//
// Rift.hpp
// NaoCar Remote
//

#ifndef _RIFT_HPP_
#define _RIFT_HPP_

#include "OVR.h"

using namespace OVR;

class Rift {
public:
    Rift(void);
    ~Rift(void);
    
    Vector3f getOrientation(void);
    
private:
    System              _system;
    Ptr<DeviceManager>  _manager;
    Ptr<HMDDevice>      _HMD;
    Ptr<SensorDevice>   _sensor;
    SensorFusion        _sensorFusion;
};

#endif
