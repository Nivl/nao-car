//
// RemoteServer.cpp
// NaoCar Remote Server
//

#include "RemoteServer.hpp"

#include <iostream>
#include <alcommon/albroker.h>
#include <alcommon/almodule.h>
#include <alcommon/albrokermanager.h>
#include <alcommon/altoolsmain.h>
#include <dns_sd.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <sstream>

#include "AutoDriving.hpp"

std::map<std::string, RemoteServer::GetFunction> RemoteServer::_getFunctions;

static bool urlDecode(const std::string& in, std::string& out);

RemoteServer::RemoteServer(boost::shared_ptr<AL::ALBroker> broker,
			   const std::string &name) :
  AL::ALModule(broker, name), _ioService(new boost::asio::io_service()),
  _bonjour(*_ioService, this), _networkThread(NULL), _tcpServer(NULL),
  _drive(broker)
{
  if (_getFunctions.size() == 0) {
    _getFunctions["/"] = &RemoteServer::defaultParams;
    _getFunctions["/get-stream-port"] = &RemoteServer::getStreamPort;
    _getFunctions["/begin"] = &RemoteServer::begin;
    _getFunctions["/end"] = &RemoteServer::end;

    _getFunctions["/go-frontwards"] = &RemoteServer::goFrontwards;
    _getFunctions["/go-backwards"] = &RemoteServer::goBackwards;
    _getFunctions["/turn-left"] = &RemoteServer::turnLeft;
    _getFunctions["/turn-right"] = &RemoteServer::turnRight;
    _getFunctions["/turn-front"] = &RemoteServer::turnFront;
    _getFunctions["/stop"] = &RemoteServer::stop;
    _getFunctions["/steeringwheel-action"] =
      &RemoteServer::steeringWheelAction;
    _getFunctions["/fun-action"] = &RemoteServer::funAction;
    _getFunctions["/carambar-action"] = &RemoteServer::carambarAction;
    _getFunctions["/setHead"] = &RemoteServer::setHead;
    _getFunctions["/start-auto-driving"] = &RemoteServer::autoDriving;
    _getFunctions["/stop-auto-driving"] = &RemoteServer::stopAutoDriving;

    
  }
  setModuleDescription("NaoCar Remote server");
  
  try {
    _autoDriving = new AutoDriving;
  } catch(...) {
    _autoDriving = NULL;
  }
}

RemoteServer::~RemoteServer()
{
  delete _streamServer;
  for (std::list<Network::ATcpSocket*>::iterator it =
	 _clients.begin(); it != _clients.end(); ++it) {
    (*it)->close();
    delete *it;
  }
  _ioService->stop();
  if (_networkThread != NULL) {
    _networkThread->join();
    delete _networkThread;
  }
  delete _tcpServer;
}

void	RemoteServer::init()
{
  _tcpServer = new Network::BoostTcpServer(_ioService);
  _tcpServer->setDelegate(this);
  if (_tcpServer->listen(0, "") == false) {
    std::cerr << "could not listen on this port" << std::endl;
    return ;
  }
  _streamServer = new StreamServer(_ioService);
  _streamPort = _streamServer->run();
  std::cout << "Server Port: " << _tcpServer->getPort() << std::endl;
  if (!_bonjour.registerService("nao-car", "_http._tcp",
  				_tcpServer->getPort()))
   std::cerr << "Could not register Bonjour service" << std::endl;
  _networkThread = new boost::thread(&RemoteServer::networkThread, this);
}

void	RemoteServer::serviceRegistered(bool error, std::string const& name) {
  if (error) {
    std::cout << "Cannot register Bonjour service" << std::endl;
  } else {
    std::cout << "\"" << name << "\"" << " Bonjour service registered" << std::endl;
  }
}

void	RemoteServer::networkThread() {
  _ioService->run();  
}

void	RemoteServer::newConnection(Network::ATcpServer* sender,
				    Network::ATcpSocket* socket) {
  if (_tcpServer != sender)
    return ;
  socket->setDelegate(this);
  _clients.push_back(socket);
  std::cout << "Connection" << _clients.size() << std::endl;
  socket->readUntil("\r\n");
}

void	RemoteServer::connected(Network::ASocket*,
				Network::ASocket::Error) {
  
}

void    RemoteServer::readFinished(Network::ASocket* sender,
				   Network::ASocket::Error error,
				   size_t) {
  Network::ATcpSocket	*socket = dynamic_cast<Network::ATcpSocket*>(sender);

  if (socket == NULL)
    return ;
  if (error) {
    _clients.remove(socket);
    delete socket;
    std::cout << "Deconnection" << _clients.size() << std::endl;
  }
}

void    RemoteServer::readFinished(Network::ASocket* sender,
				   Network::ASocket::Error error,
				   std::string const& buffer) {
  Network::ATcpSocket	*socket = dynamic_cast<Network::ATcpSocket*>(sender);

  if (socket == NULL)
    return ;
  if (error) {
    _clients.remove(socket);
    delete socket;
    std::cout << "Deconnection" << _clients.size() << std::endl;
  } else {
    _parseReceivedData(socket, buffer);
    socket->readUntil("\r\n");
  }
}

void    RemoteServer::writeFinished(Network::ASocket*,
				    Network::ASocket::Error,
				    size_t) {
  delete _toWrite.front().second;
  _toWrite.pop_front();
   if (_toWrite.size() >= 1)
     _toWrite.front().first->write(_toWrite.front().second->str().c_str(),
				   _toWrite.front().second->str().size());
}

void	RemoteServer::_parseReceivedData(Network::ATcpSocket* sender,
					 std::string const& data) {
  size_t idx = data.find("\n");
  if (idx == std::string::npos || idx < 2)
    return ;
  std::string line = data.substr(0, idx - 1);
  std::vector<std::string> words;
  boost::split(words, line, boost::is_any_of(" "));

  if (words.size() >= 3 && words[0] == "GET") {
    std::string funcName = words[1];
    std::map<std::string, std::string> params;
    idx = funcName.find("?");
    if (idx != std::string::npos) {
      std::string paramsString = funcName.substr(idx + 1);
      funcName = funcName.substr(0, idx);
      std::vector<std::string> keyValues;
      boost::split(keyValues, paramsString, boost::is_any_of("&"));
      for (auto it = keyValues.begin(); it != keyValues.end(); ++it) {
	std::string key;
	std::string value;

	idx = it->find("=");
	if (idx == std::string::npos) {
	  urlDecode(*it, key);
	} else {
	  urlDecode(it->substr(0, idx), key);
	  urlDecode(it->substr(idx + 1), value);
	}
	params[key] = value;
      }
    }
    GetFunction func = _getFunctions[funcName];
    std::cout << funcName;
    if (func != NULL)
      try {
	(this->*func)(sender, params);
	std::cout << " => OK" << std::endl;
      } catch (std::exception e) {
	std::cout << " => " << e.what() << std::endl;
	_writeHttpResponse(sender, boost::asio::const_buffer("An error occured", 15), "404 Not Found");
      } catch (...) {
	std::cout << " => An error occured" << std::endl;
	_writeHttpResponse(sender, boost::asio::const_buffer("An error occured", 15), "404 Not Found");
      }
    else {
	std::cout << " => Unknown command" << std::endl;
      _writeHttpResponse(sender, boost::asio::const_buffer("Unknown Command", 15), "404 Not Found");
    }
  }
}

void	RemoteServer::_writeHttpResponse(Network::ATcpSocket* target,
					 boost::asio::const_buffer const& buffer,
					 std::string const& code) {
  std::stringstream size(std::ios_base::in |
			 std::ios_base::out);
  std::stringstream *data = new std::stringstream(std::ios_base::in |
						  std::ios_base::out |
						  std::ios_base::binary);

  size << boost::asio::buffer_size(buffer);
  std::string header;
  header += "HTTP/1.1 " + code + "\r\n";
  header += "Content-Type: text/plain; charset=utf-8\r\n";
  header += "Content-Length: " + std::string(size.str()) + "\r\n";
  (*data) << header << "\r\n";
  data->write(boost::asio::buffer_cast<const char*>(buffer),
  	     boost::asio::buffer_size(buffer));
  _writeData(target, data);
}

void	RemoteServer::_writeData(Network::ATcpSocket* target,
		   std::stringstream *buffer) {
  _toWrite.push_back(std::pair<Network::ATcpSocket*, std::stringstream*>(target, buffer));
  if (_toWrite.size() == 1)
    target->write(buffer->str().c_str(), buffer->str().size());
}

void	RemoteServer::defaultParams(Network::ATcpSocket* sender,
				    std::map<std::string, std::string> & params) {
  for (auto it = params.begin(); it != params.end(); ++it)
    std::cout << "\tkey='" << it->first << "' value='"
	      << it->second << "'" << std::endl;
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));
}

void	RemoteServer::getStreamPort(Network::ATcpSocket* sender,
				    std::map<std::string, std::string>& params) {
  (void)params;
  std::stringstream tmp(std::ios_base::in | std::ios_base::out);
  tmp << "stream-port:";
  tmp << _streamPort;
  _writeHttpResponse(sender, boost::asio::const_buffer(tmp.str().c_str(),
						       tmp.str().size()));
}

void	RemoteServer::begin(Network::ATcpSocket* sender,
			    std::map<std::string,std::string>&) {
  _drive.begin();
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));
}

void	RemoteServer::end(Network::ATcpSocket* sender,
			   std::map<std::string,std::string>&) {
  _drive.end();
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));
}

void	RemoteServer::goFrontwards(Network::ATcpSocket* sender,
			 std::map<std::string,std::string>&) {
  _drive.goFrontwards();
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));
}

void	RemoteServer::goBackwards(Network::ATcpSocket* sender,
			   std::map<std::string,std::string>&) {
  _drive.goBackwards();
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));
}

void	RemoteServer::turnLeft(Network::ATcpSocket* sender,
			   std::map<std::string,std::string>&) {
  _drive.turnLeft();
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));
}

void	RemoteServer::turnRight(Network::ATcpSocket* sender,
			    std::map<std::string,std::string>&) {
  _drive.turnRight();
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));
}

void	RemoteServer::turnFront(Network::ATcpSocket* sender,
			       std::map<std::string,std::string>&) {
  _drive.turnFront();
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));
}

void	RemoteServer::stop(Network::ATcpSocket* sender,
			       std::map<std::string,std::string>&) {
  _drive.stop();
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));
}

void	RemoteServer::steeringWheelAction(Network::ATcpSocket* sender,
					std::map<std::string,std::string>&) {
  _drive.steeringWheelAction();
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));
}

void	RemoteServer::funAction(Network::ATcpSocket* sender,
				  std::map<std::string,
					   std::string>&) {
  _drive.funAction();
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));
}

void	RemoteServer::carambarAction(Network::ATcpSocket* sender,
				   std::map<std::string,
					    std::string>&) {
  _drive.carambarAction();
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));
}

void	RemoteServer::setHead(Network::ATcpSocket* sender,
			      std::map<std::string, std::string> &params) {
  std::cout << "setHead" << std::endl;
  for (auto it = params.begin(); it != params.end(); ++it)
    std::cout << "\tkey='" << it->first << "' value='"
	      << it->second << "'" << std::endl;
  float	yaw = 0, pitch = 0, speed = 1;
  if (params["headYaw"] != "")
    yaw = atof(params["headYaw"].c_str());
  if (params["headPitch"] != "")
    pitch = atof(params["headPitch"].c_str());
  if (params["maxSpeed"] != "")
    speed = atof(params["maxSpeed"].c_str());
  _drive.setHead(yaw, pitch, speed);
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));
}

void	RemoteServer::autoDriving(Network::ATcpSocket* sender,
				  std::map<std::string,
					   std::string>&) {
  if (_autoDriving)
    _autoDriving->start();
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));  
}


void	RemoteServer::stopAutoDriving(Network::ATcpSocket* sender,
				  std::map<std::string,
					   std::string>&) {
  if (_autoDriving)
    _autoDriving->stop();
  _writeHttpResponse(sender, boost::asio::const_buffer("", 0));  
}

//! Naoqi module registration

#ifdef _WIN32
# define ALCALL __declspec(dllexport)
#else
# define ALCALL
#endif

extern "C"
{
  ALCALL int _createModule(boost::shared_ptr<AL::ALBroker> broker)
  {
    AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
    AL::ALBrokerManager::getInstance()->addBroker(broker);
    AL::ALModule::createModule<RemoteServer>(broker, "RemoteServer");
    return 0;
  }

  ALCALL int _closeModule()
  {
    return 0;
  }
}

#ifdef REMOTE_SERVER_IS_REMOTE
int main(int argc, char* argv[])
{
  // pointer to createModule
  TMainType sig;
  sig = &_createModule;
  // call main
  return ALTools::mainFunction("RemoteServerModule", argc, argv, sig);
}
#endif

static bool urlDecode(const std::string& in, std::string& out)
{
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i) {
    if (in[i] == '%') {
      if (i + 3 <= in.size()) {
	int value = 0;
	std::istringstream is(in.substr(i + 1, 2));
	if (is >> std::hex >> value) {
	  out += static_cast<char>(value);
	  i += 2;
	} else {
	  return false;
	}
      } else {
	return false;
      }
    } else if (in[i] == '+') {
      out += ' ';
    } else {
      out += in[i];
    }
  }
  return true;
}
