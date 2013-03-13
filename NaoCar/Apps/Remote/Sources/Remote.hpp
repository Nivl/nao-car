//
// Remote.hpp
// NaoCar Remote
//

#ifndef _REMOTE_HPP_
# define _REMOTE_HPP_

# include <QApplication>
# include <QtNetwork/QHostInfo>
# include <QObject>
# include <map>

# include "MainWindow.hpp"
# include "MainWindowDelegate.hpp"
# include "Bonjour.hpp"
# include "BonjourDelegate.hpp"

# define NAOCAR_BONJOUR_SERVICE_NAME "nao-car"

class Remote : public QObject, public MainWindowDelegate, public BonjourDelegate  {
Q_OBJECT
public:

  Remote(int argc, char** argv);
  virtual ~Remote(void);

  int exec(void);

  //! Automatically search for a xbox gamepad
  void chooseGamepad(void);

  //! Called when a new Bonjour service is detected
  virtual void serviceBrowsed(bool error,
			      Bonjour::BrowsingType browsingType=Bonjour::BrowsingAdd,
			      std::string const& name="",
			      std::string const& type="",
			      std::string const& domain="");

  //! Called when a Bonjour service has been resolved
  virtual void serviceResolved(bool error,
			       std::string const& hostname="",
			       std::string const& ip="",
			       unsigned short port=0);
  void connect(void);
  void disconnect(void);
  void viewChanged(int index);
  void gamepadIDChanged(int id);
  void takeCarambar(void);
  void giveCarambar(void);
  void talk(std::string message);
  void autoDriving(void);

private:
  
  struct BonjourService {
    bool		available;
    std::string		name;
    std::string		type;
    std::string		domain;
    std::string		hostname;
    std::string		ip;
    unsigned short	port;
  };

  QApplication		_app;
  MainWindow		_mainWindow;
  int			_gamepadId;
  Bonjour		_bonjour;
  BonjourService	_serverbonjourService;
};

#endif
