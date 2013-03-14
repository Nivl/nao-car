//
// Remote.cpp
// NaoCar Remote
//

#include "Remote.hpp"

#include <iostream>
#include <sstream>

#include <QDebug>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QList>

Remote::Remote(int argc, char** argv)
  : _app(argc, argv), _mainWindow(this),
    _bonjour(this), _naoAvailable(false), _naoUrl(), _networkManager(),
    _connected(false) {
  // Launch Bonjour to automatically detect Nao on a local network
  if (!_bonjour.browseServices("_http._tcp")) {
    std::cerr << "Cannot browse Bonjour services" << std::endl;
  }
  // Configure network
  QObject::connect(&_networkManager, SIGNAL(finished(QNetworkReply*)),
		   this, SLOT(networkRequestFinished(QNetworkReply*)));
  _naoUrl.setScheme("http");

  _naoAvailable = true;
}

Remote::~Remote(void) {
  
}

int Remote::exec(void) {
  return (_app.exec());
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
      _naoAvailable = true;
      _bonjour.resolveService(name, type, domain);
    } else if (browsingType == Bonjour::BrowsingRemove) {
      _naoAvailable = false;
    }
  }
}

void Remote::serviceResolved(bool error, std::string const& hostname,
			     std::string const& ip,
			     unsigned short port) {
  (void)hostname;
  if (error) {
    std::cerr << "Error resolving Bonjour service address" << std::endl;
  } else if (_naoAvailable) {
    _naoUrl.setHost(QString(ip.c_str()));
    _naoUrl.setPort(port);
  }
}

void Remote::connect(void) {
  if (_naoAvailable) {
    sendRequest("/start");
    sendRequest("/stream-socket");
  } else {
    QMessageBox::critical(_mainWindow.getWindow(), "Connect error",
			  "No available NaoCar server found");
  }
}

void Remote::disconnect(void) {
  if (_naoAvailable)
    sendRequest("/stop");
}

void Remote::viewChanged(int index) {
  if (_naoAvailable) {
    std::stringstream str;
    str << index;
    sendRequest("/change-view", "view", str.str());
  }
}

void Remote::carambarAction(void) {
  if (_naoAvailable)
    sendRequest("/carambar-action");
}

void Remote::talk(std::string message) {
  if (_naoAvailable)
    sendRequest("/talk", "message", message);
}

void Remote::autoDriving(void) {
  if (_naoAvailable)
    sendRequest("/auto-driving");
}

void Remote::steeringWheelAction(void) {
  if (_naoAvailable)
    sendRequest("/steeringwheel-action");
}

void Remote::funAction(void) {
  if (_naoAvailable)
    sendRequest("/fun-action");
}

void Remote::steeringWheelDirectionChanged(MainWindow::Direction direction) {
  if (!_naoAvailable)
    return ;
  if (direction == MainWindow::Left) {
    sendRequest("/turn-left");
  } else if (direction == MainWindow::Right) {
    sendRequest("/turn-right");
  } else if (direction == MainWindow::Front) {
    sendRequest("/turn-front");
  }
}

void Remote::moveChanged(MainWindow::Move move) {
  if (!_naoAvailable)
    return ;
  if (move == MainWindow::Frontwards)
    sendRequest("/go-frontwards");
  else if (move == MainWindow::Backwards)
    sendRequest("/go-backwards");
  else if (move == MainWindow::Stopped)
    sendRequest("/stop");
}

void Remote::sendRequest(std::string requestStr,
			 std::string paramName,
			 std::string paramValue) {
    QNetworkRequest request;
    QUrl newUrl = _naoUrl;
    newUrl.setPath(requestStr.c_str());
    if (!paramName.empty() && !paramValue.empty()) {
      QList<QPair<QString, QString> >
	params({QPair<QString, QString>(paramName.c_str(), paramValue.c_str())});
      newUrl.setQueryItems(params);
    }
    request.setUrl(newUrl);
    std::cout << "Sending request " << newUrl.toString().toStdString() << std::endl;
    _networkManager.get(request);
}

void Remote::networkRequestFinished(QNetworkReply* reply) {
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << reply->errorString();
  } else {
    reply->readAll();
  }
}
