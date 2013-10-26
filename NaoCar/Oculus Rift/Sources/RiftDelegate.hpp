//
//  RiftDelegate.hpp
//  NaoCar VRemote
//

#ifndef NaoCar_VRemote_RiftDelegate_hpp
# define NaoCar_VRemote_RiftDelegate_hpp

# include <OVR.h>

using namespace OVR;

class RiftDelegate {
public:
    virtual ~RiftDelegate(void) {};
    
    virtual void riftOrientationUpdate(Vector3f orientation) = 0;
};

#endif
