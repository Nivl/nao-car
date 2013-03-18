//
// StreamServer.cpp for  in /home/olivie_a//rendu/nao/nao-car/NaoCar/Modules/RemoteServer
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Mar 15 16:30:36 2013 samuel olivier
// Last update Mon Mar 18 02:31:55 2013 samuel olivier
//

#include "StreamServer.hpp"
#include <fstream>

static GstFlowReturn appsink_new_preroll(GstAppSink *sink, gpointer user_data);
static GstFlowReturn appsink_new_buffer(GstAppSink *sink, gpointer user_data);

StreamServer::StreamServer(boost::asio::io_service* service) :
  _ioService(service), _mainThread(NULL), _tcpServer(NULL),
  _stop(false), _pipeline(NULL), _imageData(NULL), _imageChanged(false),
  _currentCamera(Bottom)
{
  gst_init(NULL, NULL);
}

StreamServer::~StreamServer() {
  stop();
  delete _tcpServer;
}

int	StreamServer::run() {
  if (_mainThread == NULL) {
    if (_tcpServer == NULL) {
      _tcpServer = new Network::BoostTcpServer(_ioService);
      _tcpServer->setDelegate(this);
      if (_tcpServer->listen(0, "") == false) {
	std::cerr << "could not listen on this port" << std::endl;
	return (0);
      }
    }
    std::cout << "Stream: " << _tcpServer->getPort() << std::endl;
    _stop = false;
    _mainThread = new boost::thread(&StreamServer::mainThread, this);
  }
  return (_tcpServer->getPort());
}

void	StreamServer::stop() {
  if (_mainThread != NULL) {
    _stop = true;
    _mainThread->join();
    delete _mainThread;
    _mainThread = NULL;
  }
  _stop = false;
}

void	StreamServer::mainThread() {
  _startPipeline();
  while (_stop == false) {
    _clientsMutex.lock();
    if (_imageChanged) {
      _imageMutex.lock();
      _imageChanged = false;
      for (auto it = _clients.begin(); it != _clients.end(); ++it) {
	char *data = new char[_imageSize];

	memcpy(data, _imageData, _imageSize);
	_writeData(*it, data, _imageSize);
      }
      _imageMutex.unlock();
    }
    _clientsMutex.unlock();
    usleep(10000);
  }
  _stopPipeline();
}

void	StreamServer::_startPipeline() {
  _clientsMutex.lock();
  if (_clients.size() > 0) {
    std::stringstream	tmp;

    tmp << "v4l2src device=";
    if (_currentCamera == (char)Bottom) {
      tmp << "/dev/video0";
    } else if (_currentCamera == (char)Opencv) {
      _stopPipeline();
      _clientsMutex.unlock();
      return ;
    } else {
      tmp << "/dev/video1";
    }
    tmp << " ! videoscale ! video/x-raw-yuv,width=320,height=240 "
      "! ffmpegcolorspace ! jpegenc";    
    _setPipeline(tmp.str());
  }
  _clientsMutex.unlock();
}

void	StreamServer::_stopPipeline() {
  if (_pipeline)
    {
      gst_element_set_state (_pipeline, GST_STATE_NULL);
      gst_object_unref(GST_OBJECT(_pipeline));
      gst_object_unref(GST_OBJECT(_gstAppsink));
      _pipeline = NULL;
    }  
}

void	StreamServer::_setPipeline(std::string const& pipeline)
{
  GError* error = NULL;

  _stopPipeline();
  _pipeline = gst_parse_launch((pipeline + " ! appsink").c_str(), &error);
  if (!_pipeline || error)
    {
      std::cerr << "Cannnot create pipeline" << std::endl;
      _pipeline = 0;
    }
  else
    {
      static int idx = 0;
      std::stringstream tmp;
      tmp << "appsink" << idx++;
      GstAppSinkCallbacks gstCallbacks = {
	NULL, appsink_new_preroll, appsink_new_buffer, NULL, { NULL }};
      _gstAppsink = gst_bin_get_by_name(GST_BIN(_pipeline),
					tmp.str().c_str());
      gst_app_sink_set_callbacks(GST_APP_SINK(_gstAppsink), &gstCallbacks,
				 this, NULL);
      gst_element_set_state(_pipeline, GST_STATE_PLAYING);
    }
}

void	StreamServer::setCamera(Camera type) {
  if (type != _currentCamera) {
    _currentCamera = type;
    _startPipeline();
  }
}

void	StreamServer::newConnection(Network::ATcpServer* sender,
				    Network::ATcpSocket* socket) {
  if (_tcpServer != sender)
    return ;
  socket->setDelegate(this);
  _clientsMutex.lock();
  _clients.push_back(socket);
  if (_clients.size() == 1) {
    _clientsMutex.unlock();
    _startPipeline();
  } else {
    _clientsMutex.unlock();
  }
  socket->readUntil("\n");
  std::cout << "Stream Connection " << _clients.size() << std::endl;
}

void	StreamServer::connected(Network::ASocket*,
				Network::ASocket::Error) {
}

void	StreamServer::readFinished(Network::ASocket*,
				   Network::ASocket::Error,
				   size_t) {
}

void	StreamServer::readFinished(Network::ASocket* sender,
				   Network::ASocket::Error error,
				   std::string const&) {
  Network::ATcpSocket	*socket = dynamic_cast<Network::ATcpSocket*>(sender);

  if (socket == NULL)
    return ;
  if (error) {
    _clientsMutex.lock();
    _clients.remove(socket);
    if (_clients.size() == 0)
      _stopPipeline();
    _clientsMutex.unlock();
    delete socket;
    std::cout << "Stream Deconnection " << _clients.size() << std::endl;
  } else {
    socket->readUntil("\n");
  }
}

void	StreamServer::writeFinished(Network::ASocket*,
				    Network::ASocket::Error,
				    size_t) {
  _imageMutex.lock();
  delete[] _toWrite.front().second->data;
  delete _toWrite.front().second;
  _toWrite.pop_front();
  if (_toWrite.size() >= 1)
    _toWrite.front().first->write(_toWrite.front().second->data,
				  _toWrite.front().second->size);
  _imageMutex.unlock();
}

void	StreamServer::_writeData(Network::ATcpSocket* target,
				 char* data, size_t size) {
  Packet	*packet = new Packet;
  packet->data = data;
  packet->size = size;
  _toWrite.push_back(std::pair<Network::ATcpSocket*, Packet*>(target, packet));
  if (_toWrite.size() == 1)
    target->write(data, size);
}

void	StreamServer::setImageData(char *data, size_t size) {
  _imageMutex.lock();
  delete _imageData;
  _imageChanged = true;
  _imageData = new char[size + 8];
  memcpy(_imageData, &size, 8);
  memcpy(_imageData + 8, data, size);
  _imageSize = size + 8;
  _imageMutex.unlock();
}

void	StreamServer::setOpencvData(char *data, size_t size) {
  if (_currentCamera == Opencv) {
    _imageMutex.lock();
    delete _imageData;
    _imageChanged = true;
    _imageData = new char[size + 8];
    memcpy(_imageData, &size, 8);
    memcpy(_imageData + 8, data, size);
    _imageSize = size + 8;
    _imageMutex.unlock();
  }
}

static GstFlowReturn appsink_new_preroll(GstAppSink *sink, gpointer user_data)
{
  (void)user_data;
  gst_app_sink_pull_preroll(sink);
  return GST_FLOW_OK;
}

static GstFlowReturn appsink_new_buffer(GstAppSink *sink, gpointer user_data)
{
  (void)user_data;
  GstBuffer *buffer = gst_app_sink_pull_buffer(sink);
  unsigned char* data = GST_BUFFER_MALLOCDATA(buffer);
  ((StreamServer*)user_data)->setImageData((char*)data,
					   GST_BUFFER_SIZE(buffer));
  gst_buffer_unref(buffer);
  return GST_FLOW_OK;
}
