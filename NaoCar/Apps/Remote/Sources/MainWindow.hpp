//
// MainWindow.hpp
// NaoCar Remote
//

#ifndef _MAIN_WINDOW_HPP_
# define _MAIN_WINDOW_HPP_

# include <QObject>
# include <QMainWindow>

# include "ui_MainWindow.h"
# include "MainWindowDelegate.hpp"

class MainWindow : public QObject {
Q_OBJECT
public:
  MainWindow(MainWindowDelegate* delegate);
  ~MainWindow(void);

  void setGamepadId(int id);

  QMainWindow* getWindow(void);

public slots:
  void connectAction(void);
  void viewChanged(int index);
  void gamepadIDChanged(int id);
  void carambarClicked(void);
  void talkInputEntered(void);
  void autoDrivingClicked(void);

private:
  MainWindowDelegate*	_delegate;
  QMainWindow		_window;
  Ui_MainWindow		_windowUi;
};

#endif
