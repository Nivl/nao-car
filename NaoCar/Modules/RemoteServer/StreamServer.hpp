//
// StreamServer.hpp for  in /home/olivie_a//rendu/nao/nao-car/NaoCar/Modules/RemoteServer
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Mar 15 16:21:34 2013 samuel olivier
// Last update Sat Mar 16 20:20:21 2013 samuel olivier
//

#ifndef __STREAM_SERVER_HPP__
# define __STREAM_SERVER_HPP__

# include <alcommon/almodule.h>
# include <boost/asio.hpp>
# include <boost/thread/thread.hpp>
# include <atomic>
# include <mutex>
# include <gst/gst.h>
# include <glib.h>
# include <gst/app/gstappsink.h>

# include "Network/BoostTcpServer.h"
# include "Network/BoostTcpSocket.h"
# include "Network/ITcpServerDelegate.h"
# include "Network/ITcpSocketDelegate.h"

namespace AL
{
  class ALBroker;
}

class StreamServer : public Network::ITcpServerDelegate,
		     public Network::ITcpSocketDelegate
{
public:

  StreamServer(boost::asio::io_service* ioService);
  virtual ~StreamServer();

  int	run();
  void	stop();

  virtual void	newConnection(Network::ATcpServer* sender,
			     Network::ATcpSocket* socket);
  virtual void	connected(Network::ASocket* sender,
			    Network::ASocket::Error error);
  virtual void	readFinished(Network::ASocket* sender,
			       Network::ASocket::Error error,
			       size_t bytesRead);
  virtual void	readFinished(Network::ASocket* sender,
			       Network::ASocket::Error error,
			       std::string const& buffer);
  virtual void	writeFinished(Network::ASocket* sender,
				Network::ASocket::Error error,
				size_t bytesWritten);
  void	setImageData(char *data, size_t size);

private:
  struct Packet {
    char	*data;
    size_t	size;
  };

  void	_writeData(Network::ATcpSocket* target,
		   char* data, size_t size);
  void	_setPipeline(std::string const& pipeline);

  void	mainThread();

  boost::asio::io_service	*_ioService;
  boost::thread			*_mainThread;
  Network::BoostTcpServer	*_tcpServer;
  std::list<Network::ATcpSocket*>	_clients;
  std::mutex				_clientsMutex;
  std::list<std::pair<Network::ATcpSocket*, Packet*> > _toWrite;
  std::atomic<bool>		_stop;
  GstElement			*_pipeline;
  char				*_imageData;
  uint64_t			_imageSize;
  std::atomic<bool>		_imageChanged;
  std::mutex			_imageMutex;
};

#endif

