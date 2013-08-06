//
// BonjourDelegate.hpp
// NaoCar Remote Server
//

#ifndef _BONJOUR_HPP_
# define _BONJOUR_HPP_

# include <QObject>
# include <QSocketNotifier>
# include <dns_sd.h>
# include <map>

class BonjourDelegate;

class Bonjour : public QObject {
  Q_OBJECT
public:

  enum BrowsingType {
    BrowsingAdd,
    BrowsingRemove
  };

  Bonjour(BonjourDelegate* delegate=NULL);
  ~Bonjour(void);
  
  bool browseServices(std::string const& type);
  bool resolveService(std::string const& name,
		      std::string const& type,
		      std::string const& domain);

  static void browseCallback(DNSServiceRef sdRef,
			     DNSServiceFlags flags,
			     uint32_t interfaceIndex,
			     DNSServiceErrorType errorCode,
			     const char *serviceName,
			     const char *regtype,
			     const char *replyDomain,
			     void *context);

  static void resolveHostNameCallback(DNSServiceRef sdRef,
				      DNSServiceFlags flags,
				      uint32_t interfaceIndex,
				      DNSServiceErrorType errorCode,
				      const char *fullname,
				      const char *hosttarget,
				      uint16_t port,
				      uint16_t txtLen,
				      const unsigned char *txtRecord,
				      void *context);

  static void resolveAddressCallback(DNSServiceRef sdRef,
				     DNSServiceFlags flags,
				     uint32_t interfaceIndex,
				     DNSServiceErrorType errorCode,
				     const char *hostname,
				     const struct sockaddr *address,
				     uint32_t ttl,
				     void *context);


public slots:
  void readHandler(int);

private:
  BonjourDelegate*		_delegate;
  DNSServiceRef			_browseDnssref;
  QSocketNotifier*		_browseSocketNotifier;
  std::map<int, DNSServiceRef>	_resolveDnssrefs;
  std::map<int, DNSServiceRef>	_resolveAddrDnssrefs;
  unsigned short		_resolvingPort;
};

#endif
