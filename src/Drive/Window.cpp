//
// Window.cpp for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Sep  7 20:00:32 2012 samuel olivier
//

#include <iostream>
#include "Window.hpp"

Window::Window(boost::shared_ptr<AL::ALBroker> broker) :
  QWidget(NULL), _poseManager(broker), _steeringWheelIsTaken(false),
  _gasPedalIsPushed(false), _steeringWheelDirection(Front), _speed(Up),
  _camera(broker), _cameraId("cameraId"), _cameraTimer(new QTimer(this)),
  _cameraLabel(new QLabel()), _cameraImage(), _cameraPixmap()
{
  _animations["DownShift"] =
    Animation::loadFromFile("Poses/downshift.anim");
  _animations["UpShift"] =
    Animation::loadFromFile("Poses/upshift.anim");
  _animations["PushGasPedal"] =
    Animation::loadFromFile("Poses/push-gas-pedal.anim");
  _animations["ReleaseGasPedal"] =
    Animation::loadFromFile("Poses/release-gas-pedal.anim");
  _animations["TakeSteeringWheel"] =
    Animation::loadFromFile("Poses/take-steering-wheel.anim");
  _animations["ReleaseSteeringWheel"] =
    Animation::loadFromFile("Poses/release-steering-wheel.anim");
  _animations["TurnLeft"] =
    Animation::loadFromFile("Poses/turn-left.anim");
  _animations["TurnFront"] =
    Animation::loadFromFile("Poses/turn-front.anim");
  _animations["TurnRight"] =
    Animation::loadFromFile("Poses/turn-right.anim");
  launch("ReleaseSteeringWheel");
  launch("UpShift");

  // _cameraId = _camera.subscribe(_cameraId, AL::kQQVGA, AL::kRGBColorSpace, 30);
  // connect(_cameraTimer, SIGNAL(timeout()), this, SLOT(updateCamera()));
  // _cameraTimer->setInterval(30);
  // _cameraTimer->start();

  // QVBoxLayout* layout = new QVBoxLayout(this);
  // layout->addWidget(_cameraLabel);
  // this->setLayout(layout);
}

Window::~Window()
{
  _camera.unsubscribe(_cameraId);
}

void Window::updateCamera()
{
  AL::ALValue result;

  result = _camera.getImageRemote(_cameraId);
  const unsigned char* imageData = static_cast<const unsigned char*>(result[6].GetBinary());

  if (imageData != NULL)
    {
      _cameraImage = QImage(imageData, 160, 120, QImage::Format_RGB888);
      _cameraPixmap = QPixmap::fromImage(_cameraImage);
      _cameraLabel->setPixmap(_cameraPixmap);
    }

  _camera.releaseImage(_cameraId);
}

void	Window::keyPressEvent(QKeyEvent *event)
{
  if (event->isAutoRepeat())
    return ;
  if (event->key() == Qt::Key_Up)
    {
      if (_gasPedalIsPushed == true)
	return ;
      if (_speed == Down)
  	{
  	  if (_steeringWheelIsTaken == true)
  	    {
  	      if (_steeringWheelDirection != Front)
  		{		
  		  launch("TurnFront");
  		  _steeringWheelDirection = Front;
  		}
  	      launch("ReleaseSteeringWheel");
  	      _steeringWheelIsTaken = false;
  	    }
  	  launch("UpShift");
  	  _speed = Up;
  	}
      if (_steeringWheelIsTaken == false)
  	{
  	  launch("TakeSteeringWheel");
  	  _steeringWheelIsTaken = true;
  	}
      if (_gasPedalIsPushed == false)
  	{
  	  launch("PushGasPedal");
  	  _gasPedalIsPushed = true;
  	}
    }
  else if (event->key() == Qt::Key_Down)
    {
      if (_gasPedalIsPushed == true)
  	return ;
      if (_speed == Up)
  	{
  	  if (_steeringWheelIsTaken == true)
  	    {
  	      if (_steeringWheelDirection != Front)
  		{		
  		  launch("TurnFront");
  		  _steeringWheelDirection = Front;
  		}
  	      launch("ReleaseSteeringWheel");
  	      _steeringWheelIsTaken = false;
  	    }
  	  launch("DownShift");
  	  _speed = Down;
  	}
      if (_steeringWheelIsTaken == false)
  	{
  	  launch("TakeSteeringWheel");
  	  _steeringWheelIsTaken = true;
  	}
      if (_gasPedalIsPushed == false)
  	{
  	  launch("PushGasPedal");
  	  _gasPedalIsPushed = true;
  	}
    }
  else if (event->key() == Qt::Key_Left)
    {
      if (_steeringWheelIsTaken == false)
  	{
  	  launch("TakeSteeringWheel");
  	  _steeringWheelIsTaken = true;
	  _steeringWheelDirection = Front;
  	}
      if (_steeringWheelDirection != Left)
	{
	  launch("TurnLeft");
	  _steeringWheelDirection = Left;
	}
    }
  else if (event->key() == Qt::Key_Right)
    {
      if (_steeringWheelIsTaken == false)
  	{
  	  launch("TakeSteeringWheel");
  	  _steeringWheelIsTaken = true;
	  _steeringWheelDirection = Front;
  	}
      if (_steeringWheelDirection != Right)
	{
	  launch("TurnRight");
	  _steeringWheelDirection = Right;
	}
    }
  else if (event->key() == Qt::Key_Space)
    {
      launch(_steeringWheelIsTaken == true ? "ReleaseSteeringWheel" :
	     "TakeSteeringWheel");
      _steeringWheelIsTaken = !_steeringWheelIsTaken;
    }
}

void	Window::keyReleaseEvent(QKeyEvent *event)
{
  if (event->isAutoRepeat())
    return ;
  if (event->key() == Qt::Key_Up)
    {
      if (_speed == Up && _gasPedalIsPushed == true)
  	{
  	  launch("ReleaseGasPedal");
  	  _gasPedalIsPushed = false;
  	}
    }
  else if (event->key() == Qt::Key_Down)
    {
      if (_speed == Down && _gasPedalIsPushed == true)
  	{
  	  launch("ReleaseGasPedal");
  	  _gasPedalIsPushed = false;
  	}
    }
  else if (event->key() == Qt::Key_Left)
    {
      if (_steeringWheelIsTaken == true && _steeringWheelDirection == Left)
	{
	  launch("TurnFront");
	  _steeringWheelDirection = Front;
	}
    }
  else if (event->key() == Qt::Key_Right)
    {
      if (_steeringWheelIsTaken == true && _steeringWheelDirection == Right)
	{
	  launch("TurnFront");
	  _steeringWheelDirection = Front;
	}
    }
}

void	Window::launch(std::string const& name)
{
  try
    {
      _animations[name].animate(_poseManager);
    }
  catch (...)
    {
    }
  std::cout << "Launching : " << name << std::endl;
}
