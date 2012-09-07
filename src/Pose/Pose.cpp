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
  for(const std::pair<std::string, float>& angle : angles)
    _angles[angle.first] = angle.second;
}

#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>


bool		Pose::saveToFile(const std::string& filename)
{
  std::ofstream ofs(filename);

  //boost::archive::text_oarchive oa(ofs);

  //oa << _angles;
  return (true);
}
