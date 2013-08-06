//
// MainWindow.cpp
// NaoCar Remote
//

#include "MainWindow.hpp"

#include <iostream>
#include <SFML/Window/Joystick.hpp>
#include <QDebug>

#include "ui_MainWindow.h"
#include "MainWindowDelegate.hpp"

MainWindow::MainWindow(MainWindowDelegate* delegate)
  : _delegate(delegate), _window(), _windowUi(),
    _gamepadId(-1), _gamepadTimer(), _gamepadButtonStates(),
    _steeringWheelDirection(Front), _move(Stopped) {
  // Setup ui
  _windowUi.setupUi(&_window);
  connect(_windowUi.actionConnect, SIGNAL(triggered()),
	  this, SLOT(connectAction()));
  connect(_windowUi.viewComboBox, SIGNAL(currentIndexChanged(int)),
	  this, SLOT(viewChanged(int)));
  connect(_windowUi.gamepadID, SIGNAL(valueChanged(int)),
	  this, SLOT(gamepadIDChanged(int)));
  connect(_windowUi.carambarButton, SIGNAL(clicked()),
	  this, SLOT(carambarClicked()));
  connect(_windowUi.talkInput, SIGNAL(returnPressed()),
	  this, SLOT(talkInputEntered()));
  connect(_windowUi.autoDrivingButton, SIGNAL(clicked()),
	  this, SLOT(autoDrivingClicked()));

  // Search for an Xbox gamepad
  chooseGamepad();

  // Setup gamepad timer  
  connect(&_gamepadTimer, SIGNAL(timeout()),
	  this, SLOT(gamepadUpdate()));
  _gamepadTimer.setInterval(1.0 / 10.0);
  _gamepadTimer.start();
  _windowUi.centralwidget->installEventFilter(this);

  _window.show();
}

MainWindow::~MainWindow(void) {
  _gamepadTimer.stop();
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::KeyPress)
    keyPressEvent(static_cast<QKeyEvent*>(event));
  else if (event->type() == QEvent::KeyRelease)
    keyReleaseEvent(static_cast<QKeyEvent*>(event));
  else if (event->type() == QEvent::MouseMove ||
	   event->type() == QEvent::MouseButtonPress ||
	   event->type() == QEvent::MouseButtonRelease)
    _windowUi.centralwidget->setFocus(Qt::MouseFocusReason);
  else
    QObject::eventFilter(obj, event);
  return true;
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
  if (event->isAutoRepeat())
    return ;
  if (event->key() == Qt::Key_Space)
    _delegate->steeringWheelAction();
  else if (event->key() == Qt::Key_C)
    _delegate->carambarAction();
  else if (event->key() == Qt::Key_F)
    _delegate->funAction();
  else if (event->key() == Qt::Key_Up)
    _delegate->moveChanged(Frontwards);
  else if (event->key() == Qt::Key_Down)
    _delegate->moveChanged(Backwards);
  else if (event->key() == Qt::Key_Left)
    _delegate->steeringWheelDirectionChanged(Left);
  else if (event->key() == Qt::Key_Right)
    _delegate->steeringWheelDirectionChanged(Right);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
  if (event->isAutoRepeat())
    return ;
  if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
    _delegate->moveChanged(Stopped);
  else if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)
    _delegate->steeringWheelDirectionChanged(Front);
}

void MainWindow::chooseGamepad(void) {
  sf::Joystick::update();
  for (int i = 0; i < sf::Joystick::Count; ++i) {
    if (sf::Joystick::isConnected(i)) {
      // The Xbox gamepad has 11 buttons
      if (sf::Joystick::getButtonCount(i) == 11) {
	std::cout << "Found gamepad, id: " << i << std::endl;
	setGamepadId(i);
	return ;
      }
    }
  }
}

void MainWindow::setGamepadId(int id) {
  _gamepadId = id;
  _windowUi.gamepadID->setValue(id);
}

QMainWindow* MainWindow::getWindow(void) {
  return &_window;
}

void MainWindow::connectAction(void) {
  if (_delegate)
    _delegate->connect();
}

void MainWindow::viewChanged(int index) {
  if (_delegate)
    _delegate->viewChanged(index);
}

void MainWindow::gamepadIDChanged(int id) {
  _gamepadId = id;
}

void MainWindow::carambarClicked(void) {
  if (_delegate)
    _delegate->carambarAction();
}

void MainWindow::talkInputEntered(void) {
  std::string message = _windowUi.talkInput->text().toStdString();
  if (message.empty())
    return ;
  if (_delegate)
    _delegate->talk(message);
  _windowUi.talkInput->setText("");
}

void MainWindow::autoDrivingClicked(void) {
  if (_delegate)
    _delegate->autoDriving();
}

void MainWindow::gamepadUpdate(void) {
  if (_gamepadId < 0)
    return ;
  sf::Joystick::update();
  // Check buttons and generate events
  if (sf::Joystick::getButtonCount(_gamepadId) != _gamepadButtonStates.size())
    _gamepadButtonStates.resize(sf::Joystick::getButtonCount(_gamepadId));  
  for (unsigned int i = 0; i <= _gamepadButtonStates.size(); ++i) {
    bool newState = sf::Joystick::isButtonPressed(_gamepadId, i);
    if (newState != _gamepadButtonStates[i]) {
      if (newState)
	gamepadButtonPressed(i);
      else
	gamepadButtonReleased(i);
    }
    _gamepadButtonStates[i] = newState;
  }

  // Check axis
  // Direction
  int directionAxis = sf::Joystick::getAxisPosition(_gamepadId,
						    (sf::Joystick::Axis)AxisDirection);
  if (directionAxis < -DirectionAxisTreshold
      && _steeringWheelDirection != Left) {
    if (_delegate)
      _delegate->steeringWheelDirectionChanged(Left);
    _steeringWheelDirection = Left;
  } else if (directionAxis > DirectionAxisTreshold
      && _steeringWheelDirection != Right) {
    if (_delegate)
      _delegate->steeringWheelDirectionChanged(Right);
    _steeringWheelDirection = Right;
  } else if (std::abs(directionAxis) < DirectionAxisTreshold
	     && _steeringWheelDirection != Front) {
    if (_delegate)
      _delegate->steeringWheelDirectionChanged(Front);
    _steeringWheelDirection = Front;
  }

  // Speed
  int backValue = sf::Joystick::getAxisPosition(_gamepadId,
						(sf::Joystick::Axis)AxisBackwards);
  int frontValue = sf::Joystick::getAxisPosition(_gamepadId,
						 (sf::Joystick::Axis)AxisFrontwards);
  if (backValue > MoveAxisTreshold
      && frontValue < MoveAxisTreshold
      && _move != Backwards) {
    if (_delegate)
      _delegate->moveChanged(Backwards);
    _move = Backwards;
  } else if (frontValue > MoveAxisTreshold
	     && backValue < MoveAxisTreshold
	     && _move != Frontwards) {
    if (_delegate)
      _delegate->moveChanged(Frontwards);
    _move = Frontwards;
  } else if (backValue < MoveAxisTreshold
	     && frontValue < MoveAxisTreshold
	     && _move != Stopped) {
    if (_delegate)
      _delegate->moveChanged(Stopped);
    _move = Stopped;
  }
}

void MainWindow::gamepadButtonPressed(unsigned int button) {
  if (!_delegate)
    return ;
  if (button == ButtonStearingWheel)
    _delegate->steeringWheelAction();
  else if (button == ButtonCarambar)
    _delegate->carambarAction();
  else if (button == ButtonFun)
    _delegate->funAction();
  else if (button == ButtonAutoDriving)
    _delegate->autoDriving();
}

void MainWindow::gamepadButtonReleased(unsigned int button) {
  (void)button;
}

void MainWindow::setStreamImage(QImage* image) {
  _windowUi.streamView->setStreamImage(image);
}
