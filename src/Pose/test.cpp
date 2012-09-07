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

  Pose poseInFile = Pose::loadFromFile("arm.pose");
  std::cout << poseInFile << std::endl;

  Pose pose2("Nao");
  pose2.setAngle("Arm", 20);
  pose2.setAngle("Foot", 20.456);
  pose2.setAngle("Head", 5.4321);
  pose2.saveToFile("arm.pose");

  manager.takePose(stand, 2);
  Pose pose1;
  pose1.setAngle("Arm", 20);
  pose1.saveToFile("arm.pose");

  // manager.takePose(stand); 
  return (0);
}
