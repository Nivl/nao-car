//
// PoseManager.hpp for  in /home/olivie_a//rendu/nao/nao-car/src/Pose
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Thu Sep  6 23:58:39 2012 samuel olivier
// Last update Fri Sep  7 00:16:42 2012 samuel olivier
//

#ifndef __POSE_MANAGER_HH__
# define __POSE_MANAGER_HH__

namespace AL
{
  class ALBroker;
};

# include <boost/shared_ptr.hpp>

# include "Pose.hpp"

class PoseManager
{
public:
  PoseManager(boost::shared_ptr<AL::ALBroker> broker);
  ~PoseManager();

  void	takePose(Pose const& pose, float duration);
  Pose	getRobotPose();

private:
  boost::shared_ptr<AL::ALBroker>	_broker;
};

#endif
