//
// qtMain.cpp for  in /home/olivie_a//rendu/nao/nao-car/src/Drive
// 
// Made by samuel olivier
// Login   <olivie_a@epitech.net>
// 
// Started on  Fri Sep  7 19:38:07 2012 samuel olivier
// Last update Sat Sep  8 15:18:59 2012 samuel olivier
//

#include <iostream>

#include "Window.hpp"
#include <map>
#include <QApplication>
#include <QObject>

int	main(int ac, char **av)
{
  QApplication	app(ac, av);

  if (ac != 2)
    {
      std::cout << "Usage: " << av[0] << " ip" << std::endl;
      return (1);
    }
  boost::shared_ptr<AL::ALBroker> broker;
  try
    {
      broker = AL::ALBroker::createBroker(
					  "broker",
					  "0.0.0.0",
					  0,
					  av[1],
					  9559,
					  0);
    }
  catch (...)
    {
      std::cerr << "Error" << std::endl;
    }
  Window	window(broker);

  window.show();
  return (app.exec());
}
