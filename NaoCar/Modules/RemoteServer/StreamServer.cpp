//
// StreamServer.cpp for  in /home/olivie_a//rendu/nao/nao-car/NaoCar/Modules/RemoteServer
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Mar 15 16:30:36 2013 samuel olivier
// Last update Fri Mar 15 18:09:34 2013 samuel olivier
//

#include "StreamServer.hpp"

StreamServer::StreamServer(boost::asio::io_service* service) :
  _ioService(service), _mainThread(NULL), _tcpServer(NULL),
  _stop(false)
{
}

StreamServer::~StreamServer() {
  for (auto it = _clients.begin(); it != _clients.end(); ++it) {
    (*it)->close();
    delete *it;
  }
  stop();
  delete _tcpServer;
}

int	StreamServer::run() {
  if (_mainThread == NULL) {
    if (_tcpServer == NULL) {
      _tcpServer = new Network::BoostTcpServer(_ioService);
      _tcpServer->setDelegate(this);
      if (_tcpServer->listen(0, "") == false) {
	std::cerr << "could not listen on this port" << std::endl;
	return (0);
      }
    }
    _stop = false;
    _mainThread = new boost::thread(&StreamServer::mainThread, this);
  }
  return (_tcpServer->getPort());
}

void	StreamServer::stop() {
  if (_mainThread != NULL) {
    _stop = true;
    _mainThread->join();
    delete _mainThread;
    _mainThread = NULL;
  }
  _stop = false;
}

void	StreamServer::mainThread() {
  while (_stop == false) {
    _clientsMutex.lock();
    for (auto it = _clients.begin(); it != _clients.end(); ++it) {
      char	*data = new char[12];
      memcpy(data, "Yo les mecs\n", 12);
      _writeData(*it, data, 12);
    }
    _clientsMutex.unlock();
    usleep(10000);
  }
}

void	StreamServer::newConnection(Network::ATcpServer* sender,
				    Network::ATcpSocket* socket) {
  if (_tcpServer != sender)
    return ;
  socket->setDelegate(this);
  _clientsMutex.lock();
  _clients.push_back(socket);
  _clientsMutex.unlock();
  socket->readUntil("\n");
  std::cout << "Stream Connection " << _clients.size() << std::endl;
}

void	StreamServer::connected(Network::ASocket*,
				Network::ASocket::Error) {
}

void	StreamServer::readFinished(Network::ASocket*,
				   Network::ASocket::Error,
				   size_t) {
}

void	StreamServer::readFinished(Network::ASocket* sender,
				   Network::ASocket::Error error,
				   std::string const&) {
  Network::ATcpSocket	*socket = dynamic_cast<Network::ATcpSocket*>(sender);

  if (socket == NULL)
    return ;
  if (error) {
    _clientsMutex.lock();
    _clients.remove(socket);
    _clientsMutex.unlock();
    delete socket;
    std::cout << "Stream Deconnection " << _clients.size() << std::endl;
  } else {
    socket->readUntil("\n");
  }
}

void	StreamServer::writeFinished(Network::ASocket*,
				    Network::ASocket::Error,
				    size_t) {
  delete []_toWrite.front().second->data;
  delete _toWrite.front().second;
  _toWrite.pop_front();
  if (_toWrite.size() >= 1)
    _toWrite.front().first->write(_toWrite.front().second->data,
				  _toWrite.front().second->size);
}

void	StreamServer::_writeData(Network::ATcpSocket* target,
				 char* data, size_t size) {
  Packet	*packet = new Packet;
  packet->data = data;
  packet->size = size;
  _toWrite.push_back(std::pair<Network::ATcpSocket*, Packet*>(target, packet));
  if (_toWrite.size() == 1)
    target->write(data, size);
}

