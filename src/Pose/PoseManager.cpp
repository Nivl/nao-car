//
// PoseManager.cpp for  in /home/olivie_a//rendu/nao/nao-car/src/Pose
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Thu Sep  6 23:58:43 2012 samuel olivier
// Last update Tue Sep 25 20:52:10 2012 gael jochaud-du-plessix
//

#include <alcommon/albroker.h>
#include <qi/os.hpp>

#include "PoseManager.hpp"

PoseManager::PoseManager(boost::shared_ptr<AL::ALBroker> broker) :
  _broker(broker), _motion(_broker)
{
  _motion.setStiffnesses("Body", 1);
}

PoseManager::~PoseManager()
{
  _motion.setStiffnesses("Body", 0);
}

void	PoseManager::takePose(Pose const& pose, float duration)
{
  std::map<std::string, float> const&	map = pose.getAngles();
  std::vector<std::string>		names;
  std::vector<float>			angles;

  for (std::map<std::string, float>::const_iterator it = map.begin();
       it != map.end(); ++it)
    {
      names.push_back(it->first);
      angles.push_back(it->second);
    }
  _motion.angleInterpolation(names, angles, duration, true);
}

void	PoseManager::setPose(Pose const& pose, float speed)
{
  std::map<std::string, float> const&	map = pose.getAngles();
  std::vector<std::string>		names;
  std::vector<float>			angles;

  for (std::map<std::string, float>::const_iterator it = map.begin();
       it != map.end(); ++it)
    {
      names.push_back(it->first);
      angles.push_back(it->second);
    }
  _motion.setAngles(names, angles, speed);
}

Pose	PoseManager::getRobotPose()
{
  std::vector<std::string>	names =
    {"HeadYaw", "HeadPitch", "LShoulderPitch", "LShoulderRoll", "LElbowYaw",
     "LElbowRoll", "LWristYaw", "LHand", "LHipYawPitch", "LHipRoll", "LHipPitch",
     "LKneePitch", "LAnklePitch", "LAnkleRoll", "RShoulderPitch", "RShoulderRoll",
     "RElbowYaw", "RElbowRoll", "RWristYaw", "RHand", "RHipYawPitch", "RHipRoll",
     "RHipPitch", "RKneePitch", "RAnklePitch", "RAnkleRoll"};

  std::vector<float>		angles = _motion.getAngles(names, true);
  Pose	res;

  for (uint i = 0; i < names.size(); ++i)
    res.setAngle(names[i], angles[i]);
  return (res);
}
