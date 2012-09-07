//
// Animation.cpp
//


#include <fstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <string>
#include "Animation.hpp"

Animation	Animation::loadFromFile(const std::string& filename)
{
  Animation	animation;
  std::ifstream file(filename);
  
  if (file.fail())
    return (animation);
  try
    {
      std::string line;
      getline(file, line);
      animation._name = line;
      while (getline(file, line))
	{
	  animation._files.push_back({line.substr(0, line.find(";")),
		float(atof(line.substr(line.find(";") + 1).c_str()))});
	  animation._poses.push_back({Pose::loadFromFile(line.substr(0, line.find(";"))),
		float(atof(line.substr(line.find(";") + 1).c_str()))});
	}
    }
  catch (...){}
  return (animation);
}

Animation::Animation(const std::string& name) :
  _name(name), _files()
{
  
}

Animation::Animation(const Animation& other) :
  _name(other._name), _files(other._files)
{

}

Animation::~Animation()
{
}

std::string	Animation::getName() const
{
  return (_name);
}

void		Animation::setName(const std::string& name)
{
  _name = name;
}

void		Animation::addPose(const std::string& filename, float time)
{
  _files.push_back({filename, time});
  _poses.push_back({Pose::loadFromFile(filename), time});
}

void		Animation::animate(PoseManager& poseManager)
{
  for (const std::pair<Pose, float>& pose: _poses)
    poseManager.takePose(pose.first, pose.second);
}

bool		Animation::saveToFile(const std::string& filename)
{
  try {
    std::ofstream of(filename);
    std::string	res = _name + "\n";
    
    if (of.fail())
      return (false);
    for (const std::pair<std::string, float>& file : _files)
      res += file.first + ";" + boost::lexical_cast<std::string>(file.second) + "\n";
    of << res;
  }
  catch (...)
    {
      return (false);
    }
  return (true);
}

const std::list<std::pair<std::string, float>>& Animation::getFiles() const
{
  return (_files);
}

#include <iomanip>

std::ostream& operator<<(std::ostream& os, const Animation& animation)
{
  const std::list<std::pair<std::string, float>>& files = animation.getFiles();
  os << animation.getName() << ":";
  for (const std::pair<std::string, float>& file : files)
    os << std::endl << std::left << std::setw(15) << file.first << "--> " << file.second;
  return (os);
}
