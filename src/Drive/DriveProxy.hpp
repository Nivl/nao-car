//
// DriveProxy.hpp for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
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
# include <Drive.hpp>

class DriveProxy : public AL::ALProxy
{
public:
  DriveProxy(const DriveProxy &rhs);
  DriveProxy(boost::shared_ptr< AL::ALBroker > pBroker, int pProxyMask=0,
	     int timeout=0);
  DriveProxy(const std::string &pIp, int pPort, int pProxyMask=0,
	     int timeout=0);
  DriveProxy(const std::string &pIp, int pPort,
	     boost::shared_ptr<AL::ALBroker> pBroker,
	     int pProxyMask=0, int timeout=0);
  DriveProxy(int pProxyOption, int pTimeout);

  void	start();
  void	stop();

  void	up();
  void	down();
  void	left();
  void	right();
  void	stopPush();
  void	stopTurn();
  void	takeSteeringWheel();
  void	releaseSteeringWheel();
  void	beginNoHand();
  void	endNoHand();
  void	setHead(float headYaw, float headPitch, float maxSpeed);

  bool			steeringWheelIsTaken();
  bool			isGasPedalPushed();
  Drive::SteeringWheel	steeringWheelDirection();
  Drive::Speed		speed();
  bool			isNoHand();
  bool			isAnimating();
};

#endif
