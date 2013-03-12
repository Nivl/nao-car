//
// RemoteServer.cpp
// NaoCar Remote Server
//

#include "RemoteServer.hpp"

#include <iostream>
#include <arpa/inet.h>
#include <qi/os.hpp>
#include <alcommon/albroker.h>
#include <alcommon/almodule.h>
#include <alcommon/albrokermanager.h>
#include <alcommon/altoolsmain.h>
#include <dns_sd.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

DNSServiceRef dnssref = NULL;

RemoteServer::RemoteServer(boost::shared_ptr<AL::ALBroker> broker,
			   const std::string &name) :
  AL::ALModule(broker, name)
{
  setModuleDescription("NaoCar Remote server");
}

RemoteServer::~RemoteServer()
{
}

void test(DNSServiceRef, DNSServiceFlags,
	  DNSServiceErrorType errorCode, const char *name,
	  const char *regType, const char *domain, void *data)
{
  std::cout << "Yo!\n";
  if (errorCode != kDNSServiceErr_NoError) {
    std::cerr << "Error\n";
  }
  std::cout << errorCode << std::endl;
  std::cout << name << std::endl;
  std::cout << regType << std::endl;
  std::cout << domain << std::endl;
}

void read(const boost::system::error_code& error)
{
  if (error) {
    std::cout << "error\n";
  }
  std::cout << "READ!\n";
  DNSServiceErrorType err =
    DNSServiceProcessResult(dnssref);
  if (err != kDNSServiceErr_NoError) {
    std::cerr << "Service process error" << std::endl;
  } else {
    std::cout << "ok\n";
  }
}

void	RemoteServer::init()
{
  boost::asio::io_service io_service;

  DNSServiceErrorType err =
    DNSServiceRegister(&dnssref,
		       0, 0, "NaoCar Remote Server",
		       "_http._tcp",
		       NULL,
		       NULL,
		       htons(4242),
		       0, NULL, test,
		       this);
  if (err != kDNSServiceErr_NoError) {
    std::cerr << "Error: cannot register service" << std::endl;
  } else {    
    int sockfd = DNSServiceRefSockFD(dnssref);
    if (sockfd == -1) {
      std::cerr << "Error: cannot register service" << std::endl;
    } else {
      boost::asio::ip::tcp::socket* socket =
	new boost::asio::ip::tcp::socket(io_service, boost::asio::ip::tcp::v4(), sockfd);
      boost::asio::async_read(*socket, boost::asio::buffer((char*)NULL, 0), boost::bind(read, boost::asio::placeholders::error));
    }
  }

  io_service.run();
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
