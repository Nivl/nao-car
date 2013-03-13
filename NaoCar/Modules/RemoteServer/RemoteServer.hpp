//
// RemoteServer.hpp
// NaoCar Remote Server
//

#ifndef __REMOTE_SERVER_HPP__
# define __REMOTE_SERVER_HPP__

# include <alcommon/almodule.h>
# include <boost/asio.hpp>

# include "Bonjour.hpp"
# include "BonjourDelegate.hpp"

namespace AL
{
  class ALBroker;
}

class RemoteServer : public AL::ALModule, public BonjourDelegate
{
public:

  RemoteServer(boost::shared_ptr<AL::ALBroker> broker,
	       const std::string &name);
  virtual ~RemoteServer();

  virtual void	init();

  virtual void serviceRegistered(bool error, std::string const& name="");

private:
  boost::asio::io_service		_ioService;
  Bonjour				_bonjour;
};

#endif
