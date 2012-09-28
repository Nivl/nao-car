//
// Drive.cpp for  in /home/olivie_a//naoqi-sdk-1.12.5-linux64/examples/drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep  5 23:47:17 2012 samuel olivier
// Last update Fri Sep 28 19:18:31 2012 samuel olivier
//

#include <iostream>
#include <alcommon/albroker.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <iostream>
#include <stdlib.h>
#include <qi/os.hpp>
#include <alcommon/almodule.h>
#include <alcommon/albrokermanager.h>
#include <alcommon/altoolsmain.h>

#include "Drive.hpp"

static void	launchAnimThread(void *mod);

std::map<std::string, Drive::Anim>	Drive::_animations =
  {
    {"DownShift",
     Anim{Animation::loadFromFile("Poses/downshift.anim"), Down,
	  (std::atomic<int> (Drive::*))&Drive::_speed}},
    {"UpShift",
     Anim{Animation::loadFromFile("Poses/upshift.anim"), Up,
	  (std::atomic<int> (Drive::*))&Drive::_speed}},
    {"PushGasPedal",
     Anim{Animation::loadFromFile("Poses/push-gas-pedal.anim"), true,
	  (std::atomic<int> (Drive::*))&Drive::_gasPedalIsPushed}},
    {"ReleaseGasPedal",
     Anim{Animation::loadFromFile("Poses/release-gas-pedal.anim"), false,
	  (std::atomic<int> (Drive::*))&Drive::_gasPedalIsPushed}},
    {"TakeSteeringWheel",
     Anim{Animation::loadFromFile("Poses/take-steering-wheel.anim"), true,
	  (std::atomic<int> (Drive::*))&Drive::_steeringWheelIsTaken}},
    {"ReleaseSteeringWheel",
     Anim{Animation::loadFromFile("Poses/release-steering-wheel.anim"), false,
	  (std::atomic<int> (Drive::*))&Drive::_steeringWheelIsTaken}},
    {"TurnLeft",
     Anim{Animation::loadFromFile("Poses/turn-left.anim"), Left,
	  (std::atomic<int> (Drive::*))&Drive::_steeringWheelDirection}},
    {"TurnFront",
     Anim{Animation::loadFromFile("Poses/turn-front.anim"), Front,
	  (std::atomic<int> (Drive::*))&Drive::_steeringWheelDirection}},
    {"TurnRight",
     Anim{Animation::loadFromFile("Poses/turn-right.anim"), Right,
	  (std::atomic<int> (Drive::*))&Drive::_steeringWheelDirection}},
    {"BeginNoHand",
     Anim{Animation::loadFromFile("Poses/begin-no-hand.anim"), true,
	  (std::atomic<int> (Drive::*))&Drive::_noHand}},
    {"StopNoHand",
     Anim{Animation::loadFromFile("Poses/stop-no-hand.anim"), false,
	  (std::atomic<int> (Drive::*))&Drive::_noHand}},
  };

Drive::Drive(boost::shared_ptr<AL::ALBroker> broker,
	     const std::string &name) :
  AL::ALModule(broker, name), _poseManager(broker), _animThread(NULL)
{
  setModuleDescription("The NaoCar Driving Module");
  functionName("start", getName(), "Set the nao ready for starting");
  BIND_METHOD(Drive::start);
  functionName("stop", getName(), "Stop the nao");
  BIND_METHOD(Drive::stop);
  functionName("up", getName(), "Move forward");
  BIND_METHOD(Drive::up);
  functionName("down", getName(), "Move back");
  BIND_METHOD(Drive::down);
  functionName("left", getName(), "Move left");
  BIND_METHOD(Drive::left);
  functionName("right", getName(), "Move right");
  BIND_METHOD(Drive::right);
  functionName("stopPush", getName(), "Stop pushing gas pedal");
  BIND_METHOD(Drive::stopPush);
  functionName("stopTurn", getName(), "Stop turning steering wheel");
  BIND_METHOD(Drive::stopTurn);
  functionName("takeSteeringWheel", getName(), "Take the steering wheel");
  BIND_METHOD(Drive::takeSteeringWheel);
  functionName("releaseSteeringWheel", getName(),
	       "Release the steering wheel");
  BIND_METHOD(Drive::releaseSteeringWheel);
  functionName("beginNoHand", getName(), "Start \"No Hand Driving\"");
  BIND_METHOD(Drive::beginNoHand);
  functionName("endNoHand", getName(), "Stop \"No Hand Driving\"");
  BIND_METHOD(Drive::endNoHand);
  functionName("setHead", getName(), "Set the specified angles "
	       "to the head");
  BIND_METHOD(Drive::setHead);
  functionName("steeringWheelIsTaken", getName(),
	       "return steeringWheelIsTaken");
  BIND_METHOD(Drive::steeringWheelIsTaken);
  functionName("isGasPedalPushed", getName(),
	       "return isGasPedalPushed");
  BIND_METHOD(Drive::isGasPedalPushed);
  functionName("steeringWheelDirection", getName(),
	       "return steeringWheelDirection");
  BIND_METHOD(Drive::steeringWheelDirection);
  functionName("speed", getName(),
	       "return speed");
  BIND_METHOD(Drive::speed);
  functionName("isNoHand", getName(),
	       "return isNoHand");
  BIND_METHOD(Drive::isNoHand);
  functionName("isAnimating", getName(),
	       "return isAnimating");
  BIND_METHOD(Drive::isAnimating);
}

Drive::~Drive()
{
}

void	Drive::init()
{
}

void	Drive::start()
{
  _steeringWheelIsTaken = true;
  _gasPedalIsPushed = false;
  _steeringWheelDirection = Front;
  _speed = Down;
  _noHand = false;
  _stopThread = false;
  if (_animThread == NULL)
    _animThread = new std::thread(launchAnimThread, (void*)this);
  addAnim("ReleaseSteeringWheel");
  addAnim("UpShift");
}

void	Drive::stop()
{
  _stopThread = true;
  if (_animThread)
    {
      _animThread->join();
      delete _animThread;
      _animThread = NULL;
    }
}

void	Drive::up()
{
  if (_isAnimating == true || _gasPedalIsPushed == true)
    return ;
  if (_speed == Down)
    {
      if (_steeringWheelIsTaken == true && _noHand == false)
	{
	  if (_steeringWheelDirection != Front)
	    addAnim("TurnFront");
	  addAnim("ReleaseSteeringWheel");
	}
      else if (_noHand == true)
	addAnim("StopNoHand");
      addAnim("UpShift");
    }
  if (_steeringWheelIsTaken == false && _noHand == false)
    addAnim("TakeSteeringWheel");
  if (_gasPedalIsPushed == false &&
      (_steeringWheelIsTaken == true || _noHand == true) && _speed == Up)
    {
      _animationsMutex.lock();
      _poseManager.setPose(_animations["PushGasPedal"]._anim.getPoses()
			   .front().first, 0.5);
      _animationsMutex.unlock();
      _gasPedalIsPushed = true;
    }
}

void	Drive::down()
{
  if (_isAnimating == true || _gasPedalIsPushed == true)
    return ;
  if (_speed == Up)
    {
      if (_steeringWheelIsTaken == true && _noHand == false)
	{
	  if (_steeringWheelDirection != Front)
	    addAnim("TurnFront");
	  addAnim("ReleaseSteeringWheel");
	}
      else if (_noHand == true)
	addAnim("StopNoHand");
      addAnim("DownShift");
    }
  if (_steeringWheelIsTaken == false && _noHand == false)
    addAnim("TakeSteeringWheel");
  if (_gasPedalIsPushed == false &&
      (_steeringWheelIsTaken == true || _noHand == true) && _speed == Down)
    {
      _animationsMutex.lock();
      _poseManager.setPose(_animations["PushGasPedal"]._anim.getPoses()
			   .front().first, 0.5);
      _animationsMutex.unlock();
      _gasPedalIsPushed = true;
    }
}

void	Drive::left()
{
  if (_isAnimating == true)
    return ;
  if (_steeringWheelIsTaken == false && _noHand == false)
    addAnim("TakeSteeringWheel");
  if (_steeringWheelIsTaken == true && _noHand == false
      && _steeringWheelDirection != Left)
    {
      _animationsMutex.lock();
      _poseManager.setPose(_animations["TurnLeft"]._anim.getPoses()
			   .front().first, 0.5);
      _animationsMutex.unlock();
      _steeringWheelDirection = Left;
    }
}

void	Drive::right()
{
  if (_isAnimating == true)
    return ;
  std::cout << "right" << std::endl;
  if (_steeringWheelIsTaken == false && _noHand == false)
    addAnim("TakeSteeringWheel");
  if (_steeringWheelIsTaken == true && _noHand == false
      && _steeringWheelDirection != Right)
    {
      _animationsMutex.lock();
      _poseManager.setPose(_animations["TurnRight"]._anim.getPoses()
			   .front().first, 0.5);
      _animationsMutex.unlock();
      _steeringWheelDirection = Right;
    }
}

void	Drive::stopPush()
{
  if (_gasPedalIsPushed == true)
    {
      _animationsMutex.lock();
      _poseManager.setPose(_animations["ReleaseGasPedal"]._anim.getPoses()
			   .front().first, 0.5);
      _animationsMutex.unlock();
      _gasPedalIsPushed = false;
    }
}

void	Drive::stopTurn()
{
  if (_steeringWheelIsTaken == true && _steeringWheelDirection != Front)
    {
      _animationsMutex.lock();
      _poseManager.setPose(_animations["TurnFront"]._anim.getPoses()
			   .front().first, 0.5);
      _animationsMutex.unlock();
      _steeringWheelDirection = Front;
    }
}

void	Drive::takeSteeringWheel()
{
  if (_steeringWheelIsTaken == false)
    addAnim("TakeSteeringWheel");
}

void	Drive::releaseSteeringWheel()
{
  if (_steeringWheelIsTaken == true)
    addAnim("ReleaseSteeringWheel");
}

void	Drive::beginNoHand()
{
  if (_noHand == false)
    addAnim("BeginNoHand");
}

void	Drive::endNoHand()
{
  if (_noHand == true)
    addAnim("StopNoHand");
}

void	Drive::setHead(float const& headYaw, float const& headPitch,
		       float const& maxSpeed)
{
  static std::vector<std::string> names = {"HeadYaw", "HeadPitch"};
  std::vector<float> angles = {headYaw, headPitch};

  _poseManager.getMotionProxy().setAngles(names, angles, maxSpeed);
}

bool	Drive::steeringWheelIsTaken()
{
  return (_steeringWheelIsTaken);
}

bool	Drive::isGasPedalPushed()
{
  return (_gasPedalIsPushed);
}

int	Drive::steeringWheelDirection()
{
  int	value = _steeringWheelDirection;
  return ((SteeringWheel)value);
}

int	Drive::speed()
{
  int	value = _speed;
  return ((Speed)value);
}

bool	Drive::isNoHand()
{
  return (_noHand);
}

bool	Drive::isAnimating()
{
  return (_isAnimating);
}

void	Drive::animThread()
{
  std::string	current;
  bool		move;

  while (_stopThread == false || _isAnimating == true)
    {
      move = true;
      _animListMutex.lock();
      if (_animList.size() == 0)
	{
	  move = false;
	  _isAnimating = false;
	}
      else
	{
	  _isAnimating = true;
	  current = _animList.front();
	  _animList.pop_front();
	  move = ((this->*_animations[current]._valueToChange) !=
		  _animations[current]._valueToGive);
	}
      _animListMutex.unlock();
      if (move == true)
	{
	  launch(current);
	  _animationsMutex.lock();
	  this->*_animations[current]._valueToChange =
	    _animations[current]._valueToGive;
	  _animationsMutex.unlock();
	}
      else
	usleep(1000);
    }
}

void	Drive::launch(std::string const& name)
{
  try
    {
      Animation	tmp;
      _animationsMutex.lock();
      tmp = _animations[name]._anim;
      _animationsMutex.unlock();
      tmp.animate(_poseManager);
    }
  catch (...)
    {
    }
  std::cout << "Launching : " << name << std::endl;
}

void	Drive::addAnim(std::string const& name)
{
  _animListMutex.lock();
  _animList.push_back(name);
  _animListMutex.unlock();
}

static void	launchAnimThread(void *mod)
{
  ((Drive *)mod)->animThread();
}

#ifdef _WIN32
# define ALCALL __declspec(dllexport)
#else
# define ALCALL
#endif

extern "C"
{
  ALCALL int _createModule(boost::shared_ptr<AL::ALBroker> broker)
  {
    AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
    AL::ALBrokerManager::getInstance()->addBroker(broker);
    AL::ALModule::createModule<Drive>(broker, "Drive");
    return 0;
  }

  ALCALL int _closeModule()
  {
    return 0;
  }
}

#ifdef DRIVE_IS_REMOTE

int main(int argc, char *argv[])
{
  // pointer to createModule
  TMainType sig;
  sig = &_createModule;
  // call main
  return ALTools::mainFunction("Drive", argc, argv, sig);
}
#endif
