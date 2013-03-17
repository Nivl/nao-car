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
#include <QImageReader>
#include <QApplication>

Remote::Remote()
  : _mainWindow(this),
    _bonjour(this), _naoAvailable(false), _naoUrl(), _networkManager(),
    _connected(false), _streamSocket(new QTcpSocket(this)),
    _streamImageSize(-1), _streamImage(new QImage()), _streamSizeRead(false) {
  // Launch Bonjour to automatically detect Nao on a local network
  if (!_bonjour.browseServices("_http._tcp")) {
    std::cerr << "Cannot browse Bonjour services" << std::endl;
  }
  // Configure network
  QObject::connect(&_networkManager, SIGNAL(finished(QNetworkReply*)),
		   this, SLOT(networkRequestFinished(QNetworkReply*)));
  _naoUrl.setScheme("http");
  QObject::connect(_streamSocket, SIGNAL(readyRead()),
		   this, SLOT(streamDataAvailable()));
  _streamImage->load(":/waiting-streaming.png");
  _mainWindow.setStreamImage(_streamImage);
  qDebug() << QImageReader::supportedImageFormats();
}

Remote::~Remote(void) {
  delete _streamImage;
}

int Remote::exec(void) {
  return (qApp->exec());
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
    sendRequest("/begin");
    sendRequest("/get-stream-port");
  } else {
    QMessageBox::critical(_mainWindow.getWindow(), "Connect error",
			  "No available NaoCar server found");
  }
}

void Remote::disconnect(void) {
  if (_naoAvailable)
    sendRequest("/end");
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
	params;
      params.append({QPair<QString, QString>(paramName.c_str(),
					     paramValue.c_str())});
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
    QByteArray data = reply->readAll();

    if (data.startsWith("stream-port:")) {
	_streamSocket->connectToHost(_naoUrl.host(), data.mid(12).toInt());
    }
  }
}

void Remote::streamDataAvailable() {
  if (_streamSizeRead == false &&
      (quint64)_streamSocket->bytesAvailable() >= sizeof(_streamImageSize)) {
    _streamSocket->read((char*)&_streamImageSize, sizeof(_streamImageSize));
    _streamSizeRead = true;
  }
  if (_streamSizeRead == true &&
      _streamSocket->bytesAvailable() >= _streamImageSize) {
    QByteArray data = _streamSocket->read(_streamImageSize);
    _streamImage->loadFromData(data);
    _mainWindow.setStreamImage(_streamImage);
    _streamSizeRead = false;
  }
}
