//
//  BoostTcpSocket.h
//  Babel Server
//
//  Created by Aurao on 26/11/12.
//
//

#ifndef __Babel_Server__BoostTcpSocket__
#define __Babel_Server__BoostTcpSocket__

#include "ATcpSocket.h"

#include <boost/asio.hpp>

namespace Network {

    class BoostTcpSocket : public ATcpSocket {
    public:
        BoostTcpSocket(boost::asio::io_service* service);
        virtual ~BoostTcpSocket();

        virtual void connect(std::string host, uint16_t port);
        virtual void close();
        virtual void read(void* buffer, uint32_t size, bool all);
        virtual void readUntil(std::string const& delim);
        virtual void write(const void* buffer, uint32_t size);

        virtual std::string getRemoteIp() const;

        virtual uint32_t getBinaryRemoteIp() const;

        virtual uint16_t    getRemotePort() const;

        virtual bool        isConnected() const;

        boost::asio::ip::tcp::socket*   getBoostSocket() const;

    private:

        void _resolveHandler(boost::shared_ptr<boost::asio::ip::tcp::resolver> resolver,
                             const boost::system::error_code& ec,
                             boost::asio::ip::tcp::resolver::iterator endpoints);

        void _connectHandler(const boost::system::error_code& ec);
        void _readHandler(const boost::system::error_code& ec,
                          std::size_t bytesTransfered);
        void _readUntilHandler(const boost::system::error_code& ec,
			       std::size_t bytesTransfered);
        void _writeHandler(const boost::system::error_code& ec,
                           std::size_t bytesTransfered);

        boost::asio::ip::tcp::socket*   _socket;
        boost::asio::io_service*        _ioService;
	boost::asio::streambuf		_readUntilBuffer;
    };

}

#endif /* defined(__Babel_Server__BoostTcpSocket__) */
