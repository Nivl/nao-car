//
// sfWindow.hh for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep 26 18:34:54 2012 samuel olivier
// Last update Thu Sep 27 12:12:19 2012 samuel olivier
//

#ifndef __WINDOW__
# define __WINDOW__

# include <alcommon/almodule.h>
# include <alproxies/alvideodeviceproxy.h>
# include <alvision/alvisiondefinitions.h>
# include <Animation.hpp>
# include <PoseManager.hpp>
# include <SFML/Window.hpp>

namespace AL
{
  class ALBroker;
}

class Window
{
public:
  Window(boost::shared_ptr<AL::ALBroker> broker, int joystickId);
  ~Window();

  void	exec();

private:
  void	checkEvent(sf::Event *event);
  void	launch(std::string const& name);
  void	keyPressEvent(sf::Event *event);
  void	keyReleaseEvent(sf::Event *event);

  void	up();
  void	down();
  void	left();
  void	right();
  void	stopPush();
  void	stopTurn();

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

  std::map<std::string, Animation>	_animations;
  sf::Window	_window;
  PoseManager	_poseManager;
  bool		_steeringWheelIsTaken;
  bool		_gasPedalIsPushed;
  SteeringWheel	_steeringWheelDirection;
  Speed		_speed;
  sf::Clock	_clock;
  unsigned int	_joystickId;
};

#endif
