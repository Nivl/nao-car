//
// AutoDriveProxy.hpp for  in /home/olivie_a//rendu/nao/nao-car/src/AutoDrive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Sep 28 16:49:12 2012 samuel olivier
// Last update Fri Sep 28 18:52:36 2012 samuel olivier
//

#ifndef __DIRVE_PROXY__
# define __DIRVE_PROXY__

# include <string>
# include <alcommon/alproxy.h>
# include <AutoDrive.hpp>

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
