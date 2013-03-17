//
// StreamServer.cpp for  in /home/olivie_a//rendu/nao/nao-car/NaoCar/Modules/RemoteServer
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Mar 15 16:30:36 2013 samuel olivier
// Last update Sat Mar 16 21:16:59 2013 samuel olivier
//

#include "StreamServer.hpp"
#include <fstream>

static GstFlowReturn appsink_new_preroll(GstAppSink *sink, gpointer user_data);
static GstFlowReturn appsink_new_buffer(GstAppSink *sink, gpointer user_data);

StreamServer::StreamServer(boost::asio::io_service* service) :
  _ioService(service), _mainThread(NULL), _tcpServer(NULL),
  _stop(false), _imageData(NULL), _imageChanged(false)
{
  gst_init(NULL, NULL);
}

StreamServer::~StreamServer() {
  for (auto it = _clients.begin(); it != _clients.end(); ++it) {
    (*it)->close();
    delete *it;
  }
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
  _setPipeline("v4l2src device=/dev/video0 ! videoscale ! video/x-raw-yuv,width=320,height=240 ! ffmpegcolorspace ! jpegenc");
  while (_stop == false) {
    _clientsMutex.lock();
    if (_imageChanged) {
      _imageMutex.lock();
      _imageChanged = false;
      for (auto it = _clients.begin(); it != _clients.end(); ++it) {
	uint64_t *size = new uint64_t;
	*size = _imageSize;
	_writeData(*it, (char*)size, sizeof(uint64_t));
	_writeData(*it, _imageData, _imageSize);
      }
      _imageMutex.unlock();
    }
    _clientsMutex.unlock();
    usleep(10000);
  }
}

void	StreamServer::newConnection(Network::ATcpServer* sender,
				    Network::ATcpSocket* socket) {
  if (_tcpServer != sender)
    return ;
  socket->setDelegate(this);
  _clientsMutex.lock();
  _clients.push_back(socket);
  _clientsMutex.unlock();
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

void	StreamServer::_setPipeline(std::string const& pipeline)
{
  GError* error = NULL;

  if (_pipeline)
    {
      gst_element_set_state (_pipeline, GST_STATE_NULL);
      gst_object_unref (GST_OBJECT (_pipeline));
      _pipeline = NULL;
    }
  _pipeline = gst_parse_launch((pipeline + " ! appsink").c_str(), &error);
  if (!_pipeline || error)
    {
      std::cerr << "Cannnot create pipeline" << std::endl;
      _pipeline = 0;
    }
  else
    {
      GstElement* appsink = gst_bin_get_by_name(GST_BIN(_pipeline),
						"appsink0");
      GstAppSinkCallbacks callbacks = {
	NULL, appsink_new_preroll, appsink_new_buffer, NULL, { NULL }};
      gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &callbacks,
				 this, NULL);
      gst_element_set_state(_pipeline, GST_STATE_PLAYING);
    }
}

void	StreamServer::setImageData(char *data, size_t size) {
  _imageMutex.lock();
  if (_imageChanged)
    delete _imageData;
  _imageChanged = true;
  _imageData = new char[size];
  memcpy(_imageData, data, size);
  _imageSize = size;
  _imageMutex.unlock();
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
  GstCaps *caps = gst_buffer_get_caps(buffer);
  unsigned char* data = GST_BUFFER_MALLOCDATA(buffer);
  ((StreamServer*)user_data)->setImageData((char*)data,
					   GST_BUFFER_SIZE(buffer));
  gst_caps_unref(caps);
  gst_buffer_unref(buffer);
  return GST_FLOW_OK;
}
