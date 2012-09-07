//
// test.cpp
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
  if (ac != 3)
    {
      std::cout << "Usage: " << av[0] << " ip pose_name" << std::endl;
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
  // motion.setStiffnesses("Body", 1);
  // for (int i =0; i < 10; ++i)
  //   {
  //     manager.takePose(Pose::loadFromFile("test.pose"), 0.2);
  //     manager.takePose(Pose::loadFromFile("test1.pose"), 0.2);
  //   }
  // return (0);
  motion.setStiffnesses("Body", 0);
  std::cout << "Prepare your pose and press enter" << std::endl;
  getchar();
  Pose res = manager.getRobotPose();
  motion.setStiffnesses("Body", 1);
  res.setName(av[2]);
  res.saveToFile(std::string(av[2]) + ".pose");
  return (0);
}
