#include <string>
#include <iostream>
#include "Rift.hpp"

Rift::Rift() {
    System::Init(Log::ConfigureDefaultLog(LogMask_All));

    _pManager = *DeviceManager::Create();
    _pHMD = *_pManager->EnumerateDevices<HMDDevice>().CreateDevice();
    if (_pHMD)
    {
        _pSensor = *_pHMD->GetSensor();

        HMDInfo hmd;
        if (_pHMD->GetDeviceInfo(&hmd))
        {
            std::string MonitorName = hmd.DisplayDeviceName;
            int EyeDistance = hmd.InterpupillaryDistance;
            int DistortionK[4];
            DistortionK[0] = hmd.DistortionK[0];
            DistortionK[1] = hmd.DistortionK[1];
            DistortionK[2] = hmd.DistortionK[2];
            DistortionK[3] = hmd.DistortionK[3];
        }
    } else
        std::cout << "Error on enumerate devices" << std::endl;
}

Rift::~Rift() {
    System::Destroy();
}
