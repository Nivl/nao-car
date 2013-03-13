//
// Bonjour.cpp
// NaoCar Remote Server
//

#include "Bonjour.hpp"

#include <boost/bind.hpp>

#include "BonjourDelegate.hpp"

Bonjour::Bonjour(boost::asio::io_service& ioService, BonjourDelegate* delegate)
  : _ioService(ioService), _delegate(delegate), _dnssref(NULL),
    _registerSocket(_ioService) {
  
}

Bonjour::~Bonjour(void) {
  if (_dnssref)
    DNSServiceRefDeallocate(_dnssref);
}

bool Bonjour::registerService(std::string const& name,
			      std::string const& type,
			      unsigned short port) {
  DNSServiceErrorType err =
    DNSServiceRegister(&_dnssref,
		       0, 0, name.c_str(),
		       type.c_str(),
		       NULL,
		       NULL,
		       htons(port),
		       0, NULL, &Bonjour::registerCallback,
		       this);
  if (err != kDNSServiceErr_NoError) {
    return false;
  }
  int sockfd = DNSServiceRefSockFD(_dnssref);
  if (sockfd == -1) {
    return false;
  }
  _registerSocket = boost::asio::ip::tcp::socket(_ioService,
						 boost::asio::ip::tcp::v4(),
						 sockfd);
  boost::asio::async_read(_registerSocket,
  			  boost::asio::buffer((char*)NULL, 0),
  			  boost::bind(&Bonjour::readHandler, this,
  				      boost::asio::placeholders::error,
  				      &_registerSocket));
  return true;
}

void Bonjour::readHandler(const boost::system::error_code& error,
			  boost::asio::ip::tcp::socket* socket) {
  if (error) {
    if (_delegate) {
      _delegate->serviceRegistered(true);
    }
    return ;
  }
  DNSServiceErrorType err =
    DNSServiceProcessResult(_dnssref);
  if (err != kDNSServiceErr_NoError) {
    if (_delegate) {
      _delegate->serviceRegistered(true);
    }
    return ;
  }
  
}

void Bonjour::registerCallback(DNSServiceRef, DNSServiceFlags,
			       DNSServiceErrorType errorCode, const char *name,
			       const char *regType, const char *domain, void *context) {
  Bonjour* bonjour = static_cast<Bonjour*>(context);
  if (bonjour->_delegate) {
    bonjour->_delegate->serviceRegistered(false, name);
  }
}
