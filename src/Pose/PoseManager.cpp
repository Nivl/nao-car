//
// PoseManager.cpp for  in /home/olivie_a//rendu/nao/nao-car/src/Pose
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Thu Sep  6 23:58:43 2012 samuel olivier
// Last update Fri Sep  7 11:14:34 2012 samuel olivier
//

#include <alcommon/albroker.h>
#include <alproxies/almotionproxy.h>

#include "PoseManager.hpp"

PoseManager::PoseManager(boost::shared_ptr<AL::ALBroker> broker) :
  _broker(broker)
{
}

PoseManager::~PoseManager()
{
}

void	PoseManager::takePose(Pose const& pose, float duration)
{

}

Pose	PoseManager::getRobotPose()
{
  AL::ALMotionProxy		motion(_broker);
  std::vector<std::string>	names = {"HeadYaw", "HeadPitch"};
  std::vector<float>		commandAngles = motion.getAngles(names,
								 false);

  std::cout << "Command angles: " << commandAngles << std::endl;
  return (Pose());
}
