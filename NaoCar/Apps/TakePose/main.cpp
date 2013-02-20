//
// save_pose.cpp for  in /home/olivie_a//rendu/nao/nao-car/src/Pose
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Sep  7 13:54:38 2012 samuel olivier
// Last update Fri Sep  7 14:16:34 2012 samuel olivier
//

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <alerror/alerror.h>
#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>
#include <unistd.h>

#include <alproxies/altexttospeechproxy.h>
#include <alproxies/almotionproxy.h>

#include "PoseManager.hpp"

int main(int ac, char**av)
{
  if (ac != 4)
    {
      std::cout << "Usage: " << av[0] << " ip pose_file duration" << std::endl;
      return (1);
    }

  // Create a brooker to connect to the robot
  boost::shared_ptr<AL::ALBroker> broker =
    AL::ALBroker::createBroker(
			       "broker",
			       "0.0.0.0",
			       0,
			       av[1],
			       9559,
			       0);
  PoseManager manager(broker);
  AL::ALMotionProxy	motion(broker);

  motion.setStiffnesses("Body", 1);
  Pose res = Pose::loadFromFile(av[2]);
  manager.takePose(res, atof(av[3]));
  return (0);
}
