//
// DriveProxy.cpp for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Sep 28 16:49:09 2012 samuel olivier
// Last update Fri Mar 15 14:33:41 2013 samuel olivier
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

void	DriveProxy::begin()
{
  callVoid("begin");
}

void	DriveProxy::end()
{
  callVoid("end");
}

void	DriveProxy::goFrontwards()
{
  callVoid("goFrontwards");
}

void	DriveProxy::goBackwards()
{
  callVoid("goBackwards");
}

void	DriveProxy::turnLeft()
{
  callVoid("turnLeft");
}

void	DriveProxy::turnRight()
{
  callVoid("turnRight");
}

void	DriveProxy::stop()
{
  callVoid("stop");
}

void	DriveProxy::turnFront()
{
  callVoid("turnFront");
}

void	DriveProxy::steeringWheelAction()
{
  callVoid("steeringWheelAction");
}

void	DriveProxy::funAction()
{
  callVoid("funAction");
}

void	DriveProxy::carambarAction()
{
  callVoid("carambarAction");
}

void	DriveProxy::setHead(float const& headYaw, float const& headPitch, float const& maxSpeed)
{
  callVoid("setHead", headYaw, headPitch, maxSpeed);
}

bool			DriveProxy::isSteeringWheelTaken()
{
  return (call<bool>("isSteeringWheelITaken"));
}

bool			DriveProxy::isGasPedalPushed()
{
  return (call<bool>("isGasPedalPushed"));
}

DriveProxy::SteeringWheel	DriveProxy::steeringWheelDirection()
{
  return ((DriveProxy::SteeringWheel)call<int>("steeringWheelDirection"));
}

DriveProxy::Speed		DriveProxy::speed()
{
  return ((DriveProxy::Speed)call<int>("speed"));
}

bool			DriveProxy::isNoHand()
{
  return (call<bool>("isNoHand"));
}

bool			DriveProxy::isAnimating()
{
  return (call<bool>("isAnimating"));
}

bool			DriveProxy::isCarembar()
{
  return (call<bool>("isCarembar"));
}
