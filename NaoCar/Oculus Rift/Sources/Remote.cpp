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
      _naoAvailable(false), _naoUrl(), _networkManager(),
      _connected(false), _streamSocket(new QTcpSocket(this)),
      _streamImageSize(-1), _streamImage(new QImage()), _streamSizeRead(false),
      _rift(NULL) {

    // Configure network
    QObject::connect(&_networkManager, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(networkRequestFinished(QNetworkReply*)));
    _naoUrl.setScheme("http");
    QObject::connect(_streamSocket, SIGNAL(readyRead()),
                     this, SLOT(streamDataAvailable()));
    _streamImage->load(":/waiting-streaming.png");
    _mainWindow.setStreamImage(_streamImage);

    // Timer for multithread request
    _flushRequestTimer.setInterval(5);
    _flushRequestTimer.start();
    QObject::connect(&_flushRequestTimer, SIGNAL(timeout()),
                     this, SLOT(_flushPendingRequest()));
}

Remote::~Remote(void) {
    delete _streamImage;
}

int Remote::exec(void) {
    return (qApp->exec());
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

void Remote::safeMode(void) {
    if (_naoAvailable) {
        sendRequest("/auto-driving", ParamsList() << QPair<QString, QString>("mode", "safe"));
    }
}

void Remote::steeringWheelAction(void) {
    if (_naoAvailable)
        sendRequest("/steeringwheel-action");
}

void Remote::funAction(void) {
    if (_naoAvailable)
        sendRequest("/fun-action");
}

void Remote::frontward(void) {
    if (!_naoAvailable)
        return ;
    sendRequest("/go-frontwards");
}

void Remote::backward(void) {
    if (!_naoAvailable)
        return ;
    sendRequest("/go-backwards");
}

void Remote::stop(void) {
    if (!_naoAvailable)
        return ;
    sendRequest("/stop");
}

void Remote::left(void) {
    if (!_naoAvailable)
        return ;
    sendRequest("/turn-left");
}

void Remote::right(void) {
    if (!_naoAvailable)
        return ;
    sendRequest("/turn-right");
}

void Remote::front(void) {
    if (!_naoAvailable)
        return ;
    sendRequest("/turn-front");
}

void Remote::sendRequest(std::string requestStr,
                         ParamsList const & params) {
    QNetworkRequest request;
    QUrl newUrl = _naoUrl;
    newUrl.setPath(requestStr.c_str());
    if (!params.empty()) {
        newUrl.setQueryItems(params);
    }
    _pendingRequest << newUrl;
}

void Remote::_flushPendingRequest() {
    while (!_pendingRequest.empty()) {
        QUrl url = _pendingRequest.first();
        QNetworkRequest request;
        request.setUrl(url);
        qDebug() << url;
        _networkManager.get(request);
        _pendingRequest.removeFirst();
    }
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
        if (_rift) {
            _rift->setViewImage(*_streamImage);
        }
        _streamSizeRead = false;
    }
}

void Remote::rift(void) {
    if (_rift) {
        delete _rift;
        _rift = NULL;
    } else {
        _rift = new Rift();
        _rift->setDelegate(this);
        _rift->getView()->installEventFilter(&_mainWindow);
    }
}

void Remote::riftOrientationUpdate(OVR::Vector3f orientation) {
    ParamsList params;
    params << QPair<QString, QString>("headYaw", QString::number(orientation.x))
            << QPair<QString, QString>("headPitch", QString::number(-orientation.y))
            << QPair<QString, QString>("maxSpeed", QString::number(0.5));
    sendRequest("/setHead", params);
}
