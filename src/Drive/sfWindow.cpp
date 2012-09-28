//
// sfWindow.cpp for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep 26 18:34:52 2012 samuel olivier
//

#include <iostream>
#include "sfWindow.hpp"

static void	launchVoiceThread(Window *win)
{
  win->voiceThread();
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
  GstStructure *structure = gst_caps_get_structure(caps, 0);

  unsigned char* data = GST_BUFFER_MALLOCDATA(buffer);

  gint width = 0, height = 0, bpp = 0;
  gst_structure_get_int(structure, "width", &width);
  gst_structure_get_int(structure, "height", &height);
  gst_structure_get_int(structure, "bpp", &bpp);
  ((Window *)user_data)->setStreamImage(data, width, height, bpp);
  gst_caps_unref(caps);
  gst_buffer_unref(buffer);
  return GST_FLOW_OK;
}

Window::Window(boost::shared_ptr<AL::ALBroker> broker, int joystickId,
	       const char *ip) :
  _module(broker), _window(sf::VideoMode(640, 480), "NaoCar"),
  _joystickId(joystickId), _t2p(broker), _stopThread(false),
  _voiceThread(launchVoiceThread, this), _streamImageChanged(false),
  _pipeline(NULL), _resize(true), _currentCameraMode(0)
{
  _window.setVerticalSyncEnabled(true);
  _window.setFramerateLimit(30); 
  sf::Texture	tmp;

  tmp.loadFromFile("resources/move-left-active.png");
  _textures["MoveLeftOn"] = tmp;
  tmp.loadFromFile("resources/move-left-inactive.png");
  _textures["MoveLeftOff"] = tmp;
  tmp.loadFromFile("resources/move-right-active.png");
  _textures["MoveRightOn"] = tmp;
  tmp.loadFromFile("resources/move-right-inactive.png");
  _textures["MoveRightOff"] = tmp;
  tmp.loadFromFile("resources/move-up-active.png");
  _textures["MoveUpOn"] = tmp;
  tmp.loadFromFile("resources/move-up-inactive.png");
  _textures["MoveUpOff"] = tmp;
  tmp.loadFromFile("resources/move-down-active.png");
  _textures["MoveDownOn"] = tmp;
  tmp.loadFromFile("resources/move-down-inactive.png");
  _textures["MoveDownOff"] = tmp;
  tmp.loadFromFile("resources/shift-up.png");
  _textures["CurrentSpeedUp"] = tmp;
  tmp.loadFromFile("resources/shift-down.png");
  _textures["CurrentSpeedDown"] = tmp;
  tmp.loadFromFile("resources/say-field.png");
  _textures["Say"] = tmp;
  tmp.loadFromFile("resources/button-camera-front.png");
  _textures["SelectCameraTop"] = tmp;
  tmp.loadFromFile("resources/button-camera-bottom.png");
  _textures["SelectCameraBottom"] = tmp;
  tmp.loadFromFile("resources/logo.png");
  _textures["Logo"] = tmp;  
  _sayFont.loadFromFile("resources/arial.ttf");
  _module.start();
  _cameraModes[0] = CameraMode{"SelectCameraTop", "1"};
  _cameraModes[1] = CameraMode{"SelectCameraBottom", "3"};
  _isConnected = _socket.connect(sf::IpAddress(ip), 8080) == sf::TcpSocket::Done;
  if (_isConnected)
    _socket.send(_cameraModes[_currentCameraMode]._networkValue.c_str(),
		 _cameraModes[_currentCameraMode]._networkValue.size());
  setPipeline("udpsrc port=8081 ! smokedec ! ffmpegcolorspace ! "
	      "video/x-raw-rgb,bpp=32");
}

Window::~Window()
{
}

void	Window::voiceThread()
{
  while (_stopThread == false)
    {
      std::string	current;
      bool		say = true;

      _textListMutex.lock();
      if (_textList.size() == 0)
	say = false;
      else
	{
	  current = _textList.front();
	  _textList.pop_front();
	}
      _textListMutex.unlock();
      if (say == true)
	_t2p.say(current);
      else
	usleep(1000);
    }
}

void	Window::setStreamImage(unsigned char *data, gint width,
			       gint height, gint bpp)
{
  if (bpp == 32)
    {
      _streamImageMutex.lock();
      _streamImageChanged = true;
      _streamImage.create(width, height, data);
      _streamImageMutex.unlock();
    }
}

void	Window::setPipeline(std::string const& pipeline)
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

void	Window::exec()
{
  sf::Texture tmp;
  int	w = 0, h = 0;

  while (_window.isOpen())
    {
      sf::Event	event;
      while (_window.pollEvent(event))
	checkEvent(&event);
      checkJoystick();
      _window.clear();
      if (_streamImageChanged)
	{
	  _streamImageMutex.lock();
	  tmp.loadFromImage(_streamImage);
	  tmp.bind();
	  w = _streamImage.getSize().x;
	  h = _streamImage.getSize().y;
	  _streamSprite.setTexture(tmp);
	  _streamImageMutex.unlock();
	  _streamImageChanged = false;
	  _resize = true;
	}
      if (_resize && w != 0 && h != 0)
	_streamSprite.setScale(sf::Vector2f((float)_window.getSize().x / w,
					    (float)_window.getSize().y / h));
      if (_resize == true)
	  _resize = false;
      _window.draw(_streamSprite);
      sf::Sprite tmpSprite;

      tmpSprite.setTexture(_textures[_cameraModes[_currentCameraMode]._textureName], true);
      tmpSprite.setPosition(12, 7);
      _window.draw(tmpSprite);
      tmpSprite.setTexture(_textures["Logo"], true);
      tmpSprite.setPosition(_window.getSize().x - 89, 7);
      _window.draw(tmpSprite);
      tmpSprite.setTexture(_textures["Say"], true);
      tmpSprite.setPosition(12, _window.getSize().y - 46);
      _window.draw(tmpSprite);
      tmpSprite.setTexture(_textures[_module.speed() == Drive::Up ?
				     "CurrentSpeedUp" : 
				     "CurrentSpeedDown"], true);
      tmpSprite.setPosition(_window.getSize().x - 65,
			    _window.getSize().y - 107);
      _window.draw(tmpSprite);
      tmpSprite.setTexture(_textures[_module.speed() == Drive::Up
				     && _module.isGasPedalPushed() ?
				     "MoveUpOn" : "MoveUpOff"], true);
      tmpSprite.setPosition((float)_window.getSize().x / 2 - 127.0 / 2,
			    _window.getSize().y - 146);
      _window.draw(tmpSprite);
      tmpSprite.setTexture(_textures[_module.speed() == Drive::Down
				     && _module.isGasPedalPushed() ?
				     "MoveDownOn" : "MoveDownOff"], true);
      tmpSprite.setPosition((float)_window.getSize().x / 2 - 127.0 / 2,
			    _window.getSize().y - 71);
      _window.draw(tmpSprite);
      tmpSprite.setTexture(_textures[_module.steeringWheelDirection() ==
				     Drive::Left ? "MoveLeftOn" :
				     "MoveLeftOff"], true);
      tmpSprite.setPosition((float)_window.getSize().x / 2 - 125,
			    _window.getSize().y - 116);
      _window.draw(tmpSprite);
      tmpSprite.setTexture(_textures[_module.steeringWheelDirection() ==
				     Drive::Right ? "MoveRightOn"
				     : "MoveRightOff"], true);
      tmpSprite.setPosition((float)_window.getSize().x / 2 + 66,
			    _window.getSize().y - 116);
      _window.draw(tmpSprite);
      std::string toShow = _currentText;
      if (toShow.size() > 22)
	toShow = "..." + toShow.substr(toShow.size() - 19);
      sf::Text	text(toShow, _sayFont, 18);
      text.setPosition(20, _window.getSize().y - 25);
      _window.draw(text);
      _window.display();
    }
  _stopThread = true;
  _voiceThread.join();
  _module.stop();
  if (_pipeline)
    {
      gst_element_set_state (_pipeline, GST_STATE_NULL);
      gst_object_unref (GST_OBJECT (_pipeline));
      _pipeline = NULL;
    }
}

void	Window::checkEvent(sf::Event *event)
{
  if (event->type == sf::Event::Closed)
    _window.close();
  else if (event->type == sf::Event::KeyPressed)
    keyPressEvent(event);
  else if (event->type == sf::Event::KeyReleased)
    keyReleaseEvent(event);
  else if (event->type == sf::Event::Resized)
    {
      _window.setView(sf::View(sf::FloatRect(0, 0, event->size.width,
					     event->size.height)));
      _resize = true;
    }
  else if (event->type == sf::Event::TextEntered)
    {
      if (event->text.unicode == '\r' || event->text.unicode == '\n')
	{
	  _textListMutex.lock();
	  _textList.push_back(_currentText);
	  _textListMutex.unlock();
	  _currentText.clear();
	}
      else if (event->text.unicode == '\b' && _currentText.size() > 0)
	_currentText = _currentText.substr(0, _currentText.size() - 1);
      else if (event->text.unicode != '\b')
      	_currentText += (char)event->text.unicode;
    }
  if (event->type == sf::Event::JoystickButtonPressed &&
      event->joystickButton.joystickId == _joystickId)
    {
      if (_module.isAnimating() == false &&
	  event->joystickButton.button == 0 && _module.isNoHand() == false)
	{
	  if (_module.steeringWheelIsTaken() == true)
	    _module.releaseSteeringWheel();
	  else
	    _module.takeSteeringWheel();
	}
      if (event->joystickButton.button == 4)
	_currentCameraMode = (_currentCameraMode - 1 + _cameraModes.size()) %
	  _cameraModes.size();
      if (event->joystickButton.button == 5)
	_currentCameraMode = (_currentCameraMode + 1) % _cameraModes.size();
      if ((event->joystickButton.button == 5 ||
	   event->joystickButton.button == 4) && _isConnected)
	_socket.send(_cameraModes[_currentCameraMode]._networkValue.c_str(),
		     _cameraModes[_currentCameraMode]._networkValue.size());
    }
}

void	Window::checkJoystick()
{
  if (_module.isAnimating() == false &&
      !sf::Joystick::isButtonPressed(_joystickId, 2) &&
      _module.isNoHand() == true)
    {
      _module.endNoHand();
      _module.takeSteeringWheel();
    }
  if (_module.isAnimating() == false &&
      sf::Joystick::isButtonPressed(_joystickId, 2) &&
      _module.isNoHand() == false)
    {
      _textListMutex.lock();
      _textList.push_back("Sans les mains");
      _textListMutex.unlock();
      if (_module.steeringWheelDirection() != Drive::Front)
	_module.stopTurn();
      _module.beginNoHand();
    }
  if (sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::R) > 0)
    _module.up();
  else if (sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::Z) > 0)
    _module.down();
  else
    _module.stopPush();
  if (sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::X) < -50)
    _module.left();
  else if (sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::X) > 50)
    _module.right();
  else
    _module.stopTurn();
  static int pX = 0, pY = 0;
  pX = sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::U);
  pY = sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::V);
  _module.setHead((((float)(-pX + 100) / 200.f) * (120.f + 120.f) - 120.f) *
		    M_PI / 180,
		    (((float)(pY + 100) / 200.f) * (30.f + 40.f) - 40.f) *
		    M_PI / 180, 0.4);
}

void	Window::keyPressEvent(sf::Event *event)
{
  if (event->key.code == sf::Keyboard::Up)
    _module.up();
  else if (event->key.code == sf::Keyboard::Down)
    _module.down();
  else if (event->key.code == sf::Keyboard::Left)
    _module.left();
  else if (event->key.code == sf::Keyboard::Right)
    _module.right();
}

void	Window::keyReleaseEvent(sf::Event *event)
{
  if ((event->key.code == sf::Keyboard::Up && _module.speed() == Drive::Up) ||
      (event->key.code == sf::Keyboard::Down &&
       _module.speed() == Drive::Down))
    _module.stopPush();
  else if (_module.steeringWheelIsTaken() == true &&
	   (event->key.code == sf::Keyboard::Left ||
	    event->key.code == sf::Keyboard::Right))
    _module.stopTurn();
}
