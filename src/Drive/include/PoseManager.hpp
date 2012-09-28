//
// PoseManager.hpp for  in /home/olivie_a//rendu/nao/nao-car/src/Pose
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Thu Sep  6 23:58:39 2012 samuel olivier
// Last update Wed Sep 26 20:07:41 2012 samuel olivier
//

#ifndef __POSE_MANAGER_HH__
# define __POSE_MANAGER_HH__

namespace AL
{
  class ALBroker;
};

# include <boost/shared_ptr.hpp>
# include <alproxies/almotionproxy.h>

# include "Pose.hpp"

class PoseManager
{
public:
  PoseManager(boost::shared_ptr<AL::ALBroker> broker);
  ~PoseManager();

  void	takePose(Pose const& pose, float duration);
  void	setPose(Pose const& pose, float speed);
  Pose	getRobotPose();

  AL::ALMotionProxy&	getMotionProxy();

private:
  boost::shared_ptr<AL::ALBroker>	_broker;
  AL::ALMotionProxy			_motion;
};

#endif
