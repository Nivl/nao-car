//
//  ATcpSocket.cpp
//  Babel Server
//
//  Created by Aurao on 26/11/12.
//
//

#include "ATcpSocket.h"

#include "ITcpSocketDelegate.h"

Network::ATcpSocket::ATcpSocket() :
    ASocket(ASocket::TcpSocket), _delegate(NULL)
{    
}

Network::ATcpSocket::~ATcpSocket()
{
}

Network::ITcpSocketDelegate* Network::ATcpSocket::getDelegate() const
{
    return _delegate;
}

void Network::ATcpSocket::setDelegate(ITcpSocketDelegate* delegate)
{
    _delegate = delegate;
}

void Network::ATcpSocket::_connected(ASocket::Error error)
{
    if (_delegate)
        _delegate->connected(this, error);
}

void Network::ATcpSocket::_readFinished(ASocket::Error error, size_t bytesRead)
{
    if (_delegate)
        _delegate->readFinished(this, error, bytesRead);
}

void Network::ATcpSocket::_writeFinished(ASocket::Error error, size_t bytesWritten)
{
    if (_delegate)
        _delegate->writeFinished(this, error, bytesWritten);
}