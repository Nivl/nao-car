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

# include <map>

# include "MainWindow.hpp"
# include "MainWindowDelegate.hpp"
# include "Bonjour.hpp"
# include "BonjourDelegate.hpp"

# define NAOCAR_BONJOUR_SERVICE_NAME "nao-car"

class Remote : public QObject, public MainWindowDelegate, public BonjourDelegate  {
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
    void connect(void);
    void hostEntered(std::string host);
    void disconnect(void);
    void viewChanged(int index);
    void carambarAction(void);
    void talk(std::string message);
    void autoDriving(void);
    void steeringWheelAction(void);
    void funAction(void);
    void steeringWheelDirectionChanged(MainWindow::Direction direction);
    void moveChanged(MainWindow::Move move);
    
    void sendRequest(std::string request,
                     std::string paramName="",
                     std::string paramValue="");
    
    public slots:
    void networkRequestFinished(QNetworkReply* reply);
    
    private slots:
    void streamDataAvailable();
    
private:
    MainWindow		_mainWindow;
    Bonjour		_bonjour;
    bool			_naoAvailable;
    QUrl			_naoUrl;
    QNetworkAccessManager	_networkManager;
    bool			_connected;
    QTcpSocket		*_streamSocket;
    qint64		_streamImageSize;
    QImage		*_streamImage;
    bool			_streamSizeRead;
};

#endif
