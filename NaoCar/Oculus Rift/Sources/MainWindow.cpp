//
// MainWindow.cpp
// NaoCar Remote
//

#include "MainWindow.hpp"

#include <iostream>
#include <QDebug>

#include "ui_MainWindow.h"
#include "MainWindowDelegate.hpp"

MainWindow::MainWindow(MainWindowDelegate* delegate)
: _delegate(delegate), _window(), _windowUi(),
_steeringWheelDirection(Front), _move(Stopped) {
    // Setup ui
    _windowUi.setupUi(&_window);
    connect(_windowUi.actionConnect, SIGNAL(triggered()),
            this, SLOT(connectAction()));
    connect(_windowUi.actionDisconnect, SIGNAL(triggered()),
            this, SLOT(disconnectAction()));
    connect(_windowUi.hostInput, SIGNAL(returnPressed()),
            this, SLOT(hostInputEntered()));
    connect(_windowUi.actionRift, SIGNAL(triggered()),
            this, SLOT(riftAction()));
    connect(_windowUi.viewComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(viewChanged(int)));
    connect(_windowUi.carambarButton, SIGNAL(clicked()),
            this, SLOT(carambarClicked()));
    connect(_windowUi.talkInput, SIGNAL(returnPressed()),
            this, SLOT(talkInputEntered()));
    connect(_windowUi.autoDrivingButton, SIGNAL(clicked()),
            this, SLOT(autoDrivingClicked()));
    connect(_windowUi.safeModeButton, SIGNAL(clicked()),
            this, SLOT(safeModeClicked()));
    
    _windowUi.centralwidget->installEventFilter(this);
    
    _window.show();
}

MainWindow::~MainWindow(void) {
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
        return QObject::eventFilter(obj, event);
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
        _delegate->frontward();
    else if (event->key() == Qt::Key_Down)
        _delegate->backward();
    else if (event->key() == Qt::Key_Left)
        _delegate->left();
    else if (event->key() == Qt::Key_Right)
        _delegate->right();
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
    if (event->isAutoRepeat())
        return ;
    if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down)
        _delegate->stop();
    else if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)
        _delegate->front();
}

QMainWindow* MainWindow::getWindow(void) {
    return &_window;
}

void MainWindow::connectAction(void) {
    if (_delegate)
        _delegate->connect();
}

void MainWindow::disconnectAction(void) {
    if (_delegate)
        _delegate->disconnect();
}

void MainWindow::hostInputEntered(void) {
    if (_delegate) {
        _delegate->hostEntered(_windowUi.hostInput->text().toStdString());
    }
}
    
void MainWindow::riftAction(void) {
    if (_delegate)
        _delegate->rift();
}

void MainWindow::viewChanged(int index) {
    if (_delegate)
        _delegate->viewChanged(index);
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

void MainWindow::safeModeClicked(void) {
    if (_delegate)
        _delegate->safeMode();
}

void MainWindow::setStreamImage(QImage* image) {
    _windowUi.streamView->setStreamImage(image);
}
