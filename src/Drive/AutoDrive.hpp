//
// Drive.hh for  in /home/olivie_a//naoqi-sdk-1.12.5-linux64/examples/drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep  5 23:47:40 2012 samuel olivier
// Last update Tue Nov 13 23:53:21 2012 samuel olivier
//

#ifndef __AUTO_DRIVE_HH__
# define __AUTO_DRIVE_HH__

# include <map>

# include <alcommon/almodule.h>
# include <PoseManager.hpp>
# include <Animation.hpp>
# include <atomic>
# include <thread>
# include <mutex>

# include "DriveProxy.hpp"

namespace AL
{
  class ALBroker;
}

class AutoDrive : public AL::ALModule
{
public:

  AutoDrive(boost::shared_ptr<AL::ALBroker> broker,
	    const std::string &name);
  virtual ~AutoDrive();

  virtual void	init();
  void		start();
  void		stop();
  int		thread();

private:
  boost::shared_ptr<AL::ALBroker>	_broker;
  DriveProxy*				_proxy;
  std::thread				*_thread;
  std::atomic<bool>			_stopThread;
};

#endif
