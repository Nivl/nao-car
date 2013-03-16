//
//  ATcpSocket.h
//  Babel Server
//
//  Created by Aurao on 26/11/12.
//
//

#ifndef __Babel_Server__ATcpSocket__
#define __Babel_Server__ATcpSocket__

#include "ASocket.h"

namespace Network {

    //! Abstraction of a TCP socket

    class ATcpSocket : public ASocket {
    public:
        
        //! Constructs a TCP socket
        ATcpSocket();
        
        //! Destroys the socket
        virtual ~ATcpSocket();
        
        //! Returns the delegate
        ITcpSocketDelegate* getDelegate() const;

	virtual void readUntil(std::string const& delim) = 0;

        //! Set the delegate
        void setDelegate(ITcpSocketDelegate* delegate);
        
    protected:
        virtual void _connected(ASocket::Error error);
        virtual void _readFinished(ASocket::Error error, size_t bytesRead);
        virtual void _writeFinished(ASocket::Error error, size_t bytesWritten);
        
    private:
        ITcpSocketDelegate*    _delegate;
    };

}

#endif /* defined(__Babel_Server__ATcpSocket__) */
