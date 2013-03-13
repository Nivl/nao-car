//
// Bonjour.cpp
// NaoCar Remote Server
//

#include "Bonjour.hpp"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "BonjourDelegate.hpp"

Bonjour::Bonjour(BonjourDelegate* delegate)
  : _delegate(delegate), _browseDnssref(NULL),
    _browseSocketNotifier(NULL), _resolvingPort(0) {
}

Bonjour::~Bonjour(void) {
  if (_browseDnssref)
    DNSServiceRefDeallocate(_browseDnssref);
}

bool Bonjour::browseServices(std::string const& type) {
  DNSServiceErrorType err =
    DNSServiceBrowse(&_browseDnssref, 0, 0,
		     type.c_str(), NULL,
		     &Bonjour::browseCallback, this);
  if (err != kDNSServiceErr_NoError)
    return false;
  int sockfd = DNSServiceRefSockFD(_browseDnssref);
  if (sockfd == -1)
    return false;
  _browseSocketNotifier =
    new QSocketNotifier(sockfd, QSocketNotifier::Read, this);
  QObject::connect(_browseSocketNotifier, SIGNAL(activated(int)),
		   this, SLOT(readHandler(int)));  
  return true;
}

bool Bonjour::resolveService(std::string const& name,
			     std::string const& type,
			     std::string const& domain) {
  DNSServiceRef dnssref = NULL;
  DNSServiceErrorType err =
    DNSServiceResolve(&dnssref, 0, 0,
		      name.c_str(), type.c_str(), domain.c_str(),
		      &Bonjour::resolveHostNameCallback, this);
  if (err != kDNSServiceErr_NoError) {
    DNSServiceRefDeallocate(dnssref);
    return false;    
  }
  int sockfd = DNSServiceRefSockFD(dnssref);
  if (sockfd == -1) {
    DNSServiceRefDeallocate(dnssref);
    return false;
  }
  _resolveDnssrefs[sockfd] = dnssref;
  QSocketNotifier* notifier =
    new QSocketNotifier(sockfd, QSocketNotifier::Read, this);
  QObject::connect(notifier, SIGNAL(activated(int)),
		   this, SLOT(readHandler(int)));
  return true;  
}

void Bonjour::readHandler(int socket) {
  DNSServiceRef dnssref = NULL;
  std::map<int, DNSServiceRef>::iterator it;
  if ((it = _resolveDnssrefs.find(socket)) != _resolveDnssrefs.end()) {
    dnssref = _resolveDnssrefs[socket];
    _resolveDnssrefs.erase(it);
    sender()->deleteLater();
  } else if ((it = _resolveAddrDnssrefs.find(socket)) != _resolveAddrDnssrefs.end()) {
    dnssref = _resolveAddrDnssrefs[socket];
    _resolveAddrDnssrefs.erase(it);
    delete sender();
  } else {
    dnssref = _browseDnssref;
  }
  DNSServiceErrorType err =
    DNSServiceProcessResult(dnssref);
  if (err != kDNSServiceErr_NoError) {
    if (_delegate) {
      _delegate->serviceBrowsed(true);
    }
    return ;
  }
}

void Bonjour::browseCallback(DNSServiceRef ,
			     DNSServiceFlags flags,
			     uint32_t ,
			     DNSServiceErrorType errorCode,
			     const char *serviceName,
			     const char *regType,
			     const char *replyDomain,
			     void *context) {
  Bonjour* bonjour = static_cast<Bonjour*>(context);
  if (errorCode != kDNSServiceErr_NoError) {
    if (bonjour->_delegate) {
      bonjour->_delegate->serviceBrowsed(true);
    }
    return ;
  }
  if (bonjour->_delegate) {
    BrowsingType type = (flags == kDNSServiceFlagsAdd) ? BrowsingAdd : BrowsingRemove;
    bonjour->_delegate->serviceBrowsed(false, type, serviceName, regType, replyDomain);
  }  
}

void Bonjour::resolveHostNameCallback(DNSServiceRef sdRef,
				      DNSServiceFlags ,
				      uint32_t ,
				      DNSServiceErrorType errorCode,
				      const char *,
				      const char *hosttarget,
				      uint16_t port,
				      uint16_t ,
				      const unsigned char *,
				      void *context) {
  DNSServiceRefDeallocate(sdRef);
  Bonjour* bonjour = static_cast<Bonjour*>(context);
  if (errorCode != kDNSServiceErr_NoError) {
    if (bonjour->_delegate) {
      bonjour->_delegate->serviceResolved(true);
    }
    return ;
  }
  // Now resolve ip !
  bonjour->_resolvingPort = port;
  DNSServiceRef dnssref = NULL;
  DNSServiceErrorType err =
    DNSServiceGetAddrInfo(&dnssref, 0, 0,
			  kDNSServiceProtocol_IPv4,
			  hosttarget,
			  &Bonjour::resolveAddressCallback, bonjour);
  if (err != kDNSServiceErr_NoError) {
    DNSServiceRefDeallocate(dnssref);
    if (bonjour->_delegate) {
      bonjour->_delegate->serviceResolved(true);
    }
    return ;
  }
  int sockfd = DNSServiceRefSockFD(dnssref);
  if (sockfd == -1) {
    DNSServiceRefDeallocate(dnssref);
    if (bonjour->_delegate) {
      bonjour->_delegate->serviceResolved(true);
    }
    return ;
  }
  bonjour->_resolveAddrDnssrefs[sockfd] = dnssref;
  QSocketNotifier* notifier =
    new QSocketNotifier(sockfd, QSocketNotifier::Read, bonjour);
  QObject::connect(notifier, SIGNAL(activated(int)),
		   bonjour, SLOT(readHandler(int)));
}

void Bonjour::resolveAddressCallback(DNSServiceRef sdRef,
				     DNSServiceFlags ,
				     uint32_t ,
				     DNSServiceErrorType errorCode,
				     const char *hostname,
				     const struct sockaddr *address,
				     uint32_t ,
				     void *context) {
  DNSServiceRefDeallocate(sdRef);
  Bonjour* bonjour = static_cast<Bonjour*>(context);
  if (errorCode != kDNSServiceErr_NoError) {
    if (bonjour->_delegate) {
      bonjour->_delegate->serviceResolved(true);
    }
    return ;
  }
  const struct sockaddr_in* addr = (const struct sockaddr_in*)address;
  std::string ip = inet_ntoa(addr->sin_addr);
  if (bonjour->_delegate) {
    bonjour->_delegate->serviceResolved(false, hostname, ip, bonjour->_resolvingPort);
  }
}
