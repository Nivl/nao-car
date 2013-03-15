//
//  ITcpServerDelegate.h
//  Babel Server
//
//  Created by Aurao on 03/12/12.
//
//

#ifndef Babel_Server_ITcpServerDelegate_h
# define Babel_Server_ITcpServerDelegate_h

# include "ATcpServer.h"
# include "ATcpSocket.h"

namespace Network {
    
    class ITcpServerDelegate {
    public:
        
        //! A new connection has been accepted
        /*!
         This function is called each time a new client has been accepted
         by the server.
         \param sender The server that emited the event
         \param socket The socket representing the new connection with the client
         */
        virtual void newConnection(ATcpServer* sender, ATcpSocket* socket) = 0;
        
    };
    
}

#endif
