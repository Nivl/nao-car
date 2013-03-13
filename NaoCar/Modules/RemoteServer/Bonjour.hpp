//
// BonjourDelegate.hpp
// NaoCar Remote Server
//

#ifndef _BONJOUR_HPP_
# define _BONJOUR_HPP_

# include <boost/asio.hpp>
# include <dns_sd.h>

class BonjourDelegate;

class Bonjour {
public:
  Bonjour(boost::asio::io_service& ioService,
	  BonjourDelegate* delegate=NULL);
  ~Bonjour(void);
  
  bool registerService(std::string const& name,
		       std::string const& type,
		       unsigned short port);
  
  void readHandler(const boost::system::error_code& error,
		   boost::asio::ip::tcp::socket* socket);

  static void registerCallback(DNSServiceRef, DNSServiceFlags,
			       DNSServiceErrorType errorCode, const char *name,
			       const char *regType, const char *domain, void *context);

private:
  boost::asio::io_service&	_ioService;
  BonjourDelegate*		_delegate;
  DNSServiceRef			_dnssref;
  boost::asio::ip::tcp::socket	_registerSocket;
};

#endif
