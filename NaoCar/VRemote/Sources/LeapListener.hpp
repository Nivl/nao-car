//
// LeapListener.hh for  in /home/olivie_a//rendu/nao/nao-car/NaoCar/VRemote/Sources
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Thu Aug  8 21:55:31 2013 samuel olivier
// Last update Fri Aug  9 01:10:25 2013 samuel olivier
//

#ifndef _LEAP_LISTENER_HPP_
# define _LEAP_LISTENER_HPP_

# include <QTime>
# include <Leap.h>

# define MINIMUM_MILLISECOND_STATE 1000

using namespace Leap;

class MainWindowDelegate;

class LeapListener : public Listener {
public:

  LeapListener(MainWindowDelegate* delegate);
  virtual ~LeapListener(void);

public: // Listener
  virtual void onInit(const Controller&);
  virtual void onConnect(const Controller&);
  virtual void onDisconnect(const Controller&);
  virtual void onExit(const Controller&);
  virtual void onFrame(const Controller&);
  virtual void onFocusGained(const Controller&);
  virtual void onFocusLost(const Controller&);

private:

  enum HandDirection {
    FrontWard,
    BackWard,
    Stopped,
    NoDirection
  };

  enum HandOrientation {
    Left,
    Right,
    Front,
    NoHand,
    Carambar,
    NoOrientation
  };

  HandDirection		_computeHandDirection(HandList const& hands) const;
  HandOrientation	_computeHandOrientation(HandList const& hands) const;

  MainWindowDelegate*	_delegate;
  HandDirection		_currentHandDirection;
  QTime			_lastChangedDirection;
  HandOrientation	_currentHandOrientation;  
  QTime			_lastChangedOrientation;
};

#endif
