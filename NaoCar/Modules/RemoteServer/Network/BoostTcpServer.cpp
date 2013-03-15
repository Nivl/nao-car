//
//  BoostTcpServer.cpp
//  Babel Server
//
//  Created by Aurao on 03/12/12.
//
//

#include "BoostTcpServer.h"
#include "ITcpServerDelegate.h"

#include <boost/bind.hpp>

Network::BoostTcpServer::BoostTcpServer(boost::asio::io_service* service) :
  _acceptor(NULL), _ioService(service)
{
    _acceptor = new boost::asio::ip::tcp::acceptor(*service);
}

Network::BoostTcpServer::~BoostTcpServer()
{
    delete _acceptor;
}

bool Network::BoostTcpServer::listen(uint16_t port, std::string address)
{
    if (_acceptor->is_open())
        return false;
    try {
        boost::asio::ip::tcp::endpoint endpoint =
        address == ""
        ? boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)
        : boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(address), port);
        
        _acceptor->open(endpoint.protocol());
        _acceptor->bind(endpoint);
        _acceptor->listen(getMaxPendingConnections());
        _startAccept();
    }
    catch (boost::system::system_error error) {
        return false;
    }
    return true;
}

void Network::BoostTcpServer::_startAccept()
{
    BoostTcpSocket* socket = new BoostTcpSocket(_ioService);
    _acceptor->async_accept(*socket->getBoostSocket(),
                            boost::bind(&Network::BoostTcpServer::_acceptHandler,
                                        this,
                                        boost::asio::placeholders::error,
                                        socket));
}

void Network::BoostTcpServer::_acceptHandler(const boost::system::error_code& error,
                                             BoostTcpSocket* socket)
{
    if (!error)
        _newConnection(socket);
    else
        delete socket;
    _startAccept();
}

std::string Network::BoostTcpServer::getAddress() const
{
    try {
        return _acceptor->local_endpoint().address().to_string();
    } catch (boost::system::system_error error) {
        throw new std::runtime_error("Invalid server endpoint");
    }
}

uint16_t Network::BoostTcpServer::getPort() const
{
    try {
        return _acceptor->local_endpoint().port();
    } catch (boost::system::system_error error) {
        throw new std::runtime_error("Invalid server endpoint");
    }
}
