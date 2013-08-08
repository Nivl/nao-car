//
// LeapListener.cpp for  in /home/olivie_a//rendu/nao/nao-car/NaoCar/VRemote/Sources
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Thu Aug  8 22:00:17 2013 samuel olivier
// Last update Fri Aug  9 01:51:17 2013 samuel olivier
//

#include "LeapListener.hpp"

#include <iostream>

LeapListener::LeapListener(MainWindowDelegate* delegate) :
  _delegate(delegate), _currentHandDirection(NoDirection),
  _currentHandOrientation(NoOrientation) {
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
  if (!hands.empty()) {
    HandDirection direction = _computeHandDirection(hands);
    HandOrientation orientation = _computeHandOrientation(hands);
    std::cout << hands.count() << std::endl;
    if (direction == _currentHandDirection) {
      if (_lastChangedDirection.elapsed() >= MINIMUM_MILLISECOND_STATE) {

      }
    } else {
      _currentHandDirection = direction;
      _lastChangedDirection.restart();
    }
    if (orientation == _currentHandOrientation) {
      if (_lastChangedOrientation.elapsed() >= MINIMUM_MILLISECOND_STATE) {
	
      }
    } else {
      _currentHandOrientation = orientation;
      _lastChangedOrientation.restart();
    }
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
  return FrontWard;
}

LeapListener::HandOrientation
LeapListener::_computeHandOrientation(HandList const& hands) const {
  return Left;
}
