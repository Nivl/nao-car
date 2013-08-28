//
// AutoDriving.hpp
// for NaoCar Remote Server
//

#ifndef _AUTO_DRIVING_HPP_
# define _AUTO_DRIVING_HPP_

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <sstream>
#include <mutex>
#include <iomanip>
#include <thread>

#include "DriveProxy.hpp"

namespace std {
template <class T1, class T2>
pair<T1,T2> make_pair (T1 x, T2 y) {
    return std::pair<T1, T2>(x, y);
}
}

#include "opencv2/opencv.hpp"
#include "libfreenect.hpp"
#include "libfreenect_sync.h"
#include "StreamServer.hpp"

using namespace cv;

class KinectDevice : public Freenect::FreenectDevice {
public:

    // Treshold for object detection
    // Higher value means less objects detected
    static const double ObjectTreshold;
    static const double DivisionWeightTreshold;
    static const double DirectionWeightTreshold;

    enum Direction {
        Left = -1,
        Front = 0,
        Right = 1
    };

    KinectDevice(freenect_context* ctx, int index);

    void	VideoCallback(void* rgb, uint32_t timestamp);
    void	DepthCallback(void* depth, uint32_t timestamp);

    bool	getVideo(Mat& output);
    bool	getDepth(Mat& output);

    void    calibrateFloor(void);

    Direction   getBestDirection(void);
    bool        getPushGazPedal(void);

private:    
    void _floorCalibration(uint16_t* depth);
    bool _isValidDepth(uint16_t value);

    std::vector<uint16_t> _gamma;

    std::mutex _rgbMutex;
    std::mutex _depthMutex;
    bool _newRgbFrame;
    bool _newDepthFrame;
    bool _calibrateFloor;

    Mat _depthMat;
    Mat _rgbMat;

    double _averages[480];
    double _deviations[480];

    std::mutex _objectsMutex;
    std::vector<std::pair<std::pair<Point, Point>, double> > _objects;

    Direction   _bestDirection;
    bool        _pushGazPedal;
};

class AutoDriving {
public:
    enum Mode {
        Safe,
        Auto
    };

    AutoDriving(StreamServer* ss, DriveProxy* driveProxy);
    ~AutoDriving(void);

    void start(Mode mode);
    void stop(void);
    void loop(void);
    void calibration(void);

    bool isStart(void);

private:

    std::atomic<bool>	_stop;
    std::thread*		_thread;
    Freenect::Freenect	_freenect;
    KinectDevice&       _device;
    StreamServer*       _ss;
    DriveProxy*         _driveProxy;
    Mode                _mode;
};

#endif
