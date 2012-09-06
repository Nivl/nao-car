//
// main.cpp for  in /home/olivie_a//naoqi-sdk-1.12.5-linux64/examples/drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep  5 23:56:24 2012 samuel olivier
// Last update Thu Sep  6 19:29:21 2012 samuel olivier
//

#include <iostream>
#include <stdlib.h>
#include <qi/os.hpp>

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>

#include "Drive.hh"

#ifdef _WIN32
# define ALCALL __declspec(dllexport)
#else
# define ALCALL
#endif

extern "C"
{
  ALCALL int _createModule(boost::shared_ptr<AL::ALBroker> broker)
  {
    AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
    AL::ALBrokerManager::getInstance()->addBroker(broker);
    AL::ALModule::createModule<Drive>(broker, "Drive");
    return 0;
  }

  ALCALL int _closeModule(  )
  {
    return 0;
  }
}

#ifdef DRIVE_IS_REMOTE
int	main(int ac, char **av)
{
  int pport = 9559;
  std::string pip = "127.0.0.1";

  if (ac != 1 && ac != 3 && ac != 5)
    {
      std::cerr << "Wrong number of arguments!" << std::endl;
      std::cerr << "Usage: mymodule [--pip robot_ip] [--pport port]"
		<< std::endl;
      exit(2);
    }
  if (ac == 3)
    {
      if (std::string(av[1]) == "--pip")
	pip = av[2];
      else if (std::string(av[1]) == "--pport")
	pport = atoi(av[2]);
      else
	{
	  std::cerr << "Wrong number of arguments!" << std::endl;
	  std::cerr << "Usage: mymodule [--pip robot_ip] [--pport port]"
		    << std::endl;
	  exit(2);
	}
    }
  if (ac == 5)
    {
      if (std::string(av[1]) == "--pport"
	  && std::string(av[3]) == "--pip")
	{
	  pport = atoi(av[2]);
	  pip = av[4];
	}
      else if (std::string(av[3]) == "--pport"
	       && std::string(av[1]) == "--pip")
	{
	  pport = atoi(av[4]);
	  pip = av[2];
	}
      else
	{
	  std::cerr << "Wrong number of arguments!" << std::endl;
	  std::cerr << "Usage: mymodule [--pip robot_ip] [--pport port]"
		    << std::endl;
	  exit(2);
	}
    }
  setlocale(LC_NUMERIC, "C");
  const std::string brokerName = "mybroker";
  int brokerPort = 54000;
  const std::string brokerIp = "0.0.0.0";
  boost::shared_ptr<AL::ALBroker> broker;
  try
    {
      broker = AL::ALBroker::createBroker(
					  brokerName,
					  brokerIp,
					  brokerPort,
					  pip,
					  pport,
					  0
					  );
    }
  catch(...)
    {
      std::cerr << "Fail to connect broker to: "
		<< pip
		<< ":"
		<< pport
		<< std::endl;

      AL::ALBrokerManager::getInstance()->killAllBroker();
      AL::ALBrokerManager::kill();

      return 1;
    }
  AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
  AL::ALBrokerManager::getInstance()->addBroker(broker);
  AL::ALModule::createModule<Drive>(broker, "Drive");
  return 0;
}
#endif
