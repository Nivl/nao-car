//
//  ASocket.cpp
//  Babel Server
//
//  Created by Aurao on 26/11/12.
//
//

#include "ASocket.h"

Network::ASocket::ASocket(Type type) :
    _type(type)
{

}

Network::ASocket::~ASocket()
{

}

Network::ASocket::Type    Network::ASocket::getType() const
{
    return _type;
}

void    Network::ASocket::setType(ASocket::Type type)
{
    _type = type;
}


void Network::ASocket::write(std::string string)
{
    write(string.data(), string.size());
}