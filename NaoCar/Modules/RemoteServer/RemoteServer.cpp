//
// RemoteServer.cpp
// NaoCar Remote Server
//

#include "RemoteServer.hpp"

#include <iostream>
#include <alcommon/albroker.h>
#include <alcommon/almodule.h>
#include <alcommon/albrokermanager.h>
#include <alcommon/altoolsmain.h>
#include <dns_sd.h>

RemoteServer::RemoteServer(boost::shared_ptr<AL::ALBroker> broker,
			   const std::string &name) :
  AL::ALModule(broker, name), _ioService(), _bonjour(_ioService, this)
{
  setModuleDescription("NaoCar Remote server");
}

RemoteServer::~RemoteServer()
{
}

void	RemoteServer::init()
{
  _bonjour.registerService("nao-car", "_http._tcp", 424);
  _ioService.run();
}

void	RemoteServer::serviceRegistered(bool error, std::string const& name) {
  if (error) {
    std::cout << "Cannot register Bonjour service" << std::endl;
  } else {
    std::cout << "\"" << name << "\"" << " Bonjour service registered" << std::endl;
  }
}


//! Naoqi module registration

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
    AL::ALModule::createModule<RemoteServer>(broker, "RemoteServer");
    return 0;
  }

  ALCALL int _closeModule()
  {
    return 0;
  }
}

#ifdef REMOTE_SERVER_IS_REMOTE
int main(int argc, char* argv[])
{
  // pointer to createModule
  TMainType sig;
  sig = &_createModule;
  // call main
  return ALTools::mainFunction("RemoteServer", argc, argv, sig);
}
#endif
