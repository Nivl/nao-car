//
//  BoostTcpServer.h
//  Babel Server
//
//  Created by Aurao on 03/12/12.
//
//

#ifndef __Babel_Server__BoostTcpServer__
# define __Babel_Server__BoostTcpServer__

# include <boost/asio.hpp>

# include "ATcpServer.h"
# include "BoostTcpSocket.h"

namespace Network {
    
    //! Boost implementation of a Tcp Server
    
    class BoostTcpServer : public ATcpServer {
    public:

        BoostTcpServer(boost::asio::io_service* service);
        virtual ~BoostTcpServer();
        
        virtual bool listen(uint16_t port, std::string address);
        virtual std::string getAddress() const;
        virtual uint16_t getPort() const;
        
    private:
        
        void _startAccept();
        void _acceptHandler(const boost::system::error_code& error,
                            BoostTcpSocket* socket);
        
        boost::asio::ip::tcp::acceptor* _acceptor;
	boost::asio::io_service*	_ioService;
    };
    
}

#endif /* defined(__Babel_Server__BoostTcpServer__) */
