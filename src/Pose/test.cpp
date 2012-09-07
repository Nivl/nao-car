//
// test.cpp
//


#include <iostream>
#include <boost/shared_ptr.hpp>
#include <alerror/alerror.h>
#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>

#include <alproxies/altexttospeechproxy.h>

#include "PoseManager.hpp"

int main(int ac, char**av)
{
  if (ac != 2)
    {
      std::cout << "Usage: " << av[0] << " ip" << std::endl;
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

  Pose pose = manager.getRobotPose();
  Pose stand("stand.pose");

  manager.takePose(stand, 2);
  return (0);
}
