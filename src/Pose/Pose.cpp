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

void		Pose::setAngle(const std::string& angle, float value)
{
  _angles[angle] = value;
}

void		Pose::setAngles(const std::map<std::string, float>& angles)
{
  
}

bool		Pose::saveToFile(const std::string& filename)
{
  return (true);
}
