//
// BonjourDelegate.hpp
// NaoCar Remote Server
//

#ifndef _BONJOUR_HPP_
# define _BONJOUR_HPP_

# include <boost/asio.hpp>

class BonjourDelegate;

#define AVAHI_PUBLISH_COMMAND "avahi-publish-service"

class Bonjour {
public:

  Bonjour(boost::asio::io_service& ioService,
	  BonjourDelegate* delegate=NULL);
  ~Bonjour(void);
  
  bool registerService(std::string const& name,
		       std::string const& type,
		       unsigned short port);

  void pipeReadHandler(const boost::system::error_code& error,
		       std::size_t size);
  
private:
  boost::asio::io_service&		_ioService;
  BonjourDelegate*			_delegate;
  int					_avahiPid;
  boost::asio::posix::stream_descriptor	_pipeStream;
  boost::asio::streambuf		_pipeBuffer;
};

#endif
