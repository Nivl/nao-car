//
// Drive.hh for  in /home/olivie_a//naoqi-sdk-1.12.5-linux64/examples/drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep  5 23:47:40 2012 samuel olivier
// Last update Fri Sep 28 18:58:18 2012 samuel olivier
//

#ifndef __DRIVE_HH__
# define __DRIVE_HH__

# include <map>

# include <alcommon/almodule.h>
# include <PoseManager.hpp>
# include <Animation.hpp>
# include <atomic>
# include <thread>
# include <mutex>

namespace AL
{
  class ALBroker;
}

class Drive : public AL::ALModule
{
public:
  enum SteeringWheel
    {
      Left,
      Front,
      Right
    };

  enum Speed
    {
      Up,
      Down
    };

  Drive(boost::shared_ptr<AL::ALBroker> broker,
       const std::string &name);
  virtual ~Drive();

  virtual void	init();

  void	start();
  void	stop();
  void	animThread();

  void	up();
  void	down();
  void	left();
  void	right();
  void	stopPush();
  void	stopTurn();
  void	takeSteeringWheel();
  void	releaseSteeringWheel();
  void	beginNoHand();
  void	endNoHand();
  void	setHead(float const& headYaw, float const& headPitch,
		float const& maxSpeed);

  bool	steeringWheelIsTaken();
  bool	isGasPedalPushed();
  int	steeringWheelDirection();
  int	speed();
  bool	isNoHand();
  bool	isAnimating();

private:
  void	launch(std::string const& name);
  void	addAnim(std::string const& name);

  struct Anim
  {
    Animation   _anim;
    int         _valueToGive;
    std::atomic<int>    (Drive::*_valueToChange);
  };

  static std::map<std::string, Anim>	_animations;
  PoseManager				_poseManager;
  std::mutex				_animationsMutex;
  std::list<std::string>		_animList;
  std::mutex				_animListMutex;
  std::thread				*_animThread;
  std::atomic<bool>			_isAnimating;
  std::atomic<bool>			_stopThread;

  std::atomic<int>			_steeringWheelIsTaken;
  std::atomic<int>			_gasPedalIsPushed;
  std::atomic<int>			_steeringWheelDirection;
  std::atomic<int>			_speed;
  std::atomic<int>			_noHand;
};

#endif
