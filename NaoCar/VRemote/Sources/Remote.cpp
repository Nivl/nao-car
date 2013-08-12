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
_streamImageSize(-1), _streamImage(new QImage()), _streamSizeRead(false),
_rift(NULL), _riftTimer() {
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
    
    // Configure Rift
    _riftTimer.setInterval(50);
    QObject::connect(&_riftTimer, SIGNAL(timeout()), this, SLOT(updateRift()));
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
    else if (name == NAOCAR_BONJOUR_SERVICE_NAME) {
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
        qDebug() << QString::fromStdString(ip) << port;
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

void Remote::hostEntered(std::string host) {
    size_t colonPos = host.find(':');
    if (colonPos == std::string::npos) {
        std::cout << "Using host ip: " << host << std::endl;
        _naoUrl.setHost(host.c_str());
    } else {
        std::string ip = host.substr(0, colonPos);
        std::string portStr = host.substr(colonPos + 1);
        uint port = std::atoi(portStr.c_str());
        std::cout << "Using host ip: " << ip << " and port " << port << std::endl;
        _naoUrl.setHost(ip.c_str());
        _naoUrl.setPort(port);
    }
    _naoAvailable = true;
}

void Remote::disconnect(void) {
    if (_naoAvailable)
        sendRequest("/end");
}

void Remote::viewChanged(int index) {
    if (_naoAvailable) {
        sendRequest("/change-view",
                    ParamsList() << QPair<QString, QString>("view", QString::number(index)));
    }
}

void Remote::carambarAction(void) {
    if (_naoAvailable)
        sendRequest("/carambar-action");
}

void Remote::talk(std::string message) {
    if (_naoAvailable)
        sendRequest("/talk", ParamsList() << QPair<QString, QString>("message", message.c_str()));
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
                         ParamsList const & params) {
    QNetworkRequest request;
    QUrl newUrl = _naoUrl;
    newUrl.setPath(requestStr.c_str());
    if (!params.empty()) {
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

void Remote::streamDataAvailable(void) {
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

void Remote::rift(void) {
    if (_rift) {
        _riftTimer.stop();
        delete _rift;
        _rift = NULL;
    } else {
        _rift = new Rift();
        _riftTimer.start();
    }
}

void Remote::updateRift(void) {
    OVR::Vector3f orientation = _rift->getOrientation();
    ParamsList params;
    params << QPair<QString, QString>("headYaw", QString::number(orientation.x))
    << QPair<QString, QString>("headPitch", QString::number(-orientation.y))
    << QPair<QString, QString>("maxSpeed", QString::number(0.5));
    sendRequest("/setHead", params);
}