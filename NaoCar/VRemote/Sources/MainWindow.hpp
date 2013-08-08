//
// MainWindow.hpp
// NaoCar Remote
//

#ifndef _MAIN_WINDOW_HPP_
# define _MAIN_WINDOW_HPP_

# include <QObject>
# include <QMainWindow>
# include <QTimer>
# include <QPixmap>
# include <QKeyEvent>

# include <vector>

# include "ui_MainWindow.h"

class MainWindowDelegate;

class MainWindow : public QObject {
Q_OBJECT
public:

  /*!
    Xbox button indexes:
    - A: 0
    - B: 1
    - X: 2
    - Y: 3
    
    - LB: 4
    - RB: 5
    
    - Back : 6
    - Start: 7
    - Xbox:  8

    - Joystick Left : 9
    - Joystick Right: 10

    Axis:
    - Left stick X: 0
    - Left stick Y: 1

    - Left Trigger : 2
    - Right Trigger: 3

    - Right stick X: 4
    - Right stick Y: 5

    - Arrows X: 6
    - Arrows Y: 7
   */

  enum GamepadButtons {
    ButtonStearingWheel = 1,
    ButtonCarambar	= 2,
    ButtonFun		= 0,
    ButtonAutoDriving	= 8
  };

  enum GamepadAxis {
    AxisDirection	= 0,
    AxisHeadX		= 4,
    AxisHeadY		= 5,
    AxisFrontwards	= 3,
    AxisBackwards	= 2
  };

  enum Direction {
    Left,
    Right,
    Front
  };

  enum Move {
    Frontwards,
    Backwards,
    Stopped
  };
  
  static const int DirectionAxisTreshold = 50;
  static const int MoveAxisTreshold	 = 50;

  MainWindow(MainWindowDelegate* delegate);
  ~MainWindow(void);

  //! Automatically search for a xbox gamepad
  void chooseGamepad(void);
  void setGamepadId(int id);
  void gamepadButtonPressed(unsigned int button);
  void gamepadButtonReleased(unsigned int button);
  void setStreamImage(QImage* image);

  QMainWindow* getWindow(void);

public slots:
  void connectAction(void);
  void riftAction(void);
  void viewChanged(int index);
  void gamepadIDChanged(int id);
  void carambarClicked(void);
  void talkInputEntered(void);
  void autoDrivingClicked(void);

  void gamepadUpdate(void);

protected:
  bool eventFilter(QObject* obj, QEvent* event);
  void keyPressEvent(QKeyEvent* event);
  void keyReleaseEvent(QKeyEvent* event);

private:
  MainWindowDelegate*	_delegate;
  QMainWindow		_window;
  Ui_MainWindow		_windowUi;
  int			_gamepadId;
  QTimer		_gamepadTimer;
  std::vector<bool>	_gamepadButtonStates;
  Direction		_steeringWheelDirection;
  Move			_move;
};

#endif
