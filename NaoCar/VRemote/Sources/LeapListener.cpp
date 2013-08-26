//
// LeapListener.cpp for  in /home/olivie_a//rendu/nao/nao-car/NaoCar/VRemote/Sources
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Thu Aug  8 22:00:17 2013 samuel olivier
// Last update Mon Aug 12 22:30:51 2013 samuel olivier
//

#include "LeapListener.hpp"

#include <iostream>
#include <QDebug>

#include "MainWindowDelegate.hpp"

QHash<LeapListener::HandDirection, LeapListener::NaoCallback>       LeapListener::_directionMap;

QHash<LeapListener::HandOrientation, LeapListener::NaoCallback>     LeapListener::_orientationMap;

QList<QPair<PositionConfiguration, LeapListener::HandDirection>>    LeapListener::_directionList;

QList<QPair<PositionConfiguration, LeapListener::HandOrientation>>  LeapListener::_orientationList;


LeapListener::LeapListener(MainWindowDelegate* delegate) :
    _delegate(delegate),
    _currentHandDirection(NoDirection),
    _lastLaunchedDirection(NoDirection),
    _currentHandOrientation(NoOrientation),
    _lastLaunchedOrientation(NoOrientation) {

    if (_directionMap.size() == 0) {
        _directionMap[Frontward] = &MainWindowDelegate::frontward;
        _directionMap[Backward] = &MainWindowDelegate::backward;
        _directionMap[Stopped] = &MainWindowDelegate::stop;
    }
    if (_orientationMap.size() == 0) {
        _orientationMap[Left] = &MainWindowDelegate::left;
        _orientationMap[Right] = &MainWindowDelegate::right;
        _orientationMap[Front] = &MainWindowDelegate::front;
        _orientationMap[NoHand] = &MainWindowDelegate::funAction;
        _orientationMap[Carambar] = &MainWindowDelegate::carambarAction;
    }
    if (_directionList.size() == 0) {
        _directionList << QPair<PositionConfiguration, HandDirection>(PositionConfiguration(":/Frontward.xml", "Frontward"), Frontward)
                << QPair<PositionConfiguration, HandDirection>(PositionConfiguration(":/Backward.xml", "Backward"), Backward)
                << QPair<PositionConfiguration, HandDirection>(PositionConfiguration(":/Stopped.xml", "Stopped"), Stopped);
    }
    if (_orientationList.size() == 0) {
        _orientationList << QPair<PositionConfiguration, HandOrientation>(PositionConfiguration(":/Left.xml", "Left"), Left)
                << QPair<PositionConfiguration, HandOrientation>(PositionConfiguration(":/Right.xml", "Right"), Right)
                << QPair<PositionConfiguration, HandOrientation>(PositionConfiguration(":/Front.xml", "Front"), Front);
    }
}

LeapListener::~LeapListener() {
}

void LeapListener::onInit(const Controller&) {
    std::cout << "Initialized" << std::endl;
}

void LeapListener::onConnect(const Controller&) {
    std::cout << "Connected" << std::endl;
}

void LeapListener::onDisconnect(const Controller&) {
    //Note: not dispatched when running in a debugger.
    std::cout << "Disconnected" << std::endl;
}

void LeapListener::onExit(const Controller&) {
    std::cout << "Exited" << std::endl;
}

void LeapListener::onFrame(const Controller& controller) {
    const Frame frame = controller.frame();
    HandList hands = frame.hands();
    if (!hands.isEmpty()) {
        HandDirection direction = _computeHandDirection(hands);
        HandOrientation orientation = _computeHandOrientation(hands);

        qDebug() << "Direction" << metaObject()->enumerator(metaObject()->indexOfEnumerator("HandDirection")).valueToKey(direction);
        qDebug() << "Orientation" << metaObject()->enumerator(metaObject()->indexOfEnumerator("HandOrientation")).valueToKey(orientation);

        for (int i = 0; i < hands.count(); ++i) {
            Hand hand = hands[i];
            const Vector palmPosition = hand.palmPosition();
            const Vector normal = hand.palmNormal();
            const Vector direction = hand.direction();
            float x = palmPosition.x,
                    y = palmPosition.y,
                    z = palmPosition.z,
                    pitch = direction.pitch() * RAD_TO_DEG,
                    roll = normal.roll() * RAD_TO_DEG,
                    yaw = direction.yaw() * RAD_TO_DEG;
            qDebug() << i << ":" << z;
        }

//        if (direction == _currentHandDirection) {
//            if (_lastLaunchedDirection != _currentHandDirection &&
//                    _lastChangedDirection.elapsed() >= MINIMUM_MILLISECOND_STATE &&
//                    _directionMap.contains(_currentHandDirection)) {
//                (_delegate->*_directionMap[_currentHandDirection])();
//                qDebug() << "Direction" << metaObject()->enumerator(metaObject()->indexOfEnumerator("HandDirection")).valueToKey(direction);
//                _lastLaunchedDirection = _currentHandDirection;
//            }
//        } else {
//            _currentHandDirection = direction;
//            _lastChangedDirection.restart();
//        }

//        if (orientation == _currentHandOrientation) {
//            if (_lastLaunchedOrientation != _currentHandOrientation &&
//                    _lastChangedOrientation.elapsed() >= MINIMUM_MILLISECOND_STATE &&
//                    _orientationMap.contains(_currentHandOrientation)) {
//                (_delegate->*_orientationMap[_currentHandOrientation])();
//                qDebug() << "Orientation" << metaObject()->enumerator(metaObject()->indexOfEnumerator("HandOrientation")).valueToKey(orientation);
//                _lastLaunchedOrientation = _currentHandOrientation;
//            }
//        } else {
//            _currentHandOrientation = orientation;
//            _lastChangedOrientation.restart();
//        }
    }
}

void LeapListener::onFocusGained(const Controller&) {
    std::cout << "Focus Gained" << std::endl;
}

void LeapListener::onFocusLost(const Controller&) {
    std::cout << "Focus Lost" << std::endl;
}

LeapListener::HandDirection
LeapListener::_computeHandDirection(HandList const& hands) const{
    foreach (auto current, _directionList) {
        if (current.first.matchConfiguration(hands))
            return current.second;
    }
    return Stopped;
}

LeapListener::HandOrientation
LeapListener::_computeHandOrientation(HandList const& hands) const {
    foreach (auto current, _orientationList) {
        if (current.first.matchConfiguration(hands))
            return current.second;
    }
    return NoOrientation;
}
