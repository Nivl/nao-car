//
// Drive.hh for  in /home/olivie_a//naoqi-sdk-1.12.5-linux64/examples/drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep  5 23:47:40 2012 samuel olivier
// Last update Fri Sep  7 00:18:21 2012 samuel olivier
//

#ifndef __DRIVE_HH__
# define __DRIVE_HH__

# include <map>

# include <alcommon/almodule.h>
# include <PoseManager.hpp>

namespace AL
{
  class ALBroker;
}

class Drive : public AL::ALModule
{
public:
  Drive(boost::shared_ptr<AL::ALBroker> broker,
       const std::string &name);
  virtual ~Drive();

  virtual void init();

  void	initDrivePose();
  void	pushPedal();
  void	releasePedal();

private:
  enum DrivePose
    {
      Initial,
      PedalPushed,
      PedalReleased
    };

  static std::map<DrivePose, Pose>	_poseMap;
  PoseManager				_poseManager;
};

#endif
