//
// sfWindow.cpp for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep 26 18:34:52 2012 samuel olivier
// Last update Fri Sep 28 03:12:11 2012 samuel olivier
//

#include <iostream>
#include "sfWindow.hpp"

static void	launchVoiceThread(Window *win)
{
  win->voiceThread();
}

static void	launchAnimThread(Window *win)
{
  win->animThread();
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
  return GST_FLOW_OK;
}

Window::Window(boost::shared_ptr<AL::ALBroker> broker, int joystickId,
	       const char *ip) :
  _window(sf::VideoMode(640, 480), "NaoCar"), _poseManager(broker),
  _steeringWheelIsTaken(false), _gasPedalIsPushed(false),
  _steeringWheelDirection(Front), _speed(Up), _joystickId(joystickId),
  _noHand(false), _t2p(broker), _stopThread(false),
  _voiceThread(launchVoiceThread, this), _streamImageChanged(false),
  _pipeline(NULL), _resize(true), _animThread(launchAnimThread, this),
  _isAnimating(false), _currentCameraMode(0)
{
  _animations["DownShift"] =
    Anim{Animation::loadFromFile("Poses/downshift.anim"), Down,
	 (std::atomic<int> (Window::*))&Window::_speed};
  _animations["UpShift"] =
    Anim{Animation::loadFromFile("Poses/upshift.anim"), Up,
	 (std::atomic<int> (Window::*))&Window::_speed};
  _animations["PushGasPedal"] =
    Anim{Animation::loadFromFile("Poses/push-gas-pedal.anim"), true,
	 (std::atomic<int> (Window::*))&Window::_gasPedalIsPushed};
  _animations["ReleaseGasPedal"] =
    Anim{Animation::loadFromFile("Poses/release-gas-pedal.anim"), false,
	 (std::atomic<int> (Window::*))&Window::_gasPedalIsPushed};
  _animations["TakeSteeringWheel"] =
    Anim{Animation::loadFromFile("Poses/take-steering-wheel.anim"), true,
	 (std::atomic<int> (Window::*))&Window::_steeringWheelIsTaken};
  _animations["ReleaseSteeringWheel"] =
    Anim{Animation::loadFromFile("Poses/release-steering-wheel.anim"), false,
	 (std::atomic<int> (Window::*))&Window::_steeringWheelIsTaken};
  _animations["TurnLeft"] =
    Anim{Animation::loadFromFile("Poses/turn-left.anim"), Left,
	 (std::atomic<int> (Window::*))&Window::_steeringWheelDirection};
  _animations["TurnFront"] =
    Anim{Animation::loadFromFile("Poses/turn-front.anim"), Front,
	 (std::atomic<int> (Window::*))&Window::_steeringWheelDirection};
  _animations["TurnRight"] =
    Anim{Animation::loadFromFile("Poses/turn-right.anim"), Right,
	 (std::atomic<int> (Window::*))&Window::_steeringWheelDirection};
  _animations["BeginNoHand"] =
    Anim{Animation::loadFromFile("Poses/begin-no-hand.anim"), true,
	 (std::atomic<int> (Window::*))&Window::_noHand};
  _animations["StopNoHand"] =
    Anim{Animation::loadFromFile("Poses/stop-no-hand.anim"), false,
	 (std::atomic<int> (Window::*))&Window::_noHand};
  addAnim("ReleaseSteeringWheel");
  addAnim("UpShift");
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

  _cameraModes[0] = CameraMode{"SelectCameraTop", "1"};
  _cameraModes[1] = CameraMode{"SelectCameraBottom", "3"};
  _socket.connect(sf::IpAddress(ip), 8080);
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

void	Window::animThread()
{
  while (_stopThread == false)
    {
      std::string	current;
      bool		move = true;

      _animListMutex.lock();
      if (_animList.size() == 0)
	{
	  move = false;
	  _isAnimating = false;
	}
      else
	{
	  _isAnimating = true;
	  current = _animList.front();
	  _animList.pop_front();
	}
      _animListMutex.unlock();
      if (move == true)
	{
	  launch(current);
	  _animationsMutex.lock();
	  (this->*_animations[current]._valueToChange) =
	    _animations[current]._valueToGive;
	  _animationsMutex.unlock();
	}
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
      tmpSprite.setTexture(_textures[_speed == Up ? "CurrentSpeedUp" : 
				     "CurrentSpeedDown"], true);
      tmpSprite.setPosition(_window.getSize().x - 65,
			    _window.getSize().y - 107);
      _window.draw(tmpSprite);
      tmpSprite.setTexture(_textures[_speed == Up && _gasPedalIsPushed ?
				     "MoveUpOn" : "MoveUpOff"], true);
      tmpSprite.setPosition((float)_window.getSize().x / 2 - 127.0 / 2,
			    _window.getSize().y - 146);
      _window.draw(tmpSprite);
      tmpSprite.setTexture(_textures[_speed == Down && _gasPedalIsPushed ?
				     "MoveDownOn" : "MoveDownOff"], true);
      tmpSprite.setPosition((float)_window.getSize().x / 2 - 127.0 / 2,
			    _window.getSize().y - 71);
      _window.draw(tmpSprite);
      tmpSprite.setTexture(_textures[_steeringWheelDirection == Left ?
				     "MoveLeftOn" : "MoveLeftOff"], true);
      tmpSprite.setPosition((float)_window.getSize().x / 2 - 125,
			    _window.getSize().y - 116);
      _window.draw(tmpSprite);
      tmpSprite.setTexture(_textures[_steeringWheelDirection == Right ?
				     "MoveRightOn" : "MoveRightOff"], true);
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
  _animThread.join();
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
      if (_isAnimating == false &&
	  event->joystickButton.button == 0 && _noHand == false)
	{
	  addAnim(_steeringWheelIsTaken == true ? "ReleaseSteeringWheel" :
		  "TakeSteeringWheel");
	}
      if (event->joystickButton.button == 4)
	{
	  _currentCameraMode = (_currentCameraMode - 1 + _cameraModes.size()) %
	    _cameraModes.size();
	  _socket.send(_cameraModes[_currentCameraMode]._networkValue.c_str(),
		       _cameraModes[_currentCameraMode]._networkValue.size());
	}
      if (event->joystickButton.button == 5)
	{
	  _currentCameraMode = (_currentCameraMode + 1) % _cameraModes.size();
	  _socket.send(_cameraModes[_currentCameraMode]._networkValue.c_str(),
		       _cameraModes[_currentCameraMode]._networkValue.size());
	}
    }
}

void	Window::checkJoystick()
{
  if (_isAnimating == false &&
      !sf::Joystick::isButtonPressed(_joystickId, 2) && _noHand == true)
    {
      addAnim("StopNoHand");
      addAnim("TakeSteeringWheel");
    }
  if (_isAnimating == false && sf::Joystick::isButtonPressed(_joystickId, 2)
      && _noHand == false)
    {
      _textListMutex.lock();
      _textList.push_back("Sans les mains");
      _textListMutex.unlock();
      if (_steeringWheelDirection != Front)
	addAnim("TurnFront");
      addAnim("BeginNoHand");
    }
  if (sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::R) > 0)
    up();
  else if (sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::Z) > 0)
    down();
  else
    stopPush();
  if (sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::X) < -50)
    left();
  else if (sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::X) > 50)
    right();
  else
    stopTurn();
  static int pX = 0, pY = 0;
  pX = sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::U);
  pY = sf::Joystick::getAxisPosition(_joystickId, sf::Joystick::V);
  static std::vector<std::string>	names =
    {"HeadYaw", "HeadPitch"};
  std::vector<float>			angles =
    {((float)(-pX + 100) / 200.f) * (120.f + 120.f) - 120.f,
     ((float)(pY + 100) / 200.f) * (30.f + 40.f) - 40.f};
  angles[0] = angles[0] * M_PI / 180;
  angles[1] = angles[1] * M_PI / 180;
  
  _poseManager.getMotionProxy().setAngles(names, angles, 0.4);
}

void	Window::keyPressEvent(sf::Event *event)
{
  if (event->key.code == sf::Keyboard::Up)
    up();
  else if (event->key.code == sf::Keyboard::Down)
    down();
  else if (event->key.code == sf::Keyboard::Left)
    left();
  else if (event->key.code == sf::Keyboard::Right)
    right();
  // else if (event->key.code == sf::Keyboard::Space)
  //   {
  //     addAnim(_steeringWheelIsTaken == true ? "ReleaseSteeringWheel" :
  // 	     "TakeSteeringWheel");
  //     _steeringWheelIsTaken = !_steeringWheelIsTaken;
  //   }
}

void	Window::keyReleaseEvent(sf::Event *event)
{
  if ((event->key.code == sf::Keyboard::Up && _speed == Up) ||
      (event->key.code == sf::Keyboard::Down && _speed == Down))
    stopPush();
  else if (_steeringWheelIsTaken == true &&
	   (event->key.code == sf::Keyboard::Left ||
	    event->key.code == sf::Keyboard::Right))
    stopTurn();
}

void	Window::launch(std::string const& name)
{
  try
    {
      Animation	tmp;
      _animationsMutex.lock();
      tmp = _animations[name]._anim;
      _animationsMutex.unlock();
      tmp.animate(_poseManager);
    }
  catch (...)
    {
    }
  std::cout << "Launching : " << name << std::endl;
}

void	Window::addAnim(std::string const& name)
{
  _animListMutex.lock();
  _animList.push_back(name);
  _animListMutex.unlock();
}

void	Window::up()
{
  if (_isAnimating == true || _gasPedalIsPushed == true)
    return ;
  if (_speed == Down)
    {
      if (_steeringWheelIsTaken == true && _noHand == false)
	{
	  if (_steeringWheelDirection != Front)
	    addAnim("TurnFront");
	  addAnim("ReleaseSteeringWheel");
	}
      else if (_noHand == true)
	addAnim("StopNoHand");
      addAnim("UpShift");
    }
  if (_steeringWheelIsTaken == false && _noHand == false)
    addAnim("TakeSteeringWheel");
  if (_gasPedalIsPushed == false &&
      (_steeringWheelIsTaken == true || _noHand == true) && _speed == Up)
    {
      _animationsMutex.lock();
      _poseManager.setPose(_animations["PushGasPedal"]._anim.getPoses()
			   .front().first, 0.5);
      _animationsMutex.unlock();
      _gasPedalIsPushed = true;
    }
}

void	Window::down()
{
  if (_isAnimating == true || _gasPedalIsPushed == true)
    return ;
  if (_speed == Up)
    {
      if (_steeringWheelIsTaken == true && _noHand == false)
	{
	  if (_steeringWheelDirection != Front)
	    addAnim("TurnFront");
	  addAnim("ReleaseSteeringWheel");
	}
      else if (_noHand == true)
	addAnim("StopNoHand");
      addAnim("DownShift");
    }
  if (_steeringWheelIsTaken == false && _noHand == false)
    addAnim("TakeSteeringWheel");
  if (_gasPedalIsPushed == false &&
      (_steeringWheelIsTaken == true || _noHand == true) && _speed == Down)
    {
      _animationsMutex.lock();
      _poseManager.setPose(_animations["PushGasPedal"]._anim.getPoses()
			   .front().first, 0.5);
      _animationsMutex.unlock();
      _gasPedalIsPushed = true;
    }
}

void	Window::left()
{
  if (_isAnimating == true)
    return ;
  if (_steeringWheelIsTaken == false && _noHand == false)
    addAnim("TakeSteeringWheel");
  if (_steeringWheelIsTaken == true && _noHand == false
      && _steeringWheelDirection != Left)
    {
      _animationsMutex.lock();
      _poseManager.setPose(_animations["TurnLeft"]._anim.getPoses()
			   .front().first, 0.5);
      _animationsMutex.unlock();
      _steeringWheelDirection = Left;
    }
}

void	Window::right()
{
  if (_isAnimating == true)
    return ;
  if (_steeringWheelIsTaken == false && _noHand == false)
    addAnim("TakeSteeringWheel");
  if (_steeringWheelIsTaken == true && _noHand == false
      && _steeringWheelDirection != Right)
    {
      _animationsMutex.lock();
      _poseManager.setPose(_animations["TurnRight"]._anim.getPoses()
			   .front().first, 0.5);
      _animationsMutex.unlock();
      _steeringWheelDirection = Right;
    }
}

void	Window::stopPush()
{
  if (_gasPedalIsPushed == true)
    {
      _animationsMutex.lock();
      _poseManager.setPose(_animations["ReleaseGasPedal"]._anim.getPoses()
			   .front().first, 0.5);
      _animationsMutex.unlock();
      _gasPedalIsPushed = false;
    }
}

void	Window::stopTurn()
{
  if (_steeringWheelIsTaken == true && _steeringWheelDirection != Front)
    {
      _animationsMutex.lock();
      _poseManager.setPose(_animations["TurnFront"]._anim.getPoses()
			   .front().first, 0.5);
      _animationsMutex.unlock();
      _steeringWheelDirection = Front;
    }
}
