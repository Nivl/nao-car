
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

namespace std {
  template <class T1, class T2>
  pair<T1,T2> make_pair (T1 x, T2 y) {
    return std::pair<T1, T2>(x, y);
  }
}

#include "opencv2/opencv.hpp"
#include "libfreenect.hpp"
#include "libfreenect_sync.h"

using namespace cv;

class MyFreenectDevice : public Freenect::FreenectDevice {
public:
  MyFreenectDevice(freenect_context *_ctx, int _index);

  void	VideoCallback(void* _rgb, uint32_t timestamp);
  void	DepthCallback(void* _depth, uint32_t timestamp);
  bool	getVideo(Mat& output);
  bool	getDepth(Mat& output);
  void	initializeFloor();
  void	getObjects(std::vector<std::pair<std::pair<Point, Point>, double> > &to);

private:
  void  _checkObject(uint16_t* depth, int x, int y, int &minx, int &maxx, 
		     int &miny, int &maxy, int d, double &average, double& nb);

  std::vector<uint8_t> _buffer_depth;
  std::vector<uint8_t> _buffer_rgb;
  std::vector<uint16_t> _gamma;

  std::mutex _rgb_mutex;
  std::mutex _depth_mutex;
  bool _new_rgb_frame;
  bool _new_depth_frame;
  bool  _initialize;

  Mat depthMat;
  Mat rgbMat;
  Mat ownMat;


  double _averages[480];
  double _ecarts[480];

  std::mutex _objectsMutex;
  std::vector<std::pair<std::pair<Point, Point>, double> > _objects;

  char _dones[307200];
};

class AutoDriving {
public:
  AutoDriving();

  void start();
  void stop();
  void loop();

private:

  bool	_stop;
  std::thread*	_thread;
};

#endif
