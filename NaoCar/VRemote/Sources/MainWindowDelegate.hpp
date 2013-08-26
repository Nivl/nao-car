//
// MainWindowDelegate.hpp
// NaoCar Remote
//

#ifndef _MAIN_WINDOW_DELEGATE_HPP_
# define _MAIN_WINDOW_DELEGATE_HPP_

# include "MainWindow.hpp"

class MainWindowDelegate {
public:
    virtual ~MainWindowDelegate(void) {}
    
    virtual void connect(void) = 0;
    virtual void hostEntered(std::string host) = 0;
    virtual void disconnect(void) = 0;
    virtual void viewChanged(int index) = 0;
    virtual void carambarAction(void) = 0;
    virtual void talk(std::string message) = 0;
    virtual void autoDriving(void) = 0;
    virtual void steeringWheelAction(void) = 0;
    virtual void funAction(void) = 0;
    virtual void frontward(void) = 0;
    virtual void backward(void) = 0;
    virtual void stop(void) = 0;
    virtual void left(void) = 0;
    virtual void right(void) = 0;
    virtual void front(void) = 0;
    virtual void rift(void) = 0;
};

#endif
