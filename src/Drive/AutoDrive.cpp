//
// AutoDrive.cpp for  in /home/olivie_a//naoqi-sdk-1.12.5-linux64/examples/autoDrive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep  5 23:47:17 2012 samuel olivier
// Last update Fri Sep 28 19:18:31 2012 samuel olivier
//

#include <iostream>
#include <alcommon/albroker.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <iostream>
#include <stdlib.h>
#include <qi/os.hpp>
#include <alcommon/almodule.h>
#include <alcommon/albrokermanager.h>
#include <alcommon/altoolsmain.h>

#include "AutoDrive.hpp"

AutoDrive::AutoDrive(boost::shared_ptr<AL::ALBroker> broker,
		     const std::string &name) :
  AL::ALModule(broker, name)
{
  setModuleDescription("The NaoCar Auto Driving Module");
  functionName("start", getName(), "Set the nao ready for starting");
  BIND_METHOD(AutoDrive::start)
  functionName("stop", getName(), "Stop the nao auto driving");
  BIND_METHOD(AutoDrive::stop);
  _proxy = new DriveProxy(broker);
}

AutoDrive::~AutoDrive()
{
}

void	AutoDrive::init()
{
}

void	AutoDrive::start()
{
}

void	AutoDrive::stop()
{
}

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
    AL::ALModule::createModule<AutoDrive>(broker, "AutoDrive");
    return 0;
  }

  ALCALL int _closeModule()
  {
    return 0;
  }
}

#ifdef AUTODRIVE_IS_REMOTE
int main(int argc, char* argv[])
{
  // pointer to createModule
  TMainType sig;
  sig = &_createModule;
  // call main
  return ALTools::mainFunction("AutoDrive", argc, argv, sig);
}
#endif
