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
#include "Animation.hpp"

int main(int ac, char**av)
{
  if (ac <= 3 || ac % 2 == 0)
    {
      std::cout << "Usage: ./createAnimation name file [poseFilename duration ...]" << std::endl;
      return (0);
    }
  Animation anim(av[1]);
  for (int i = 0; i < (ac - 3) / 2; ++i)
    anim.addPose(av[i * 2 + 3], atof(av[i * 2 + 4]));
  anim.saveToFile(av[2]);
}
