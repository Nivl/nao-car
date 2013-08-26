//
// Bonjour.cpp
// NaoCar Remote Server
//

#include "Bonjour.hpp"

#include <boost/bind.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <sstream>
#include <unistd.h>
#include <signal.h>

#include "BonjourDelegate.hpp"

Bonjour::Bonjour(boost::asio::io_service& ioService, BonjourDelegate* delegate)
    : _ioService(ioService), _delegate(delegate), _avahiPid(-1),
      _pipeStream(_ioService), _pipeBuffer() {
}

Bonjour::~Bonjour(void) {
    if (_avahiPid != -1) {
        kill(_avahiPid, SIGINT);
        std::cout << "Unregistered Bonjour service" << std::endl;
    }
}

bool Bonjour::registerService(std::string const& name,
                              std::string const& type,
                              unsigned short port) {
    // Use avahi-publish-service to register the Bonjour service
    std::stringstream portStr;
    portStr << port;

    int pipeFds[2];
    if (pipe(pipeFds) != 0)
        return false;
    int pid = fork();
    if (pid < 0)
        return false;
    // Child process, execute the command
    else if (pid == 0) {
        close(pipeFds[0]);
        dup2(pipeFds[1], 2);
        execlp(AVAHI_PUBLISH_COMMAND, AVAHI_PUBLISH_COMMAND,
               name.c_str(), type.c_str(), portStr.str().c_str(), NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }
    _avahiPid = pid;
    close(pipeFds[1]);
    _pipeStream.assign(pipeFds[0]);
    boost::asio::async_read_until(_pipeStream, _pipeBuffer, '\n',
                                  boost::bind(&Bonjour::pipeReadHandler,
                                              this, boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
    return (true);
}

void Bonjour::pipeReadHandler(const boost::system::error_code&,
                              std::size_t) {
    if (!_delegate)
        return ;
    std::istream is(&_pipeBuffer);
    std::string line;
    std::getline(is, line);
    if (boost::starts_with(line, "Established under name")) {
        std::string name = line.substr(24, line.size() - 24 - 1);
        _delegate->serviceRegistered(false, name);
    } else {
        _delegate->serviceRegistered(true);
    }
}
