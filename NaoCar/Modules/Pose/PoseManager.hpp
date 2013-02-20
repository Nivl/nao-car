//
// PoseManager.hpp for  in /home/olivie_a//rendu/nao/nao-car/src/Pose
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Thu Sep  6 23:58:39 2012 samuel olivier
// Last update Wed Feb 20 12:44:20 2013 samuel olivier
//

#ifndef __POSE_MANAGER_HH__
# define __POSE_MANAGER_HH__

# include <boost/shared_ptr.hpp>
# include <alproxies/almotionproxy.h>
# include <alcommon/albroker.h>

# include "Pose.hpp"

class PoseManager
{
public:
  PoseManager(boost::shared_ptr<AL::ALBroker> broker);
  ~PoseManager();

  void	takePose(Pose const& pose, float duration);
  void	setPose(Pose const& pose, float speed);
  Pose	getRobotPose();

  AL::ALMotionProxy&	getProxy();

private:
  boost::shared_ptr<AL::ALBroker>	_broker;
  AL::ALMotionProxy			_motion;
};

#endif
