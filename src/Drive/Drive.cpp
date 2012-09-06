//
// Drive.cpp for  in /home/olivie_a//naoqi-sdk-1.12.5-linux64/examples/drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep  5 23:47:17 2012 samuel olivier
// Last update Thu Sep  6 22:00:54 2012 samuel olivier
//

#include <iostream>
#include <alcommon/albroker.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <qi/log.hpp>

#include "Drive.hh"

Drive::Drive(boost::shared_ptr<AL::ALBroker> broker,
	   const std::string &name) :
  AL::ALModule(broker, name)
{
}

Drive::~Drive()
{
}

void	Drive::init()
{
  std::cout << "INIT" << std::endl; 
}
