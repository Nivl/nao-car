//
// AutoDriving.cpp
// for NaoCar Remote Server
//

#include "AutoDriving.hpp"

using namespace cv;
using namespace std;


#ifdef NAO_LOCAL_COMPILATION
# define FLOOR_FILE "/home/nao/modules/RemoteServer/floor.depth"
#else
# define FLOOR_FILE "Modules/RemoteServer/Resources/floor.depth"
#endif

MyFreenectDevice::MyFreenectDevice(freenect_context *_ctx, int _index)
  : Freenect::FreenectDevice(_ctx, _index), _buffer_depth(FREENECT_DEPTH_11BIT),_buffer_rgb(FREENECT_VIDEO_RGB), _gamma(2048), _new_rgb_frame(false), _new_depth_frame(false),
    depthMat(Size(640,480),CV_8UC3), rgbMat(Size(640,480),CV_8UC3,Scalar(0)), ownMat(Size(640,480),CV_8UC3,Scalar(0))
{
  for( unsigned int i = 0 ; i < 2048 ; i++) {
    float v = i/2048.0;
    v = std::pow(v, 3)* 6;
    _gamma[i] = v*6*256;
    }
  _initialize = false;
  
  std::ifstream file(FLOOR_FILE);
  file.read((char*)_averages, sizeof(_averages));
  file.read((char*)_ecarts, sizeof(_ecarts));
}

void MyFreenectDevice::VideoCallback(void* _rgb, uint32_t) {
  _rgb_mutex.lock();
  uint8_t* rgb = static_cast<uint8_t*>(_rgb);
  rgbMat.data = rgb;
    _new_rgb_frame = true;
    _rgb_mutex.unlock();
};

void MyFreenectDevice::DepthCallback(void* _depth, uint32_t) {
  _depth_mutex.lock();
  uint16_t* depth = static_cast<uint16_t*>(_depth);
  
  if (_initialize) {
    double averages[480];
    double ecart[480];
    for (int i=0; i<480; i++) {
      double total = 0;
      averages[i] = 0;
      ecart[i] = 0;
      // COMPUTE TOTAL NOISE
      for (int j=0; j<640; j++) {
	if (depth[i * 640 + j] != 0 && depth[i * 640 + j] != 2047)
	  total += 1;
      }
      
      // COMPUTE AVERAGE
      int test = 0;
      for (int j=0; j<640; j++) {
	if (depth[i * 640 + j] != 0 && depth[i * 640 + j] != 2047) {
	  averages[i] += depth[i * 640 + j] / total;
	  test += 1;
	  }
      }
      
      // COMPUTE STANDARD DEVIATION 
      for (int j=0; j<640; j++) {
	if (depth[i * 640 + j] != 0 && depth[i * 640 + j] != 2047)
	  ecart[i] += (depth[i * 640 + j] - averages[i]) * (depth[i * 640 + j] - averages[i]) / total;
      }
      ecart[i] = sqrt(ecart[i]);
    }
    std::ofstream file(FLOOR_FILE);
    
    //for (int i=0; i<480; i++)
    //file << averages[i] << " " << ecart[i] << "\n";
    file.write((char*)averages, sizeof(averages));
    file.write((char*)ecart, sizeof(ecart));
    _initialize = false;
  }
  
  // DETECT OBJECT
  for (int i=0; i<480; i++) {
    for (int j=0; j<640; j++) {
      if (depth[i * 640 + j] != 0 && depth[i * 640 + j] != 2047 &&
	  (depth[i * 640 + j] > _averages[i] + 10 * _ecarts[i] || depth[i * 640 + j] < _averages[i] - 10 * _ecarts[i]))
	;//rgbMat.data[3 * i * 640 + j * 3] = 125;//depth[i * 640 + j] = -1;
      else
	depth[i * 640 + j] = -1;
    }
  }
  
  // LIST ALL OBJECTS
  _objectsMutex.lock();
  _objects.clear();
  
  
  memset(_dones, 0, sizeof(_dones));
  for (int i=0; i<480; i++) {
    for (int j=0; j<640; j++) {
      if (_dones[i * 640 + j] == 0) {
	if (depth[i * 640 + j] != (uint16_t)-1) {
	  int minx=i, maxx=i, miny=j, maxy=j;
	  double average = 0.0, nb = 0;
	  _checkObject(depth, i, j, minx, maxx, miny, maxy, 0, average, nb);
	  
	  bool colapse = false;
	  for (unsigned int kk=0;kk < _objects.size();++kk) {
	    if (_objects[kk].first.first.x <= miny && _objects[kk].first.second.x >= maxy &&
		_objects[kk].first.first.y <= minx && _objects[kk].first.second.y >= maxx)
	      colapse = true;
	  }
	  if (!colapse && (maxx - minx) * (maxy - miny) > 10)
	    {
	      //std::cout << average << std::endl;;
	      _objects.push_back(std::pair<std::pair<Point, Point>, double>(std::pair<Point, Point>(Point(miny, minx), Point(maxy, maxx)), average));
	    }
	}
      }
    }
  }
  _objectsMutex.unlock();
  
  //_objects.push_back(std::pair<std::pair<Point, Point>, double>(std::pair<Point, Point>(Point(200, 200), Point(50, 50)), 1000.));
  
  for (int i=0; i<640*480; i++) {
    if (depth[i] == (uint16_t)-1) {
      depthMat.data[3*i+0] = 0;
      depthMat.data[3*i+1] = 0;
      depthMat.data[3*i+2] = 0;
    }
    // else if (depth[i] == 0) {
    // 	  depthMat.data[3*i+0] = 255;
    // 	  depthMat.data[3*i+1] = 0;
    // 	  depthMat.data[3*i+2] = 0;
    // }
    else {
      int pval = _gamma[depth[i]];//depth[i]];
      int lb = pval & 0xff;
      switch (pval>>8) {
      case 0:
	depthMat.data[3*i+0] = 255;
	depthMat.data[3*i+1] = 255-lb;
	depthMat.data[3*i+2] = 255-lb;
	break;
      case 1:
	depthMat.data[3*i+0] = 255;
	depthMat.data[3*i+1] = lb;
	depthMat.data[3*i+2] = 0;
	break;
      case 2:
	depthMat.data[3*i+0] = 255-lb;
	depthMat.data[3*i+1] = 255;
	depthMat.data[3*i+2] = 0;
	break;
      case 3:
	depthMat.data[3*i+0] = 0;
	depthMat.data[3*i+1] = 255;
	depthMat.data[3*i+2] = lb;
	break;
      case 4:
	depthMat.data[3*i+0] = 0;
	depthMat.data[3*i+1] = 255-lb;
	depthMat.data[3*i+2] = 255;
	break;
      case 5:
	depthMat.data[3*i+0] = 0;
	depthMat.data[3*i+1] = 0;
	depthMat.data[3*i+2] = 255-lb;
	break;
      default:
	//std::cout << averages[i / 640] << " " << (pval >> 8) << " " << lb << std::endl;
	depthMat.data[3*i+0] = 0;
	depthMat.data[3*i+1] = 0;
	depthMat.data[3*i+2] = 0;
	break;
      }
    }
    // if (depth[i] == 2047 || depth[i] == 0)
    // 	{
    // 	  depthMat.data[3*i+0]  = depthMat.data[3*i+1] = 0;
    // 	  depthMat.data[3*i+2] = 255;
    // 	}
    // else
    // 	{
    // 	  depthMat.data[3*i+0] = int(depth[i] / 2048. * 255.);
    // 	  depthMat.data[3*i+1] = depthMat.data[3*i+2] = 0;
    // 	}
  }
  
  //depthMat.data = (uchar*) depth;
  _new_depth_frame = true;
  _depth_mutex.unlock();
}

bool MyFreenectDevice::getVideo(Mat& output) {
  _rgb_mutex.lock();
  if(_new_rgb_frame) {
      cv::cvtColor(rgbMat, output, CV_RGB2BGR);
      _new_rgb_frame = false;
      _rgb_mutex.unlock();
      return true;
  } else {
    _rgb_mutex.unlock();
    return false;
    }
}

bool MyFreenectDevice::getDepth(Mat& output) {
  _depth_mutex.lock();
  if(_new_depth_frame) {
    depthMat.copyTo(output);
    _new_depth_frame = false;
    _depth_mutex.unlock();
    return true;
  } else {
    _depth_mutex.unlock();
    return false;
  }
}

void MyFreenectDevice::initializeFloor() {
  _initialize = true;
}

void MyFreenectDevice::getObjects(std::vector<std::pair<std::pair<Point, Point>, double> > &to) {
  _objectsMutex.lock();
  to = _objects;
  _objectsMutex.unlock();
}

void MyFreenectDevice::_checkObject(uint16_t* depth, int x, int y, int &minx, int &maxx, int &miny, int &maxy, int d, double &average, double& nb) {
  if (x < 0 || y < 0 || x > 480 || y > 640 || _dones[x * 640 + y] != 0 || depth[x * 640 + y] == (uint16_t)-1 || d > 20000)
    return;
  
  _dones[x * 640 + y] = 1;
  average *= nb;
  
  average += depth[x * 640 + y];
  nb += 1;
  average /= nb;
  
  if (x < minx)
    minx = x;
  if (x > maxx)
    maxx = x;
  if (y < miny)
    miny = y;
  if (y > maxy)
    maxy = y;
  
  _checkObject(depth, x + 1, y, minx, maxx, miny, maxy, d+1, average, nb);
  _checkObject(depth, x - 1, y, minx, maxx, miny, maxy, d+1, average, nb);
  _checkObject(depth, x + 1, y + 1, minx, maxx, miny, maxy, d+1, average, nb);
  _checkObject(depth, x + 1, y - 1, minx, maxx, miny, maxy, d+1, average, nb);
  _checkObject(depth, x - 1, y + 1, minx, maxx, miny, maxy, d+1, average, nb);
  _checkObject(depth, x - 1, y - 1, minx, maxx, miny, maxy, d+1, average, nb);
  _checkObject(depth, x, y - 1, minx, maxx, miny, maxy, d+1, average, nb);
  _checkObject(depth, x, y + 1, minx, maxx, miny, maxy, d+1, average, nb);
  
}

AutoDriving::AutoDriving() {

}

void AutoDriving::start() {
  _stop = false;

  _thread = new std::thread(&AutoDriving::loop, this);
}

void AutoDriving::loop() {
  
  Mat depthMat(Size(640,480),CV_8UC3);
  Mat depthf(Size(640,480),CV_8UC1);
  Mat rgbMat(Size(640,480),CV_8UC3,Scalar(0));
  Mat ownMat(Size(640,480),CV_8UC3,Scalar(0));

  int tilt = -20;

  Freenect::Freenect freenect;
  MyFreenectDevice& device = freenect.createDevice<MyFreenectDevice>(0);

  device.setTiltDegrees(tilt);
  device.startVideo();
  device.startDepth();
  while (!_stop) {
    device.getVideo(rgbMat);
    device.getDepth(depthMat);
    //depthMat.convertTo(depthf, CV_8UC1, 255.0/2048.0);

    // DRAW SQUARES AROUND OBJECTS
    std::vector<std::pair<std::pair<Point, Point>, double> > objects;
    device.getObjects(objects);
    
    std::cout << "----BEGIN----" << std::endl;
    for (unsigned int k = 0; k < objects.size(); ++k)
      {
	rectangle(rgbMat, objects[k].first.first, objects[k].first.second, Scalar(10, 10, 163), 2);
	rectangle(depthMat, objects[k].first.first, objects[k].first.second, Scalar(10, 10, 163), 2);
	std::stringstream ss;
	ss << std::setprecision(3) << objects[k].second * 2. / 1000. << "m";
	putText(depthMat, ss.str(), Point(objects[k].first.first.x + 2, objects[k].first.first.y + 10), FONT_HERSHEY_SIMPLEX, 0.3, Scalar(10, 10, 163));
	std::cout << objects[k].first.first << " " << objects[k].first.second << " " << ss.str() << std::endl;
      }
    std::cout << "----END----" << std::endl;

    //cv::imshow("rgb", rgbMat);
    //cv::imshow("depth",depthMat);

    // char k = cvWaitKey(5);
    // if( k == 27 ){
    //   cvDestroyWindow("rgb");
    //   cvDestroyWindow("depth");
    //   break;
    // }
    // if( k == 8 ) {
    //   std::ostringstream file;
    //   file << filename << i_snap << suffix;
    //   cv::imwrite(file.str(),rgbMat);
    //   i_snap++;
    // }
    // if (k == 'i')
    //   device.initializeFloor();
    // else if (k == 'w') {
    //   tilt += 1;
    //   device.setTiltDegrees(tilt);
    // }
    // else if (k == 's') {
    //   tilt -= 1;
    //   device.setTiltDegrees(tilt);
    // }
  }

  device.stopVideo();
  device.stopDepth();
}

void AutoDriving::stop() {
  _stop = true;
 
  _thread->join();
  delete _thread;
}
