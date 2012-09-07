//
// PoseManager.cpp for  in /home/olivie_a//rendu/nao/nao-car/src/Pose
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Thu Sep  6 23:58:43 2012 samuel olivier
// Last update Fri Sep  7 18:07:13 2012 samuel olivier
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
  std::cout << pose.getName() << std::endl;
  std::map<std::string, float> const&	map = pose.getAngles();
  std::vector<std::string>		names;
  std::vector<float>			angles;
  AL::ALMotionProxy			motion(_broker);

  for (std::map<std::string, float>::const_iterator it = map.begin();
       it != map.end(); ++it)
    {
      names.push_back(it->first);
      angles.push_back(it->second);
    }
  motion.setStiffnesses("Body", 1);
  motion.angleInterpolation(names, angles, duration, true);
}

Pose	PoseManager::getRobotPose()
{
  AL::ALMotionProxy		motion(_broker);
  std::vector<std::string>	names = {"HeadYaw", "HeadPitch", "LShoulderPitch", "LShoulderRoll", "LElbowYaw", "LElbowRoll", "LWristYaw", "LHand", "LHipYawPitch", "LHipRoll", "LHipPitch", "LKneePitch", "LAnklePitch", "LAnkleRoll", "RShoulderPitch", "RShoulderRoll", "RElbowYaw", "RElbowRoll", "RWristYaw", "RHand", "RHipYawPitch", "RHipRoll", "RHipPitch", "RKneePitch", "RAnklePitch", "RAnkleRoll"};
  std::vector<float>		angles = motion.getAngles(names, true);
  Pose	res;

  for (uint i = 0; i < names.size(); ++i)
    res.setAngle(names[i], angles[i]);
  return (res);
}
