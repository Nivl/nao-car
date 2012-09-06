//
// Kart.cpp for  in /home/olivie_a//naoqi-sdk-1.12.5-linux64/examples/kart
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep  5 23:47:17 2012 samuel olivier
// Last update Thu Sep  6 00:08:03 2012 samuel olivier
//

#include <alcommon/albroker.h>

#include "Kart.hh"

Kart::Kart(boost::shared_ptr<AL::ALBroker> broker,
	   const std::string &name) :
  AL::ALModule(broker, name)
{
}

Kart::~Kart()
{
}

void	Kart::init()
{
}
