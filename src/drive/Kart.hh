//
// Kart.hh for  in /home/olivie_a//naoqi-sdk-1.12.5-linux64/examples/kart
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Wed Sep  5 23:47:40 2012 samuel olivier
// Last update Thu Sep  6 00:07:28 2012 samuel olivier
//

#ifndef __KART_HH__
# define __KART_HH__

# include <alcommon/almodule.h>

namespace AL
{
  class ALBroker;
}

class Kart : public AL::ALModule
{
public:
  Kart(boost::shared_ptr<AL::ALBroker> broker,
       const std::string &name);
  virtual ~Kart();

  virtual void init();
};

#endif
