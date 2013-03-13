//
// MainWindow.cpp
// NaoCar Remote
//

#include "MainWindow.hpp"

#include <iostream>

#include "ui_MainWindow.h"

MainWindow::MainWindow(MainWindowDelegate* delegate)
  : _delegate(delegate), _window(), _windowUi() {
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

  _window.show();
}

MainWindow::~MainWindow(void) {

}

void MainWindow::setGamepadId(int id) {
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
  if (_delegate)
    _delegate->gamepadIDChanged(id);
}

void MainWindow::carambarClicked(void) {
  if (_delegate)
    _delegate->takeCarambar();
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
