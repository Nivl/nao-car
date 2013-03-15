//
//  ATcpServer.cpp
//  Babel Server
//
//  Created by Aurao on 30/11/12.
//
//

#include "ATcpServer.h"

#include "ITcpServerDelegate.h"

Network::ATcpServer::ATcpServer() :
    _maxPendingConnections(maxPendingConnectionsDefault)
{
    
}

Network::ATcpServer::~ATcpServer()
{
    
}

int Network::ATcpServer::getMaxPendingConnections() const
{
    return _maxPendingConnections;
}

void Network::ATcpServer::setMaxPendingConnections(int maxPendingConnections)
{
    _maxPendingConnections = maxPendingConnections;
}

Network::ITcpServerDelegate* Network::ATcpServer::getDelegate() const
{
    return _delegate;
}

void Network::ATcpServer::setDelegate(ITcpServerDelegate* delegate)
{
    _delegate = delegate;
}

void Network::ATcpServer::_newConnection(ATcpSocket* socket)
{
    if (_delegate)
    {
        _delegate->newConnection(this, socket);
    }
}