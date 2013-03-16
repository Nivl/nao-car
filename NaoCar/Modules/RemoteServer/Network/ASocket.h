//
//  ASocket.h
//  Babel Server
//
//  Created by Aurao on 26/11/12.
//
//

#ifndef __Babel_Server__ASocket__
# define __Babel_Server__ASocket__


# include "Types.h"
# include <string>

namespace Network {

    class ITcpSocketDelegate;

    //! Abstraction of a socket
    /*!
     All socket operation are asynchronous, to respond to socket events, the user
     should provide a delegate object that implements the ISocketDelegate interface.
     */

    class ASocket {
    public:

        //! Socket type (usually TCP or UDP)
        enum Type {
            TcpSocket,
            UdpSocket
        };

        //! Socket errors
        enum Error {
            NoError = 0,
            HostNotFound,
            HostUnreachable,
            WriteError,
            ReadError
        };

        //! Constructs a socket
        /*!
         Constructs a socket of the specified type
         \param type Type of the socket
         */
        ASocket(ASocket::Type type);

        //! Destroys the socket
        virtual ~ASocket();

        //! Returns the type of the socket
        ASocket::Type       getType() const;
        //! Set the type of the socket
        void                setType(ASocket::Type type);

        //! Asynchronously connect to the given host with the given port
        /*!
         Connect starts by resolving the given host, and then try to connect
         to it through the given port. If an error occurs, the connected() function
         of the delegate is called with the correct error.
         When done, the connected() function of the delegate is called.
         \param host A string representing the host to connect to
         \param port The port to which to connect to
         */
        virtual void        connect(std::string host, uint16_t port) = 0;

        //! Close the socket connexion
        virtual void        close() = 0;


        //! Asynchronously read a given amount of data
        /*!
         Reads the given size and call the readFinished() function of the delegate.
         Data is stored in the given buffer which must be allocated by the user.
         If an error occurs, the readFinished() function of the delegate is called
         with the correct error and the amount of data read before the error
         happened.
         If all is true, readFinished() is not called until all bytes have been
         read.
         If all is false, readFinished() is called directly when data is available
         and the number of read bytes is passed.
         \param buffer A buffer in which at least size bytes could be written
         \param size The size to read
         \param all Read some of all size
         */
        virtual void        read(void* buffer, uint32_t size, bool all = true) = 0;


        //! Asynchronously write a given amount of data
        /*!
         Write the data from the buffer to the socket.
         When the write operation, writeFinished() function of the delegate is
         called. If an error occured, the number of byte written before the error
         is written.
         */
        virtual void        write(const void* buffer, uint32_t size) = 0;

        virtual void        write(std::string string);

        //! Returns the IP the socket is connected to
        /*!
        Returns an empty string if an error occured.
        */
        virtual std::string getRemoteIp() const = 0;

        //! Returns the IP the socket is connected to
        /*!
        Returns an empty string if an error occured.
        */
        virtual uint32_t getBinaryRemoteIp() const = 0;

        //! Returns the port the socket is connected to
        /*!
         Returns 0 if an error occured.
         */
        virtual uint16_t    getRemotePort() const = 0;

        //! Returns true if the socket is connected
        virtual bool        isConnected() const = 0;

    private:
        ASocket::Type       _type;
    };

}

#endif /* defined(__Babel_Server__ASocket__) */
