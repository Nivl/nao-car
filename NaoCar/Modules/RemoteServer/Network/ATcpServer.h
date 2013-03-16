//
//  ATcpServer.h
//  Babel Server
//
//  Created by Aurao on 30/11/12.
//
//

#ifndef __Babel_Server__ATcpServer__
# define __Babel_Server__ATcpServer__

# include "Types.h"
# include <string>

# include "ATcpSocket.h"

namespace Network {
    
    class ITcpServerDelegate;
    
    //! Base class for a TCP Server
    
    class ATcpServer {
    public:
        
        //! Default maximum number of pending connections
        static const int maxPendingConnectionsDefault = 30;
        
        ATcpServer();
        virtual ~ATcpServer();
        
        //! Tells the server to listen for incomming connections
        /*!
         Connections are listened on the given address
         and port.
         \param port If zero, the port is choosen automatically
         \param address If the address is an empty string, the server will accept connections from any addresses
         */
        virtual bool listen(uint16_t port=0, std::string address="") = 0;
        
        // Returns the address on wich the server is listening
        virtual std::string getAddress() const = 0;
        
        // Returns the address on wich the server is listening
        virtual uint16_t getPort() const = 0;
        
        //! Returns the maximum number of pending connections
        int     getMaxPendingConnections() const;
        void    setMaxPendingConnections(int maxPendingConnections);
        
        ITcpServerDelegate* getDelegate() const;
        void                setDelegate(ITcpServerDelegate* delegate);
        
    protected:
        void    _newConnection(ATcpSocket* socket);
        
    private:
        ITcpServerDelegate* _delegate;
        int                 _maxPendingConnections;
    };
    
}

#endif /* defined(__Babel_Server__ATcpServer__) */
