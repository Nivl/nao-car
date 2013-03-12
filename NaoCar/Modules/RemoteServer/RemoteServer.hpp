//
// RemoteServer.hpp
// NaoCar Remote Server
//

#ifndef __REMOTE_SERVER_HPP__
# define __REMOTE_SERVER_HPP__

# include <alcommon/almodule.h>

namespace AL
{
  class ALBroker;
}

class RemoteServer : public AL::ALModule
{
public:

  RemoteServer(boost::shared_ptr<AL::ALBroker> broker,
	       const std::string &name);
  virtual ~RemoteServer();

  virtual void	init();

private:
  boost::shared_ptr<AL::ALBroker>	_broker;
};

#endif
