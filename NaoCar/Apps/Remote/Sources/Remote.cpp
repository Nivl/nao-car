//
// Remote.cpp
// NaoCar Remote
//

#include "Remote.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <SFML/Window/Joystick.hpp>
#include <QSocketNotifier>

#include "dns_sd.h"

DNSServiceRef dnssref = NULL;
DNSServiceRef dnssref2 = NULL;

void bonjourResolveReply(DNSServiceRef sdRef,
			 DNSServiceFlags flags,
			 uint32_t interfaceIndex,
			 DNSServiceErrorType errorCode,
			 const char *fullname,
			 const char *hosttarget,
			 uint16_t port, /* In network byte order */
			 uint16_t txtLen,
			 const unsigned char *txtRecord,
			 void *context)
{
  if (errorCode != kDNSServiceErr_NoError) {
    std::cerr << "Resolving error\n";
    return;
  }
     
  port = ntohs(port);
  std::cout << "resolved!\n"
	    << fullname << "\n"
	    << hosttarget << "\n"
	    << port << std::endl;
//   QHostInfo::lookupHost(QString::fromUtf8(hostTarget),
// 			resolver, SLOT(finishConnect(const QHostInfo &)));
}

void bonjourBrowseReply(DNSServiceRef,
			DNSServiceFlags flags, quint32,
			DNSServiceErrorType errorCode,
			const char *serviceName, const char *regType,
			const char *replyDomain, void *context) {
  Remote* remote = (Remote*)context;
  std::cout << "BROWSE!\n";
  if (errorCode != kDNSServiceErr_NoError) {
    std::cerr << "error!\n";
  } else {
    std::cout << serviceName << "\n";
    // Now resolve the service host
    DNSServiceErrorType err =
      DNSServiceResolve(&dnssref2, 0, 0,
			serviceName,
			regType,
			replyDomain,
			bonjourResolveReply, remote);
    if (err != kDNSServiceErr_NoError) {
      std::cerr << "error: cannot resolve\n";
    } else {
      int sockfd = DNSServiceRefSockFD(dnssref2);
      if (sockfd == -1) {
	std::cerr << "error: cannot resolve2\n";
      } else {
	QSocketNotifier* bonjourSocket = new QSocketNotifier(sockfd,
							     QSocketNotifier::Read, remote);
	QObject::connect(bonjourSocket, SIGNAL(activated(int)),
			 remote, SLOT(bonjourSocketReadyRead2()));
      }
    }
  }
}

void Remote::bonjourSocketReadyRead(void) {
  std::cout << "ready read\n";
  DNSServiceErrorType err =
    DNSServiceProcessResult(dnssref);
  if (err != kDNSServiceErr_NoError)
    std::cout << "error: cannot process browser" << std::endl;    
}

void Remote::bonjourSocketReadyRead2(void) {
  std::cout << "ready read2\n";
  DNSServiceErrorType err =
    DNSServiceProcessResult(dnssref2);
  if (err != kDNSServiceErr_NoError)
    std::cout << "error: cannot process browser" << std::endl;    
}

Remote::Remote(int argc, char** argv)
  : _app(argc, argv), _mainWindow(this), _gamepadId(0) {

  
  DNSServiceErrorType err =
    DNSServiceBrowse(&dnssref, 0, 0,
		     "_http._tcp", NULL,
		     bonjourBrowseReply, this);
  if (err != kDNSServiceErr_NoError) {
    std::cout << "error: cannot browse" << std::endl;
  } else {
    int sockfd = DNSServiceRefSockFD(dnssref);
    if (sockfd == -1) {
      std::cout << "error: cannot browse2" << std::endl;
    } else {
      QSocketNotifier* bonjourSocket = new QSocketNotifier(sockfd,
							   QSocketNotifier::Read, this);
      QObject::connect(bonjourSocket, SIGNAL(activated(int)),
		       this, SLOT(bonjourSocketReadyRead()));
    }
  }
  
  chooseGamepad();
}

Remote::~Remote(void) {
  
}


int Remote::exec(void) {
  return (_app.exec());
}

void Remote::chooseGamepad(void) {
  sf::Joystick::update();
  for (int i = 0; i < sf::Joystick::Count; ++i) {
    if (sf::Joystick::isConnected(i)) {
      // The Xbox gamepad has 11 buttons
      if (sf::Joystick::getButtonCount(i) == 11) {
	_gamepadId = i;
	_mainWindow.setGamepadId(_gamepadId);
	return ;
      }
    }
  }
}

void Remote::connect(void) {
  
}

void Remote::disconnect(void) {
  
}

void Remote::viewChanged(int index) {
  
}

void Remote::gamepadIDChanged(int id) {
  _gamepadId = id;
}

void Remote::takeCarambar(void) {
  
}

void Remote::giveCarambar(void) {
  
}

void Remote::talk(std::string message) {
  
}

void Remote::autoDriving(void) {
  
}
