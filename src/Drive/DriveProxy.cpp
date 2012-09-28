//
// DriveProxy.cpp for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Sep 28 16:49:09 2012 samuel olivier
// Last update Fri Sep 28 19:08:37 2012 samuel olivier
//

#include "DriveProxy.hpp"

DriveProxy::DriveProxy(const DriveProxy &rhs) :
  AL::ALProxy(rhs)
{

}

DriveProxy::DriveProxy(boost::shared_ptr< AL::ALBroker > pBroker,
		       int pProxyMask, int timeout) :
  AL::ALProxy(pBroker, "Drive", pProxyMask, timeout)
{
}

DriveProxy::DriveProxy(const std::string &pIp, int pPort,
		       int pProxyMask, int timeout) :
  AL::ALProxy("Drive", pIp, pPort, pProxyMask, timeout)
{

}

DriveProxy::DriveProxy(const std::string &pIp, int pPort,
		       boost::shared_ptr<AL::ALBroker> pBroker,
		       int pProxyMask, int timeout) :
  AL::ALProxy("Drive", pIp, pPort, pBroker, pProxyMask, timeout)
{

}

DriveProxy::DriveProxy(int pProxyOption, int pTimeout) :
  AL::ALProxy("Drive", pProxyOption, pTimeout)
{

}

void	DriveProxy::start()
{
  callVoid("start");
}

void	DriveProxy::stop()
{
  callVoid("stop");
}

void	DriveProxy::up()
{
  callVoid("up");
}

void	DriveProxy::down()
{
  callVoid("down");
}

void	DriveProxy::left()
{
  callVoid("left");
}

void	DriveProxy::right()
{
  callVoid("right");
}

void	DriveProxy::stopPush()
{
  callVoid("stopPush");
}

void	DriveProxy::stopTurn()
{
  callVoid("stopTurn");
}

void	DriveProxy::takeSteeringWheel()
{
  callVoid("takeSteeringWheel");
}

void	DriveProxy::releaseSteeringWheel()
{
  callVoid("releaseSteeringWheel");
}

void	DriveProxy::beginNoHand()
{
  callVoid("beginNoHand");
}

void	DriveProxy::endNoHand()
{
  callVoid("endNoHand");
}

void	DriveProxy::setHead(float headYaw, float headPitch, float maxSpeed)
{
  callVoid("setHead", headYaw, headPitch, maxSpeed);
}

bool			DriveProxy::steeringWheelIsTaken()
{
  return (call<bool>("steeringWheelIsTaken"));
}

bool			DriveProxy::isGasPedalPushed()
{
  return (call<bool>("isGasPedalPushed"));
}

Drive::SteeringWheel	DriveProxy::steeringWheelDirection()
{
  return ((Drive::SteeringWheel)call<int>("steeringWheelDirection"));
}

Drive::Speed		DriveProxy::speed()
{
  return ((Drive::Speed)call<int>("speed"));
}

bool			DriveProxy::isNoHand()
{
  return (call<bool>("isNoHand"));
}

bool			DriveProxy::isAnimating()
{
  return (call<bool>("isAnimating"));
}
