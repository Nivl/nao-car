//
// Remote.hpp
// NaoCar Remote
//

#ifndef _REMOTE_HPP_
# define _REMOTE_HPP_

# include <QObject>
# include <QNetworkAccessManager>
# include <QNetworkReply>
# include <QUrl>
# include <QTcpSocket>
# include <QTimer>
# include <leap/Leap.h>

# include <map>

# include "MainWindow.hpp"
# include "MainWindowDelegate.hpp"
# include "Bonjour.hpp"
# include "BonjourDelegate.hpp"
# include "Rift.hpp"

# define NAOCAR_BONJOUR_SERVICE_NAME "nao-car"
using namespace Leap;

class LeapListener;

class Remote : public QObject, public MainWindowDelegate, public BonjourDelegate, public RiftDelegate  {
    Q_OBJECT
public:
    
    Remote();
    virtual ~Remote(void);
    
    int exec(void);
    
    //! Called when a new Bonjour service is detected
    virtual void serviceBrowsed(bool error,
                                Bonjour::BrowsingType browsingType=Bonjour::BrowsingAdd,
                                std::string const& name="",
                                std::string const& type="",
                                std::string const& domain="");
    
    //! Called when a Bonjour service has been resolved
    virtual void serviceResolved(bool error,
                                 std::string const& hostname="",
                                 std::string const& ip="",
                                 unsigned short port=0);
    
    // Main window delegate functions
    virtual void connect(void);
    virtual void hostEntered(std::string host);
    virtual void disconnect(void);
    virtual void viewChanged(int index);
    virtual void carambarAction(void);
    virtual void talk(std::string message);
    virtual void autoDriving(void);
    virtual void steeringWheelAction(void);
    virtual void funAction(void);
    virtual void frontward(void);
    virtual void backward(void);
    virtual void stop(void);
    virtual void left(void);
    virtual void right(void);
    virtual void front(void);
    virtual void rift(void);

    // Rift delegate functions
    virtual void riftOrientationUpdate(OVR::Vector3f orientation);

    typedef QList<QPair<QString, QString> > ParamsList;

    void sendRequest(std::string request,
                     ParamsList const & params=ParamsList());

    public slots:
    void networkRequestFinished(QNetworkReply* reply);
    
    private slots:
    void streamDataAvailable();
    void _flushPendingRequest();

private:
    MainWindow              _mainWindow;
    Bonjour                 _bonjour;
    bool                    _naoAvailable;
    QUrl                    _naoUrl;
    QNetworkAccessManager	_networkManager;
    bool                    _connected;
    QTcpSocket*             _streamSocket;
    qint64                  _streamImageSize;
    QImage*                 _streamImage;
    bool                    _streamSizeRead;
    Rift*                   _rift;
    Controller*             _leapController;
    LeapListener*           _leapListener;
    QList<QUrl>             _pendingRequest;
    QTimer*                 _flushRequestTimer;
};

#endif
