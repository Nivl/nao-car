//
// DriveProxy.hpp for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Sep 28 16:49:12 2012 samuel olivier
// Last update Fri Mar 15 14:32:15 2013 samuel olivier
//

#ifndef __DRIVE_PROXY__
# define __DRIVE_PROXY__

# include <string>
# include <alcommon/alproxy.h>

class DriveProxy : public AL::ALProxy
{
public:
  enum SteeringWheel
    {
      Left,
      Front,
      Right
    };

  enum Speed
    {
      Up,
      Down
    };

  DriveProxy(const DriveProxy &rhs);
  DriveProxy(boost::shared_ptr< AL::ALBroker > pBroker, int pProxyMask=0,
	     int timeout=0);
  DriveProxy(const std::string &pIp, int pPort, int pProxyMask=0,
	     int timeout=0);
  DriveProxy(const std::string &pIp, int pPort,
	     boost::shared_ptr<AL::ALBroker> pBroker,
	     int pProxyMask=0, int timeout=0);
  DriveProxy(int pProxyOption, int pTimeout);

  void  begin();
  void  end();

  void  goFrontwards();
  void  goBackwards();
  void  turnLeft();
  void  turnRight();
  void  turnFront();
  void  stop();
  void  steeringWheelAction();
  void  funAction();
  void  carambarAction();
  void  setHead(float const& headYaw, float const& headPitch,
                float const& maxSpeed);

  bool  isSteeringWheelTaken();
  bool  isGasPedalPushed();
  SteeringWheel	steeringWheelDirection();
  Speed	speed();
  bool  isNoHand();
  bool  isAnimating();
  bool  isCarembar();
};

#endif
