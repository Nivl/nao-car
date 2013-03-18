//
// RemoteServer.hpp
// NaoCar Remote Server
//

#ifndef __REMOTE_SERVER_HPP__
# define __REMOTE_SERVER_HPP__

# include <alcommon/almodule.h>
# include <boost/asio.hpp>
# include <boost/thread/thread.hpp>

# include "Bonjour.hpp"
# include "BonjourDelegate.hpp"
# include "Network/BoostTcpServer.h"
# include "Network/BoostTcpSocket.h"
# include "Network/ITcpServerDelegate.h"
# include "Network/ITcpSocketDelegate.h"
# include "DriveProxy.hpp"
# include "StreamServer.hpp"
# include "AutoDriving.hpp"
# include "VoiceSpeaker.hpp"

namespace AL
{
  class ALBroker;
}

class RemoteServer : public AL::ALModule,
		     public BonjourDelegate,
		     public Network::ITcpServerDelegate,
		     public Network::ITcpSocketDelegate
{
public:

  RemoteServer(boost::shared_ptr<AL::ALBroker> broker,
	       const std::string &name);
  virtual ~RemoteServer();

  virtual void	init();

  virtual void serviceRegistered(bool error, std::string const& name="");

  void networkThread();

  virtual void newConnection(Network::ATcpServer* sender,
			     Network::ATcpSocket* socket);

  virtual void    connected(Network::ASocket* sender,
			    Network::ASocket::Error error);
  virtual void    readFinished(Network::ASocket* sender,
			       Network::ASocket::Error error,
			       size_t bytesRead);
  virtual void    readFinished(Network::ASocket* sender,
			       Network::ASocket::Error error,
			       std::string const& buffer);
  virtual void    writeFinished(Network::ASocket* sender,
				Network::ASocket::Error error,
				size_t bytesWritten);

private:
  void	_writeHttpResponse(Network::ATcpSocket* target,
			   boost::asio::const_buffer const& buffer,
			   std::string const& code = "200 OK", const std::string& contentType = "text/plain");
  void	_parseReceivedData(Network::ATcpSocket* sender,
			   std::string const& data);
  void	_writeData(Network::ATcpSocket* target,
		   std::stringstream *buffer);

  void	defaultParams(Network::ATcpSocket* socket,
		      std::map<std::string, std::string>& params);
  void	getStreamPort(Network::ATcpSocket* socket,
		      std::map<std::string, std::string>& params);
  void	begin(Network::ATcpSocket* socket,
	      std::map<std::string,std::string>& params);
  void	end(Network::ATcpSocket* socket,
	    std::map<std::string,std::string>& params);
  void	goFrontwards(Network::ATcpSocket* socket,
		     std::map<std::string,std::string>& params);
  void	goBackwards(Network::ATcpSocket* socket,
		    std::map<std::string,std::string>& params);
  void	turnLeft(Network::ATcpSocket* socket,
		 std::map<std::string,std::string>& params);
  void	turnRight(Network::ATcpSocket* socket,
		  std::map<std::string,std::string>& params);
  void	turnFront(Network::ATcpSocket* socket,
		  std::map<std::string,std::string>& params);
  void	stop(Network::ATcpSocket* socket,
	     std::map<std::string,std::string>& params);
  void	steeringWheelAction(Network::ATcpSocket* socket,
			    std::map<std::string,std::string>& params);
  void	funAction(Network::ATcpSocket* socket,
		  std::map<std::string,std::string>& params);
  void	carambarAction(Network::ATcpSocket* socket,
		       std::map<std::string,std::string>& params);
  void	setHead(Network::ATcpSocket* socket,
		std::map<std::string,std::string>& params);
  void	talk(Network::ATcpSocket* socket,
	     std::map<std::string, std::string>& params);
  void	autoDriving(Network::ATcpSocket* socket,
		    std::map<std::string,std::string>& params);
  void	upShift(Network::ATcpSocket* socket,
		std::map<std::string,std::string>& params);
  void	downShift(Network::ATcpSocket* socket,
		std::map<std::string,std::string>& params);
  void	releasePedal(Network::ATcpSocket* socket,
		     std::map<std::string,std::string>& params);
  void	pushPedal(Network::ATcpSocket* socket,
		  std::map<std::string,std::string>& params);

  typedef void (RemoteServer::*GetFunction)
  (Network::ATcpSocket* socket,
   std::map<std::string, std::string>&);

  boost::asio::io_service		*_ioService;
  Bonjour				_bonjour;
  boost::thread				*_networkThread;
  Network::BoostTcpServer		*_tcpServer;
  std::list<Network::ATcpSocket*>	_clients;
  static std::map<std::string, GetFunction>			  _getFunctions;
  std::list<std::pair<Network::ATcpSocket*, std::stringstream*> > _toWrite;
  DriveProxy	_drive;
  StreamServer	*_streamServer;
  int		_streamPort;
  AutoDriving*	_autoDriving;

  VoiceSpeaker	_voiceSpeaker;
};

#endif
