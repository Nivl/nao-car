//
// AutoDriving.cpp
// for NaoCar Remote Server
//

#include <algorithm>
#include "AutoDriving.hpp"

using namespace cv;
using namespace std;

#ifdef NAO_LOCAL_COMPILATION
# define FLOOR_FILE "/home/nao/modules/RemoteServer/floor.depth"
#else
# define FLOOR_FILE "Modules/RemoteServer/Resources/floor.depth"
#endif

const double KinectDevice::ObjectTreshold = 15.0;
const double KinectDevice::DivisionWeightTreshold = 5.5e+07;
const double KinectDevice::DirectionWeightTreshold = 0.5e+07;

KinectDevice::KinectDevice(freenect_context* ctx, int index)
    : Freenect::FreenectDevice(ctx, index), _gamma(2048),
      _rgbMutex(), _depthMutex(),
      _newRgbFrame(false), _newDepthFrame(false),
      _calibrateFloor(false),
      _depthMat(Size(640,480), CV_8UC3),
      _rgbMat(Size(640,480), CV_8UC3,Scalar(0)),
      _averages(), _deviations(),
      _bestDirection(Front), _pushGazPedal(true)
{
    // Gamma array is used for rendering the depth buffer in RGB
    for( unsigned int i = 0 ; i < 2048 ; i++) {
        float v = i/2048.0;
        v = std::pow(v, 3)* 6;
        _gamma[i] = v*6*256;
    }

    // Read floor reference file
    std::ifstream file(FLOOR_FILE);
    if (file.is_open()) {
        file.read((char*)_averages, sizeof(_averages));
        file.read((char*)_deviations, sizeof(_deviations));
    }
}

void KinectDevice::VideoCallback(void* data, uint32_t) {
    _rgbMutex.lock();
    uint8_t* rgb = static_cast<uint8_t*>(data);
    _rgbMat.data = rgb;
    _newRgbFrame = true;
    _rgbMutex.unlock();
};

void KinectDevice::DepthCallback(void* data, uint32_t) {
    _depthMutex.lock();
    uint16_t* depth = static_cast<uint16_t*>(data);

    if (_calibrateFloor) {
        _floorCalibration(depth);
        _calibrateFloor = false;
    }

    // Detect objects (relatively to the saved floor)
    for (int i = 0; i < 480; ++i) {
        for (int j = 0; j < 640; ++j) {
            if (_isValidDepth(depth[i*640 + j])
                    && std::abs(depth[i * 640 + j] - _averages[i])
                        > ObjectTreshold*_deviations[i]) {
                // This is an object !
                depth[i * 640 + j] = (uint16_t)-1;
            } else {
                // No object here so safe zone !
            }
        }
    }

    // Calculate weight of several vertical divisions of the screen
    // The more "safe zone" are near the sensor, the bigger the weight is
    int nbDivs = 3;
    std::vector<double> divs(nbDivs);

    // For each screen division, calculate its weight
    for (int div = 0; div < nbDivs; ++div) {
        divs[div] = 0.0;
        for (int xDiv = 0; xDiv < 640/nbDivs; ++xDiv) {
            double x = div * (640/nbDivs) + xDiv;
            // Only check the lower part of the screen
            for (double y = 200; y < 480; ++y) {
                uint16_t value = depth[(int)y * 640 + (int)x];
                if (_isValidDepth(value) && value != (uint16_t)-1) {
                    // Nearest is best
                    divs[div] += (2047 - value);
                }
            }
        }
    }

    // Using that weights, determine the best direction
    // And wether or not to push the gaz pedal
    float left = divs[0], middle = divs[1], right = divs[2];

    _bestDirection = Front;
    _pushGazPedal = true;

    if (middle < DivisionWeightTreshold) {
        // No way. stop !
        _pushGazPedal = false;
    } else {
        // If one direction is under the treshold but the other is not,
        // try to evitate the obstacle !
        if (left < DivisionWeightTreshold && right > DivisionWeightTreshold) {
            _bestDirection = Right;
        } else if (right < DivisionWeightTreshold && left > DivisionWeightTreshold) {
            _bestDirection = Left;
        } else {
            // If no direction is critical, try to see if one direction is
            // better than middle
            if ((left - middle) > DirectionWeightTreshold || (right - middle) > DirectionWeightTreshold) {
                _bestDirection = left > right ? Left : Right;
            }
        }
    }

    std::cout << left << " " << middle << " " << right
              << " push: " << _pushGazPedal << ", dir: "
              << (_bestDirection == Left ? "left"
                                         : _bestDirection == Right ? "right"
                                                                   : "front")
              << std::endl;

    // Transform depth data to rgb values
    for (int i = 0; i < 640*480; ++i) {
        if (depth[i] == (uint16_t)-1) {
            _depthMat.data[3*i + 0] = 0;
            _depthMat.data[3*i + 1] = 0;
            _depthMat.data[3*i + 2] = 0;
        }
        else {
            int pval = _gamma[depth[i]];
            int lb = pval & 0xff;
            switch (pval>>8) {
            case 0:
                _depthMat.data[3*i+0] = 255;
                _depthMat.data[3*i+1] = 255-lb;
                _depthMat.data[3*i+2] = 255-lb;
                break;
            case 1:
                _depthMat.data[3*i+0] = 255;
                _depthMat.data[3*i+1] = lb;
                _depthMat.data[3*i+2] = 0;
                break;
            case 2:
                _depthMat.data[3*i+0] = 255-lb;
                _depthMat.data[3*i+1] = 255;
                _depthMat.data[3*i+2] = 0;
                break;
            case 3:
                _depthMat.data[3*i+0] = 0;
                _depthMat.data[3*i+1] = 255;
                _depthMat.data[3*i+2] = lb;
                break;
            case 4:
                _depthMat.data[3*i+0] = 0;
                _depthMat.data[3*i+1] = 255-lb;
                _depthMat.data[3*i+2] = 255;
                break;
            case 5:
                _depthMat.data[3*i+0] = 0;
                _depthMat.data[3*i+1] = 0;
                _depthMat.data[3*i+2] = 255-lb;
                break;
            default:
                _depthMat.data[3*i+0] = 0;
                _depthMat.data[3*i+1] = 0;
                _depthMat.data[3*i+2] = 0;
                break;
            }
        }
    }

    /*
    std::stringstream text;
    text << "Best: " << bestDiv;
    putText(_depthMat, text.str(), Point(320, 240), FONT_HERSHEY_SIMPLEX,
            1.0, Scalar(0, 0, 255));
    */

    _newDepthFrame = true;
    _depthMutex.unlock();
}

void KinectDevice::_floorCalibration(uint16_t* depth) {
    for (int line = 0; line < 480; ++line) {
        // Number of valid samples we will pick on the line
        double total = 0;

        _averages[line] = 0;
        _deviations[line] = 0;

        // Compute average of the line
        for (int i = 0; i < 640; ++i) {
            if (_isValidDepth(depth[line * 640 + i])) {
                total += 1;
                _averages[line] += depth[line * 640 + i];
             }
        }

        _averages[line] /= total;

        // Compute standard deviation of the line
        for (int i = 0; i < 640; ++i) {
            if (_isValidDepth(depth[line * 640 + i])) {
                _deviations[line] += ((depth[line * 640 + i] - _averages[line])
                                    * (depth[line * 640 + i] - _averages[line])) / total;
            }
        }
        _deviations[line] = sqrt(_deviations[line]);
    }

    // Save the values
    std::ofstream file(FLOOR_FILE);
    if (file.is_open()) {
        file.write((char*)_averages, sizeof(_averages));
        file.write((char*)_deviations, sizeof(_deviations));
        std::cout << "Floor calibration successfull" << std::endl;
    }
}

bool KinectDevice::_isValidDepth(uint16_t value) {
    return value != 0 && value != 2047;
}

bool KinectDevice::getVideo(Mat& output) {
    _rgbMutex.lock();
    if(_newRgbFrame) {
        cv::cvtColor(_rgbMat, output, CV_RGB2BGR);
        _newRgbFrame = false;
        _rgbMutex.unlock();
        return true;
    } else {
        _rgbMutex.unlock();
        return false;
    }
}

bool KinectDevice::getDepth(Mat& output) {
    _depthMutex.lock();
    if(_newDepthFrame) {
        _depthMat.copyTo(output);
        _newDepthFrame = false;
        _depthMutex.unlock();
        return true;
    } else {
        _depthMutex.unlock();
        return false;
    }
}

void KinectDevice::calibrateFloor(void) {
    _calibrateFloor = true;
}

KinectDevice::Direction KinectDevice::getBestDirection(void) {
    return (_bestDirection);
}

bool KinectDevice::getPushGazPedal(void) {
    return (_pushGazPedal);
}

AutoDriving::AutoDriving(StreamServer* ss, DriveProxy* driveProxy) : 
    _stop(true), _thread(NULL), _freenect(),
    _device(_freenect.createDevice<KinectDevice>(0)),
    _ss(ss), _driveProxy(driveProxy), _mode() {
}

AutoDriving::~AutoDriving(void) {
    stop();
}

void AutoDriving::start(Mode mode) {
    if (_thread == NULL) {
        _mode = mode;
        _stop = false;
        _thread = new std::thread(&AutoDriving::loop, this);
    }
}

void AutoDriving::loop(void) {

    Mat depthMat(Size(640,480), CV_8UC3);
    Mat rgbMat(Size(640,480), CV_8UC3, Scalar(0));

    _device.setTiltDegrees(-15);
    _device.startVideo();
    _device.startDepth();

    KinectDevice::Direction currentDirection = KinectDevice::Front;

    while (!_stop) {

        // Process each frame (we are not synced with the stream so maybe
        // we didn't processed the last frame)
        _device.getDepth(depthMat);

        if (1) { // Enable or disable actions depending on decisions
            // Decisions are made by the device, once the frame is processed,
            // apply the decisions
            KinectDevice::Direction dir = _device.getBestDirection();
            bool pushPedal = _device.getPushGazPedal();

            if (pushPedal) {
                if (_mode == Auto && !_driveProxy->isGasPedalPushed()) {
                    _driveProxy->pushPedal();
                }
            } else {
                if (_driveProxy->isGasPedalPushed()) {
                    // In Safe mode, release gaz pedal only if we try to go
                    // forward
                    if (_driveProxy->speed() == DriveProxy::Up) {
                        _driveProxy->releasePedal();
                    }
                }
            }
            if (_mode == Auto && dir != currentDirection) {
                if (dir == KinectDevice::Left) {
                    _driveProxy->turnLeft();
                } else if (dir == KinectDevice::Front) {
                    _driveProxy->turnFront();
                } else if (dir == KinectDevice::Right){
                    _driveProxy->turnRight();
                }
                currentDirection = dir;
            }
        }

        // Send depth image to the stream server
        vector<unsigned char> buf;
        imencode(".jpg", depthMat, buf);
        _ss->setOpencvData((char*)(&buf[0]), buf.size());

        usleep(200000);
    }

    _device.stopVideo();
    _device.stopDepth();

    return;
}

void AutoDriving::stop(void) {
    _stop = true;

    if (_thread) {
        _thread->join();
        delete _thread;
        _thread = NULL;
    }
}

void AutoDriving::calibration(void) {
    if (!_stop) {
        _device.calibrateFloor();
    }
}

bool AutoDriving::isStart(void) {
    return (!_stop);
}
