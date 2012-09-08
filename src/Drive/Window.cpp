//
// Window.cpp for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Sep  7 20:00:32 2012 samuel olivier
// Last update Sat Sep  8 15:43:48 2012 samuel olivier
//

#include <iostream>
#include "Window.hpp"

Window::Window(boost::shared_ptr<AL::ALBroker> broker) :
  QWidget(NULL), _poseManager(broker), _steeringWheelIsTaken(false),
  _gasPedalIsPushed(false), _steeringWheelDirection(Front), _speed(Up)
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
}

void	Window::keyPressEvent(QKeyEvent *event)
{
  if (event->isAutoRepeat())
    return ;
  if (event->key() == Qt::Key_Up)
    {
      if (_gasPedalIsPushed == true)
  	{
  	  std::cout << "NEVER?" << std::endl;
  	  return ;
  	}
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
