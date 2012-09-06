//
// main.cpp for  in /home/olivie_a//naoqi-sdk-1.12.5-linux64/examples/drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep  5 23:56:24 2012 samuel olivier
// Last update Thu Sep  6 21:36:51 2012 samuel olivier
//

#include <iostream>
#include <stdlib.h>
#include <qi/os.hpp>

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>
#include <alcommon/altoolsmain.h>

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

#ifdef POSE_IS_REMOTE

int main(int argc, char *argv[])
{
  // pointer to createModule                                                                                                                               
  TMainType sig;
  sig = &_createModule;
  // call main                                                                                                                                             
  return ALTools::mainFunction("Drive", argc, argv, sig);
}
#endif
