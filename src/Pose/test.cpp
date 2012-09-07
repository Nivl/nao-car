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

  std::cout << "Waiting for Pose 1" << std::endl;
  getchar();
  Pose pose = manager.getRobotPose();
  std::cout << "Waiting for Pose 2" << std::endl;
  getchar();
  Pose pose3 = manager.getRobotPose();
  // Pose stand("stand.pose");

  // Pose poseInFile = Pose::loadFromFile("arm.pose");
  // std::cout << poseInFile << std::endl;

  // Pose pose2("Nao");
  // pose2.setAngle("Arm", 20);
  // pose2.setAngle("Foot", 20.456);
  // pose2.setAngle("Head", 5.4321);
  // pose2.saveToFile("arm.pose");

  // manager.takePose(stand, 2);
  // Pose pose1;
  // pose1.setAngle("Arm", 20);
  // pose1.saveToFile("arm.pose");

  std::cout << "Waiting for Initial Pose" << std::endl;
  getchar();
  AL::ALMotionProxy                     motion(broker);
  motion.setStiffnesses("Body", 1);
  std::cout << "Stiffnesses set to 1, waiting for starting move" << std::endl;
  getchar();
  manager.takePose(pose, 1);
  manager.takePose(pose3, 1);
  std::cout << "Waiting for ending" << std::endl;
  getchar();
  motion.setStiffnesses("Body", 0);
  return (0);
}
