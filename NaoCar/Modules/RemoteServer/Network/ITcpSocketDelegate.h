//
//  ISocketDelegate.h
//  Babel Server
//
//  Created by Aurao on 26/11/12.
//
//

#ifndef Babel_Server_ISocketDelegate_h
#define Babel_Server_ISocketDelegate_h

#include "ATcpSocket.h"

namespace Network {

    class ITcpSocketDelegate {
    public:
        //! Socket has etablished a connection
        /*!
         Called after connect() has been called on the socket and the connection
         have successfully been etablished to the host, or an error occured.
         \param sender The socket that emited the event
         \param error The error if an error occured
         */
        virtual void    connected(ASocket* sender, ASocket::Error error) = 0;
        
        //! A read operation has finished
        /*!
         Called after read() has been called on the socket and the data have
         successfully been read to the buffer.
         \param sender The socket that emited the event
         \param error The error if an error occured
         \param bytesRead Size read if no error occured
         */
        virtual void    readFinished(ASocket* sender, ASocket::Error error,
				     size_t bytesRead) = 0;
	
        virtual void    readFinished(ASocket* sender, ASocket::Error error,
				     std::string const& buffer) = 0;
        //! A write operation has finished
        /*!
         Called after write() has been called on the socket and the data have
         successfully been read to the buffer.
         \param sender The socket that emited the event
         \param error The error if an error occured
         \param bytesRead Size written if no error occured
         */
        virtual void    writeFinished(ASocket* sender, ASocket::Error error,
                              size_t bytesWritten) = 0;
    };
    
}

#endif
