//
// Remote.cpp
// NaoCar Remote
//

#include "Remote.hpp"

#include <iostream>
#include <SFML/Window/Joystick.hpp>
#include <QSocketNotifier>
#include <QDebug>
#include <QMessageBox>

Remote::Remote(int argc, char** argv)
  : _app(argc, argv), _mainWindow(this), _gamepadId(0),
    _bonjour(this), _serverbonjourService() {
  // Search for an Xbox gamepad
  chooseGamepad();
  // Launch Bonjour to automatically detect Nao on a local network
  if (!_bonjour.browseServices("_http._tcp")) {
    std::cerr << "Cannot browse Bonjour services" << std::endl;
  }
}

Remote::~Remote(void) {
  
}


int Remote::exec(void) {
  return (_app.exec());
}

void Remote::chooseGamepad(void) {
  sf::Joystick::update();
  for (int i = 0; i < sf::Joystick::Count; ++i) {
    if (sf::Joystick::isConnected(i)) {
      // The Xbox gamepad has 11 buttons
      if (sf::Joystick::getButtonCount(i) == 11) {
	_gamepadId = i;
	_mainWindow.setGamepadId(_gamepadId);
	return ;
      }
    }
  }
}

void Remote::serviceBrowsed(bool error,
			    Bonjour::BrowsingType browsingType,
			    std::string const& name,
			    std::string const& type,
			    std::string const& domain) {
  if (error) {
    std::cerr << "Error browsing Bonjour services" << std::endl;
  }
  else if (name == NAOCAR_BONJOUR_SERVICE_NAME || 1) {
    if (browsingType == Bonjour::BrowsingAdd) {
      _serverbonjourService.available = true;
      _serverbonjourService.name = name;
      _serverbonjourService.type = type;
      _serverbonjourService.domain = domain;
      _bonjour.resolveService(name, type, domain);
    } else if (browsingType == Bonjour::BrowsingRemove) {
      _serverbonjourService.available = false;
    }
  }
}

void Remote::serviceResolved(bool error, std::string const& hostname,
			       std::string const& ip,
			       unsigned short port) {
  if (error) {
    std::cerr << "Error resolving Bonjour service address" << std::endl;
  } else if (_serverbonjourService.available) {
    _serverbonjourService.hostname = hostname;
    _serverbonjourService.ip = ip;
    _serverbonjourService.port = port;    
  }
}

void Remote::connect(void) {
  if (_serverbonjourService.available) {

  } else {
    QMessageBox::critical(_mainWindow.getWindow(), "Connect error",
			  "No available NaoCar server found");
  }
}

void Remote::disconnect(void) {
  
}

void Remote::viewChanged(int index) {
  
}

void Remote::gamepadIDChanged(int id) {
  _gamepadId = id;
}

void Remote::takeCarambar(void) {
  
}

void Remote::giveCarambar(void) {
  
}

void Remote::talk(std::string message) {
  
}

void Remote::autoDriving(void) {
  
}
