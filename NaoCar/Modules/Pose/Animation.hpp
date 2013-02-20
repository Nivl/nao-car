//
// Animation.hpp
//

#ifndef _ANIMATION_HPP_
# define _ANIMATION_HPP_

# include <string>
# include <map>
# include <ostream>
# include <list>
# include <boost/shared_ptr.hpp>
# include <alerror/alerror.h>
# include <alcommon/albroker.h>
# include "Pose.hpp"
# include "PoseManager.hpp"

class Animation
{
public:
  static Animation loadFromFile(const std::string& filename);

  Animation(const std::string& name="");
  Animation(const Animation& other);
  ~Animation();

  std::string	getName() const;
  void		setName(const std::string& name);

  void		addPose(const std::string& filename, float time);
  bool		saveToFile(const std::string& filename);

  void		animate(PoseManager& poseManager);

  const std::list<std::pair<std::string, float>>& getFiles() const;
  const std::list<std::pair<Pose, float>>& getPoses() const;

private:
  std::string					_name;
  std::list<std::pair<std::string, float>>	_files;
  std::list<std::pair<Pose, float>>		_poses;
};

std::ostream& operator<<(std::ostream& os, const Animation& animation);

#endif
