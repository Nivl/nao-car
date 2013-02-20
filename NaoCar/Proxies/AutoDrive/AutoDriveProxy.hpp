//
// AutoDriveProxy.hpp for  in /home/olivie_a//rendu/nao/nao-car/src/AutoDrive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Sep 28 16:49:12 2012 samuel olivier
// Last update Wed Feb 20 18:06:21 2013 samuel olivier
//

#ifndef __AUTO_DRIVE_PROXY__
# define __AUTO_DRIVE_PROXY__

# include <string>
# include <alcommon/alproxy.h>

class AutoDriveProxy : public AL::ALProxy
{
public:
  AutoDriveProxy(const AutoDriveProxy &rhs);
  AutoDriveProxy(boost::shared_ptr< AL::ALBroker > pBroker, int pProxyMask=0,
	     int timeout=0);
  AutoDriveProxy(const std::string &pIp, int pPort, int pProxyMask=0,
	     int timeout=0);
  AutoDriveProxy(const std::string &pIp, int pPort,
	     boost::shared_ptr<AL::ALBroker> pBroker,
	     int pProxyMask=0, int timeout=0);
  AutoDriveProxy(int pProxyOption, int pTimeout);

  void	start();
  void	stop();
};

#endif
