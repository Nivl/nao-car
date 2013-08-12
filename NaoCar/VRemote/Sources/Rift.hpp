#include "OVR.h"
using namespace OVR;

#ifndef RIFT_HPP
#define RIFT_HPP

class Rift {
    public:
        Rift();
        ~Rift();

    private:
        Ptr<DeviceManager> _pManager;
        Ptr<HMDDevice> _pHMD;
        Ptr<SensorDevice> _pSensor;
};

#endif // RIFT_HPP
