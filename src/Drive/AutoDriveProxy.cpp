//
// DriveProxy.cpp for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Sep 28 16:49:09 2012 samuel olivier
// Last update Tue Nov 13 22:56:15 2012 samuel olivier
//

#include "AutoDriveProxy.hpp"

AutoDriveProxy::AutoDriveProxy(const AutoDriveProxy &rhs) :
  AL::ALProxy(rhs)
{

}

AutoDriveProxy::AutoDriveProxy(boost::shared_ptr< AL::ALBroker > pBroker,
		       int pProxyMask, int timeout) :
  AL::ALProxy(pBroker, "AutoDrive", pProxyMask, timeout)
{
}

AutoDriveProxy::AutoDriveProxy(const std::string &pIp, int pPort,
		       int pProxyMask, int timeout) :
  AL::ALProxy("AutoDrive", pIp, pPort, pProxyMask, timeout)
{
}

AutoDriveProxy::AutoDriveProxy(const std::string &pIp, int pPort,
		       boost::shared_ptr<AL::ALBroker> pBroker,
		       int pProxyMask, int timeout) :
  AL::ALProxy("AutoDrive", pIp, pPort, pBroker, pProxyMask, timeout)
{
}

AutoDriveProxy::AutoDriveProxy(int pProxyOption, int pTimeout) :
  AL::ALProxy("AutoDrive", pProxyOption, pTimeout)
{
}

void	AutoDriveProxy::start()
{
  callVoid("start");
}

void	AutoDriveProxy::stop()
{
  callVoid("stop");
}
