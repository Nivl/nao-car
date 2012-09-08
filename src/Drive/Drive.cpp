//
// Drive.cpp for  in /home/olivie_a//naoqi-sdk-1.12.5-linux64/examples/drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep  5 23:47:17 2012 samuel olivier
// Last update Sat Sep  8 17:29:01 2012 samuel olivier
//

#include <iostream>
#include <alcommon/albroker.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/altexttospeechproxy.h>

#include "Drive.hh"

std::map<Drive::DrivePose, Pose>	Drive::_poseMap;


Drive::Drive(boost::shared_ptr<AL::ALBroker> broker,
	   const std::string &name) :
  AL::ALModule(broker, name), _poseManager(broker)
{
  if (_poseMap.size() == 0)
    {
      _poseMap[Initial] = Pose::loadFromFile("drive_initial.pose");
      _poseMap[PedalPushed] = Pose::loadFromFile("drive_pedalPushed.pose");
      _poseMap[PedalReleased] = Pose::loadFromFile("drive_pedalReleased.pose");
    }
}

Drive::~Drive()
{
}

void	Drive::init()
{
  std::cout << "INIT" << std::endl; 
}

void	Drive::initDrivePose()
{
  _poseManager.takePose(_poseMap[Initial], 1);
}

void	Drive::pushPedal()
{
  _poseManager.takePose(_poseMap[PedalPushed], 0.5);
}

void	Drive::releasePedal()
{
  _poseManager.takePose(_poseMap[PedalReleased], 0.5);
}
