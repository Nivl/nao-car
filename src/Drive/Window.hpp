//
// Window.hpp for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Sep  7 19:59:36 2012 samuel olivier
// Last update Sat Sep  8 15:13:20 2012 samuel olivier
//

#ifndef __WINDOW__
# define __WINDOW__

# include <alcommon/almodule.h>
# include <Animation.hpp>
# include <PoseManager.hpp>

# include <QWidget>
# include <QPushButton>
# include <QVBoxLayout>
# include <QKeyEvent>

namespace AL
{
  class ALBroker;
}

class Window : public QWidget
{

Q_OBJECT

public:
  Window(boost::shared_ptr<AL::ALBroker> broker);

protected:
  void	keyPressEvent(QKeyEvent *event);
  void	keyReleaseEvent(QKeyEvent *event);

private:

  void	launch(std::string const& name);

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
  PoseManager	_poseManager;
  bool		_steeringWheelIsTaken;
  bool		_gasPedalIsPushed;
  SteeringWheel	_steeringWheelDirection;
  Speed		_speed;
};

#endif
