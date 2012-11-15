//
// main_auto.cpp for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Nov 14 12:37:55 2012 samuel olivier
// Last update Wed Nov 14 13:52:49 2012 samuel olivier
//


#include <alcommon/almodulecore.h>
#include <alcommon/almoduleinfo.h>
#include <alcommon/albroker.h>

#include <iostream>

#include "AutoDriveProxy.hpp"

int	main(int ac, char **av)
{
  boost::shared_ptr<AL::ALBroker> broker;
  try
    {
      broker = AL::ALBroker::createBroker("broker","0.0.0.0",0,av[1],9559,0);
    }
  catch (...)
    {
      std::cerr << "Error" << std::endl;
      return (1);
    }
  AutoDriveProxy *autoProxy = new AutoDriveProxy(broker);
  autoProxy->start();
  getchar();
  autoProxy->stop();
  return (0);
}
