//
// PoseManager.hpp
//

#include <boost/shared_ptr.hpp>

class PoseManager {
public:
  PoseManager(boost::shared_ptr<AL::Broker> broker);
  ~PoseManager();
};
