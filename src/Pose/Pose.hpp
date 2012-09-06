//
// Pose.hpp
//

#ifndef _POSE_HPP_
# define _POSE_HPP_

# include <string>
# include <map>

class Pose
{
public:
  static Pose loadFromFile(const std::string& filename);

  Pose(const std::string& name="");
  Pose(const Pose& other);
  ~Pose();

  std::string	getName() const;
  void		setName(const std::string& name);

  float		getAngle(const std::string& angle);
  void		setAngle(const std::string& angle, float value);
  void		setAngles(const std::map<std::string, float>& angles);

  bool		saveToFile(const std::string& filename);

private:
  std::string			_name;
  std::map<std::string, float>	_angles;
};

#endif
