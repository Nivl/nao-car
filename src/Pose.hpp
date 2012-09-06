//
// Pose.hpp
//

#include <string>

class Pose
{
public:
  Pose(std::string &name="");
  ~Pose();

private:
  std::string			_name;
  std::map<std::string, float>	_angles;
};
