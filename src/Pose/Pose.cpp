//
// Pose.cpp
//

#include "Pose.hpp"

Pose	Pose::loadFromFile(const std::string& filename)
{
  Pose	pose;

  return (pose);
}

Pose::Pose(const std::string& name) :
  _name(name), _angles()
{
  
}

Pose::Pose(const Pose& other) :
  _name(other._name), _angles(other._angles)
{

}

Pose::~Pose()
{
}

std::string	Pose::getName() const
{
  return (_name);
}

void		Pose::setName(const std::string& name)
{
  _name = name;
}

float		Pose::getAngle(const std::string& angle)
{
  return (_angles[angle]);
}

const std::map<std::string, float>&
		Pose::getAngles() const
{
  return (_angles);
}

void		Pose::setAngle(const std::string& angle, float value)
{
  _angles[angle] = value;
}

void		Pose::setAngles(const std::map<std::string, float>& angles)
{
  for(const std::pair<std::string, float>& angle : angles)
    _angles[angle.first] = angle.second;
}

#include <fstream>
#include <iostream>
#include <boost/lexical_cast.hpp>

bool		Pose::saveToFile(const std::string& filename)
{
  try {
    std::ofstream of(filename);
    std::string	res = _name + "\n";
    
    if (of.fail())
      return (false);
    for (const std::pair<std::string, float>& angle : _angles)
      res += angle.first + ";" + boost::lexical_cast<std::string>(angle.second) + "\n";
    of << res;
  }
  catch (...)
    {
      return (false);
    }
  return (true);
}

std::ostream& operator<<(std::ostream& os, const Pose& pose)
{
  const std::map<std::string, float>& angles = pose.getAngles();
  os << pose.getName() << ":";
  for (const std::pair<std::string, float>& angle : angles)
    os << std::endl << angle.first << "\t-->" << angle.second;
  return (os);
}
