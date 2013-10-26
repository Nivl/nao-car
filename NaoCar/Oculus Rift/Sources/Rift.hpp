//
// Rift.hpp
// NaoCar Remote
//

#ifndef _RIFT_HPP_
# define _RIFT_HPP_

# include <QObject>
# include <QTimer>
# include <OVR.h>

# include "RiftDelegate.hpp"
# include "VRView.hpp"

using namespace OVR;

class Rift : public QObject {
    Q_OBJECT
public:
    Rift(void);
    ~Rift(void);
    
    void setDelegate(RiftDelegate* delegate);
    Vector3f getOrientation(void);
    
    void setViewImage(const QImage& image);
    
    QWidget* getView(void);
    
    private slots:
    void _update(void);
    
    
private:
    RiftDelegate*       _delegate;
    System              _system;
    Ptr<DeviceManager>  _manager;
    Ptr<HMDDevice>      _HMD;
    Ptr<SensorDevice>   _sensor;
    SensorFusion        _sensorFusion;
    QTimer              _timer;
    VRView*             _view;
};

#endif
