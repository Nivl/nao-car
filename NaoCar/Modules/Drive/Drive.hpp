//
// Drive.hh for  in /home/olivie_a//naoqi-sdk-1.12.5-linux64/examples/drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep  5 23:47:40 2012 samuel olivier
// Last update Sat Mar 16 16:50:39 2013 loick michard
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
  void	animThread();

  void	begin();
  void	end();

  void	goFrontwards();
  void	goBackwards();
  void	turnLeft();
  void	turnRight();
  void	turnFront();
  void	stop();
  void	steeringWheelAction();
  void	funAction();
  void	carambarAction();
  void	setHead(float const& headYaw, float const& headPitch,
		float const& maxSpeed);

  bool	isSteeringWheelTaken();
  bool	isGasPedalPushed();
  int	steeringWheelDirection();
  int	speed();
  bool	isNoHand();
  bool	isAnimating();
  bool	isCarembar();

private:
  enum PositionState {
    Vegetative,
    Ready,
    DrivingFront,
    DrivingLeft,
    DrivingRight
  };
  enum DirectionState {
    Forward,
    Backward
  };
  enum PedalState {
    Pushed,
    Released
  }

  struct State {
    PositionState	position;
    DirectionState	direction;
    PedalState		pedal;
  };
  State								_currentState;

  void	launch(std::string const& name);
  void	addAnim(std::string const& name);

  struct Anim
  {
    Animation   _anim;
  };

  static std::map<std::string, Anim>	_animations;
  PoseManager				_poseManager;
  std::mutex				_animationsMutex;
  std::list<std::string>		_animList;
  std::mutex				_animListMutex;
  std::thread				*_animThread;
  std::atomic<bool>			_isAnimating;
  std::atomic<bool>			_stopThread;

};

#endif
