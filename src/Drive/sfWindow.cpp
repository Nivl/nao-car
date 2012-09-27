//
// sfWindow.cpp for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep 26 18:34:52 2012 samuel olivier
// Last update Thu Sep 27 12:08:55 2012 samuel olivier
//

#include <iostream>
#include "sfWindow.hpp"

Window::Window(boost::shared_ptr<AL::ALBroker> broker, int joystickId) :
  _window(sf::VideoMode(100, 100), "NaoCar"), _poseManager(broker),
  _steeringWheelIsTaken(false), _gasPedalIsPushed(false),
  _steeringWheelDirection(Front), _speed(Up), _joystickId(joystickId)
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
  _window.setVerticalSyncEnabled(true);
  _window.setFramerateLimit(30); 
}

Window::~Window()
{
}

void	Window::exec()
{
  while (_window.isOpen())
    {
      sf::Event	event;
      while (_window.pollEvent(event))
	checkEvent(&event);
    }
}

void	Window::checkEvent(sf::Event *event)
{
  if (event->type == sf::Event::Closed)
    _window.close();
  else if (event->type == sf::Event::KeyPressed)
    keyPressEvent(event);
  else if (event->type == sf::Event::KeyReleased)
    keyReleaseEvent(event);
  else if (event->type == sf::Event::JoystickButtonPressed &&
  	   event->joystickButton.joystickId == _joystickId)
    {
      if (event->joystickButton.button == 0)
	{
	  launch(_steeringWheelIsTaken == true ? "ReleaseSteeringWheel" :
		 "TakeSteeringWheel");
	  _steeringWheelIsTaken = !_steeringWheelIsTaken;
	}
    }
  else if (event->type == sf::Event::JoystickMoved &&
  	   event->joystickButton.joystickId == _joystickId)
    {
      if (sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::R) > 0)
	up();
      else if (sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::Z) > 0)
	down();
      else
	stopPush();
      if (sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::X) < -50)
	left();
      else if (sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::X) > 50)
	right();
      else
	stopTurn();
      static int pX = 0, pY = 0;
      // if ((sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::U) != pX ||
      // 	   sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::V) != pY) &&
      // 	  _clock.getElapsedTime().asMilliseconds() > 100)
      // 	{
	  pX = sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::U);
	  pY = sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::V);
	  static std::vector<std::string>	names =
	    {"HeadYaw", "HeadPitch"};
	  std::vector<float>			angles =
	    {((float)(-pX + 100) / 200.f) * (120.f + 120.f) - 120.f,
	     ((float)(pY + 100) / 200.f) * (30.f + 40.f) - 40.f};
	  angles[0] = angles[0] * M_PI / 180;
	  angles[1] = angles[1] * M_PI / 180;
	  _poseManager.getMotionProxy().setAngles(names, angles, 0.4);
	  _clock.restart();
	// }
    }
}

void	Window::keyPressEvent(sf::Event *event)
{
  if (event->key.code == sf::Keyboard::Up)
    up();
  else if (event->key.code == sf::Keyboard::Down)
    down();
  else if (event->key.code == sf::Keyboard::Left)
    left();
  else if (event->key.code == sf::Keyboard::Right)
    right();
  else if (event->key.code == sf::Keyboard::Space)
    {
      launch(_steeringWheelIsTaken == true ? "ReleaseSteeringWheel" :
  	     "TakeSteeringWheel");
      _steeringWheelIsTaken = !_steeringWheelIsTaken;
    }
}

void	Window::keyReleaseEvent(sf::Event *event)
{
  if ((event->key.code == sf::Keyboard::Up && _speed == Up) ||
      (event->key.code == sf::Keyboard::Down && _speed == Down))
    stopPush();
  else if (_steeringWheelIsTaken == true &&
	   (event->key.code == sf::Keyboard::Left ||
	    event->key.code == sf::Keyboard::Right))
    stopTurn();
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

void	Window::up()
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
      _poseManager.setPose(_animations["PushGasPedal"].getPoses()
			   .front().first, 0.5);
      _gasPedalIsPushed = true;
    }
}

void	Window::down()
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
      _poseManager.setPose(_animations["PushGasPedal"].getPoses()
			   .front().first, 0.5);
      _gasPedalIsPushed = true;
    }
}

void	Window::left()
{
  if (_steeringWheelIsTaken == false)
    {
      launch("TakeSteeringWheel");
      _steeringWheelIsTaken = true;
      _steeringWheelDirection = Front;
    }
  if (_steeringWheelDirection != Left)
    {
      _poseManager.setPose(_animations["TurnLeft"].getPoses()
			   .front().first, 0.5);
      _steeringWheelDirection = Left;
    }
}

void	Window::right()
{
  if (_steeringWheelIsTaken == false)
    {
      launch("TakeSteeringWheel");
      _steeringWheelIsTaken = true;
      _steeringWheelDirection = Front;
    }
  if (_steeringWheelDirection != Right)
    {
      _poseManager.setPose(_animations["TurnRight"].getPoses()
			   .front().first, 0.5);
      _steeringWheelDirection = Right;
    }
}

void	Window::stopPush()
{
  if (_gasPedalIsPushed == true)
    {
      _poseManager.setPose(_animations["ReleaseGasPedal"].getPoses()
			   .front().first, 0.5);
      _gasPedalIsPushed = false;
    }
}

void	Window::stopTurn()
{
  if (_steeringWheelIsTaken == true && _steeringWheelDirection != Front)
    {
      _poseManager.setPose(_animations["TurnFront"].getPoses()
			   .front().first, 0.5);
      _steeringWheelDirection = Front;
    }
}

