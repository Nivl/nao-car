//
// sfWindow.hh for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep 26 18:34:54 2012 samuel olivier
// Last update Thu Nov 15 19:39:56 2012 samuel olivier
//

#ifndef __WINDOW__
# define __WINDOW__

# include <alcommon/almodule.h>
# include <alproxies/altexttospeechproxy.h>
# include <alproxies/alvideodeviceproxy.h>
# include <alvision/alvisiondefinitions.h>
# include <Animation.hpp>
# include <PoseManager.hpp>
# include <SFML/Window.hpp>
# include <SFML/Graphics.hpp>
# include <SFML/Network.hpp>
# include <thread>
# include <mutex>
# include <atomic>
# include <gst/gst.h>
# include <glib.h>
# include <gst/app/gstappsink.h>

# include "DriveProxy.hpp"
# include "AutoDriveProxy.hpp"

namespace AL
{
  class ALBroker;
}

class Window
{
public:
  Window(boost::shared_ptr<AL::ALBroker> broker, int joystickId,
	 const char *ip);
  ~Window();

  void	exec();

  void	voiceThread();

  void	setStreamImage(unsigned char *data, gint width, gint height, gint bpp);
  void	setPipeline(std::string const& pipeline);

private:
  void	checkEvent(sf::Event *event);
  void	checkJoystick();
  void	checkKeyboard();

  struct CameraMode
  {
    std::string	_textureName;
    std::string	_networkValue;
  };

  DriveProxy			_module;
  sf::RenderWindow		_window;
  sf::Clock			_clock;
  unsigned int			_joystickId;

  AL::ALTextToSpeechProxy	_t2p;
  std::string			_currentText;
  std::list<std::string>	_textList;
  std::mutex			_textListMutex;
  std::atomic<bool>		_stopThread;
  std::thread			_voiceThread;

  sf::Sprite			_streamSprite;
  sf::Image			_streamImage;
  std::mutex			_streamImageMutex;
  bool				_streamImageChanged;
  GstElement			*_pipeline;
  bool				_resize;
  std::map<std::string, sf::Texture>	_textures;

  sf::Font			_sayFont;

  std::map<int, CameraMode>	_cameraModes;
  int				_currentCameraMode;
  sf::TcpSocket			_socket;
  bool				_isConnected;
  bool				_isAutoDriving;
  AutoDriveProxy		*_autoDriveProxy;
};

#endif
