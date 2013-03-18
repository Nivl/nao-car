//
//  BoostTcpSocket.cpp
//  Babel Server
//
//  Created by Aurao on 26/11/12.
//
//

#include "BoostTcpSocket.h"

#include <sstream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include "ITcpSocketDelegate.h"

Network::BoostTcpSocket::BoostTcpSocket(boost::asio::io_service* service) :
    ATcpSocket(), _socket(NULL), _ioService(service)
{
  _socket = new boost::asio::ip::tcp::socket(*_ioService);
}

Network::BoostTcpSocket::~BoostTcpSocket()
{
    delete _socket;
}

void Network::BoostTcpSocket::connect(std::string host, uint16_t port)
{
    std::stringstream portString;
    portString << port;

    boost::asio::ip::tcp::resolver::query resolverQuery(host, portString.str());
    boost::shared_ptr<boost::asio::ip::tcp::resolver>
        resolver(new boost::asio::ip::tcp::resolver(*_ioService));
    
    resolver->async_resolve(resolverQuery,
                           boost::bind(&Network::BoostTcpSocket::_resolveHandler,
                                       this,
                                        resolver,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::iterator));
}

void Network::BoostTcpSocket::close()
{
    _socket->close();
}

void Network::BoostTcpSocket::_resolveHandler(boost::shared_ptr<boost::asio::ip::tcp::resolver>,
                                              const boost::system::error_code& ec,
                                              boost::asio::ip::tcp::resolver::iterator endpoints)
{
    if (!ec)
    {
      _socket->async_connect((*endpoints).endpoint(),
			     boost::bind(&Network::BoostTcpSocket::_connectHandler,
					 this,
					 boost::asio::placeholders::error));
    }
    else
        _connected(ASocket::HostNotFound);
}

void Network::BoostTcpSocket::_connectHandler(const boost::system::error_code& ec)
{
    if (!ec)
        _connected(ASocket::NoError);
    else
        _connected(ASocket::HostUnreachable);
}

void Network::BoostTcpSocket::read(void* buffer, uint32_t size, bool all)
{
    if (all)
        boost::asio::async_read(*_socket, boost::asio::buffer(buffer, size),
                                boost::bind(&Network::BoostTcpSocket::_readHandler,
                                            this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    else
        _socket->async_read_some(boost::asio::buffer(buffer, size),
                                 boost::bind(&Network::BoostTcpSocket::_readHandler,
                                             this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
}

void Network::BoostTcpSocket::readUntil(std::string const& delim) {
  boost::asio::async_read_until(*_socket, _readUntilBuffer, delim,
				boost::bind(&Network::BoostTcpSocket::_readUntilHandler,
					    this,
					    boost::asio::placeholders::error,
					    boost::asio::placeholders::bytes_transferred));
}


void Network::BoostTcpSocket::_readHandler(const boost::system::error_code& ec,
                                           std::size_t bytesTransfered)
{
    if (!ec)
        _readFinished(ASocket::NoError, bytesTransfered);
    else
        _readFinished(ASocket::ReadError, bytesTransfered);
}

void Network::BoostTcpSocket::_readUntilHandler(const boost::system::error_code& ec,
						std::size_t)
{
  ITcpSocketDelegate*	delegate = getDelegate();

  if (!delegate)
    return ;
  if (!ec) {
    std::istream is(&_readUntilBuffer);
    std::string data;
    int length = _readUntilBuffer.size();
    char * buffer = new char [length];

    is.read(buffer,length);
    data.assign(buffer, length);
    delete buffer;
    delegate->readFinished(this, ASocket::NoError, data);
  } else {
    delegate->readFinished(this, ASocket::ReadError, "");
  }
}

void Network::BoostTcpSocket::write(const void* buffer, uint32_t size)
{
    boost::asio::async_write(*_socket, boost::asio::buffer(buffer, size),
                             boost::bind(&Network::BoostTcpSocket::_writeHandler,
                                         this,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred));
}

void Network::BoostTcpSocket::_writeHandler(const boost::system::error_code& ec,
                                            std::size_t bytesTransfered)
{
  std::cout << ec << std::endl;
    if (!ec)
        _writeFinished(ASocket::NoError, bytesTransfered);
    else
        _writeFinished(ASocket::WriteError, bytesTransfered);
}

std::string Network::BoostTcpSocket::getRemoteIp() const
{
    try {
        boost::asio::ip::tcp::endpoint endpoint = _socket->remote_endpoint();
        return endpoint.address().to_string();
    }
    catch (boost::system::system_error e) {
        return "";
    }
}

uint32_t Network::BoostTcpSocket::getBinaryRemoteIp() const
{
    try {
        boost::asio::ip::tcp::endpoint endpoint = _socket->remote_endpoint();
        return endpoint.address().to_v4().to_ulong();
    }
    catch (boost::system::system_error e) {
        return 0;
    }
}

uint16_t    Network::BoostTcpSocket::getRemotePort() const
{
    try {
        boost::asio::ip::tcp::endpoint endpoint = _socket->remote_endpoint();
        return endpoint.port();
    }
    catch (boost::system::system_error e) {
        return 0;
    }
}

bool        Network::BoostTcpSocket::isConnected() const
{
    return _socket->is_open();
}

boost::asio::ip::tcp::socket*   Network::BoostTcpSocket::getBoostSocket() const
{
    return _socket;
}
