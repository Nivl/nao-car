//
// Pose.cpp
//


#include <fstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <string>
#include "Pose.hpp"

Pose	Pose::loadFromFile(const std::string& filename)
{
  Pose	pose;
  std::ifstream file(filename);
  
  if (file.fail())
    return (pose);
  try {
    std::string line;
    getline(file, line);
    pose._name = line;
    while (getline(file, line))
      {
	pose._angles[line.substr(0, line.find(";"))] = float(atof(line.substr(line.find(";") + 1).c_str()));
      }
  }
  catch (...){}
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

#include <iomanip>

std::ostream& operator<<(std::ostream& os, const Pose& pose)
{
  const std::map<std::string, float>& angles = pose.getAngles();
  os << pose.getName() << ":";
  for (const std::pair<std::string, float>& angle : angles)
    os << std::endl << std::left << std::setw(15) << angle.first << "--> " << angle.second;
  return (os);
}
