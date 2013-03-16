//
// Drive.cpp for  in /home/olivie_a//naoqi-sdk-1.12.5-linux64/examples/drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep  5 23:47:17 2012 samuel olivier
// Last update Fri Mar 15 15:22:36 2013 samuel olivier
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

#ifdef NAO_LOCAL_COMPILATION
# define POSE_DIR "/home/nao/modules/Poses/"
#else
# define POSE_DIR "Modules/Drive/Poses/"
#endif

static void	launchAnimThread(void *mod);
//std::map<Drive::State, std::map<Drive::Action, Drive::actionFunction> >     Drive::_fsm;

std::map<std::string, Drive::Anim>	Drive::_animations =
  {
    {"DownShift",
     Anim{Animation::loadFromFile(POSE_DIR "downshift.anim")}},
    {"UpShift",
     Anim{Animation::loadFromFile(POSE_DIR "upshift.anim")}},
    {"PushGasPedal",
     Anim{Animation::loadFromFile(POSE_DIR "push-gas-pedal.anim")}},
    {"ReleaseGasPedal",
     Anim{Animation::loadFromFile(POSE_DIR "release-gas-pedal.anim")}},
    {"TakeSteeringWheel",
     Anim{Animation::loadFromFile(POSE_DIR "take-steering-wheel.anim")}},
    {"ReleaseSteeringWheel",
     Anim{Animation::loadFromFile(POSE_DIR "release-steering-wheel.anim")}},
    {"TurnLeft",
     Anim{Animation::loadFromFile(POSE_DIR "turn-left.anim")}},
    {"TurnFront",
     Anim{Animation::loadFromFile(POSE_DIR "turn-front.anim")}},
    {"TurnRight",
     Anim{Animation::loadFromFile(POSE_DIR "turn-right.anim")}},
    {"BeginNoHand",
     Anim{Animation::loadFromFile(POSE_DIR "begin-no-hand.anim")}},
    {"StopNoHand",
     Anim{Animation::loadFromFile(POSE_DIR "stop-no-hand.anim")}},
    {"TakeCarembar",
     Anim{Animation::loadFromFile(POSE_DIR "take-carembar.anim")}},
    {"GiveCarembar",
     Anim{Animation::loadFromFile(POSE_DIR "give-carembar.anim")}},
  };

Drive::Drive(boost::shared_ptr<AL::ALBroker> broker,
	     const std::string &name) :
  AL::ALModule(broker, name), _poseManager(broker), _animThread(NULL)
{
  setModuleDescription("The NaoCar Driving Module");
  functionName("begin", getName(), "Set the nao ready for starting");
  BIND_METHOD(Drive::begin)
  functionName("end", getName(), "Stop the nao car drive mode");
  BIND_METHOD(Drive::end);
  functionName("goFrontwards", getName(), "Move forward");
  BIND_METHOD(Drive::goFrontwards);
  functionName("goBackwards", getName(), "Move back");
  BIND_METHOD(Drive::goBackwards);
  functionName("turnLeft", getName(), "Move left");
  BIND_METHOD(Drive::turnLeft);
  functionName("turnRight", getName(), "Move right");
  BIND_METHOD(Drive::turnRight);
  functionName("turnFront", getName(), "Stop turning steering wheel");
  BIND_METHOD(Drive::turnFront);
  functionName("stop", getName(), "Stop pushing gas pedal");
  BIND_METHOD(Drive::stop);
  functionName("steeringWheelAction", getName(),
	       "Take or release the steering wheel");
  BIND_METHOD(Drive::steeringWheelAction);
  functionName("funAction", getName(), "Start or finish \"No Hand Driving\"");
  BIND_METHOD(Drive::funAction);
  functionName("carambarAction", getName(), "Take or give a carambar");
  BIND_METHOD(Drive::carambarAction);
  functionName("setHead", getName(), "Set the specified angles "
	       "to the head");
  BIND_METHOD(Drive::setHead);
  functionName("isSteeringWheelTaken", getName(),
	       "return steeringWheelIsTaken");
  BIND_METHOD(Drive::isSteeringWheelTaken);
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
  functionName("isCarembar", getName(),
	       "return isCarembar");
  BIND_METHOD(Drive::isCarembar);

}

Drive::~Drive()
{
}

void	Drive::init()
{
  _currentState.position = Vegetative;
  _currentState.direction = Forward;
  _currentState.pedal = Released;
}

void	Drive::begin()
{
  if (_currentState == Vegetative) {
    _stopThread = false;

    // LAUNCH ANIMATION FOR WAKE UP
    _poseManager.getProxy().setStiffnesses("Body", 1);
    addAnim("ReleaseSteeringWheel");
    addAnim("UpShift");
    if (_animThread == NULL)
      _animThread = new std::thread(launchAnimThread, (void*)this);

    _currentState.position = Ready;
    _currentState.direction = Forward;
    _currentState.pedal = Released;
  }
}

void	Drive::end()
{
  _stopThread = true;
  if (_animThread)
    {
      _animThread->join();
      delete _animThread;
      _animThread = NULL;
    }
  _poseManager.getProxy().setStiffnesses("Body", 0);
  _currentState = Vegetative;
  _currentState.direction = Forward;
  _currentState.pedal = Released;
}

void	Drive::goFrontwards()
{
  if (_currentState.position == DrivingFront ||
      _currentState.position == DrivingLeft ||
      _currentState.position == DrivingRight) {
    if (_currentState.direction == Forward && _currentState.pedal == Released) {
      addAnim("PushGasPedal");
      _currentState.position = Pushed;
      return;
    }
  }
  if (_currentState.direction == Backward) {
    if (_currentState.position == DrivingLeft || _currentState.position == DrivingRight) {
      addAnim("TurnFront");
      _currentState.position = DrivingFront;
      return goFrontwards();
    }
    if (_currentState == DrivingFront) {
      addAnim("ReleaseSteeringWheel");
      _currentState.position = Ready;
      return goFrontwards();
    }
    if (_currentState == Ready) {
      addAnim("UpShift");
      addAnim("TakeSteeringWheel");
      addAnim("PushGasPedal");
      _currentState.position = DrivingFront;
      _currentState.direction = Forward;
      _currentState.pedal = Pushed;
    }
  }
}

void	Drive::goBackwards()
{
  if (_currentState.position == DrivingFront ||
      _currentState.position == DrivingLeft ||
      _currentState.position == DrivingRight) {
    if (_currentState.direction == Backward && _currentState.pedal == Released) {
      addAnim("PushGasPedal");
      _currentState.position = Pushed;
      return;
    }
  }
  if (_currentState.direction == Frontward) {
    if (_currentState.position == DrivingLeft || _currentState.position == DrivingRight) {
      addAnim("TurnFront");
      _currentState.position = DrivingFront;
      return goBackwards();
    }
    if (_currentState == DrivingFront) {
      addAnim("ReleaseSteeringWheel");
      _currentState.position = Ready;
      return goBackwards();
    }
    if (_currentState == Ready) {
      addAnim("DownShift");
      addAnim("TakeSteeringWheel");
      addAnim("PushGasPedal");
      _currentState.position = DrivingFront;
      _currentState.direction = Backward;
      _currentState.pedal = Pushed;
    }
  }

}

void	Drive::turnLeft()
{
  if (_currentState.position = DrivingFront ||
      _currentState.position = DrivingLeft ||
      _currentState.position = DrivingRIght) {
    addAnim("TurnLeft");
    _currentState.position = DrivingLeft;
  }
}

void	Drive::turnRight()
{
  if (_currentState.position = DrivingFront ||
      _currentState.position = DrivingLeft ||
      _currentState.position = DrivingRIght) {
    addAnim("TurnRight");
    _currentState.position = DrivingRight;
  }
}

void	Drive::turnFront()
{
  if (_currentState.position = DrivingFront ||
      _currentState.position = DrivingLeft ||
      _currentState.position = DrivingRIght) {
    addAnim("TurnFront");
    _currentState.position = DrivingFront;
  }
}

void	Drive::stop()
{
  addAnim("ReleaseGasPedal");
  _currentState.pedale = Released;
}

void	Drive::steeringWheelAction() {
  if (_currentState.position == Ready) {
    addAnim("TakeSteeringWheel");
    _currentState.position = Ready;
  }
  else if (_currentState.position == DrivingFront && _currentState.pedale == Released)
    addAnim("ReleaseSteeringWheel");
}

void	Drive::funAction() {
  if (_currentState.position == DrivingFront)
    
  addAnim(_noHand == false ?
	  "BeginNoHand" :
	  "StopNoHand");
}

void	Drive::carambarAction() {
  addAnim(_carembar == false ?
	  "TakeCarembar" :
	  "GiveCarembar");
}

void	Drive::setHead(float const& headYaw, float const& headPitch,
		       float const& maxSpeed)
{
  static std::vector<std::string> names = {"HeadYaw", "HeadPitch"};
  std::vector<float> angles = {headYaw, headPitch};

  _poseManager.getProxy().setAngles(names, angles, maxSpeed);
}

bool	Drive::isSteeringWheelTaken()
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

bool	Drive::isCarembar()
{
  return (_carembar);
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
	  move = true;
	}
      _animListMutex.unlock();
      if (move == true)
	{
	  std::cout << "Launching : " << current << std::endl;
	  launch(current);
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

      if (name == "PushGasPedal" || name == "ReleaseGasPedal" || name == "TurnRight" || name == "TurnLeft") {
	_poseManager.setPose(_animations[name]._anim.getPoses()
			     .front().first, 0.5);
	_animationsMutex.unlock();
	return;
      }
      _animationsMutex.unlock();
      tmp.animate(_poseManager);
    }
  catch (...)
    {
    }
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
int main(int argc, char* argv[])
{
  // pointer to createModule
  TMainType sig;
  sig = &_createModule;
  // call main
  return ALTools::mainFunction("Drive", argc, argv, sig);
}
#endif
