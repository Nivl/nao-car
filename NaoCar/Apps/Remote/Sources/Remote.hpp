//
// Remote.hpp
// NaoCar Remote
//

#ifndef _REMOTE_HPP_
# define _REMOTE_HPP_

# include <QApplication>
# include <QtNetwork/QHostInfo>
# include <QObject>

# include "MainWindow.hpp"
# include "MainWindowDelegate.hpp"

class Remote : public QObject, public MainWindowDelegate {
Q_OBJECT
public:
  Remote(int argc, char** argv);
  ~Remote(void);

  int exec(void);

  //! Automatically search for a xbox gamepad
  void chooseGamepad(void);

  void connect(void);
  void disconnect(void);
  void viewChanged(int index);
  void gamepadIDChanged(int id);
  void takeCarambar(void);
  void giveCarambar(void);
  void talk(std::string message);
  void autoDriving(void);

public slots:
  void bonjourSocketReadyRead(void);
  void bonjourSocketReadyRead2(void);
  void finishConnect(const QHostInfo &hostInfo);

private:
  QApplication	_app;
  MainWindow	_mainWindow;
  int		_gamepadId;
};

#endif
