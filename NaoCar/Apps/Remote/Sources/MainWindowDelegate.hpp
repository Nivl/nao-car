//
// MainWindowDelegate.hpp
// NaoCar Remote
//

#ifndef _MAIN_WINDOW_DELEGATE_HPP_
# define _MAIN_WINDOW_DELEGATE_HPP_

class MainWindowDelegate {
public:
  virtual ~MainWindowDelegate(void) {}

  virtual void connect(void) = 0;
  virtual void disconnect(void) = 0;
  virtual void viewChanged(int index) = 0;
  virtual void gamepadIDChanged(int id) = 0;
  virtual void takeCarambar(void) = 0;
  virtual void giveCarambar(void) = 0;
  virtual void talk(std::string message) = 0;
  virtual void autoDriving(void) = 0;
};

#endif
