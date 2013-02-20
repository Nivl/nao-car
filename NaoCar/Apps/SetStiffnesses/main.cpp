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
  if (ac != 4)
    {
      std::cout << "Usage: " << av[0] << "ip part_name value" << std::endl;
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

  AL::ALMotionProxy	motion(broker);
  motion.setStiffnesses(av[2], atof(av[3]));
  return (0);
}
