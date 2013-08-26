//
// LeapListener.hh for  in /home/olivie_a//rendu/nao/nao-car/NaoCar/VRemote/Sources
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Thu Aug  8 21:55:31 2013 samuel olivier
// Last update Sun Aug 11 17:38:54 2013 samuel olivier
//

#ifndef _LEAP_LISTENER_HPP_
# define _LEAP_LISTENER_HPP_

# include <QObject>
# include <QTime>
# include <QHash>
# include <QList>
# include <Leap/Leap.h>
# include <QMetaEnum>

# include "PositionConfiguration.hpp"

# define MINIMUM_MILLISECOND_STATE 25

using namespace Leap;

class MainWindowDelegate;

class LeapListener : public QObject, public Listener {
    Q_OBJECT
    Q_ENUMS(HandDirection)
    Q_ENUMS(HandOrientation)

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

    enum HandDirection {
        Frontward,
        Backward,
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


private:
    typedef void  (MainWindowDelegate::*NaoCallback)(void);


    HandDirection   _computeHandDirection(HandList const& hands) const;
    HandOrientation	_computeHandOrientation(HandList const& hands) const;

    MainWindowDelegate*	_delegate;
    HandDirection		_currentHandDirection;
    HandDirection		_lastLaunchedDirection;
    QTime               _lastChangedDirection;
    HandOrientation     _currentHandOrientation;
    HandOrientation     _lastLaunchedOrientation;
    QTime               _lastChangedOrientation;

    static QHash<HandDirection, NaoCallback>      _directionMap;
    static QHash<HandOrientation, NaoCallback>    _orientationMap;
    static QList<QPair<PositionConfiguration, HandDirection>>   _directionList;
    static QList<QPair<PositionConfiguration, HandOrientation>> _orientationList;
};

#endif
