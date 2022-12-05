#include <iostream>
#include <exception>
#include <signal.h>
#include "exceptions/IncorrectArguments.h"
#include "exceptions/EpollFailed.h"
#include "tools/readPort.h"
#include "Server/Server.h"
#include "Handler/Handler.h"
#include <sys/epoll.h>

// TODO: move to config file
namespace Settings{
    int EPOLL_TIMEOUT = -1; // -1 == infinity
    int EPOLL_MAX_EVENTS = 1; 
}

namespace ServerGlobal{
    Server server;
}


void close(int _ = -1){
    ServerGlobal::server.close();
    std::cout << "Server stopped" << std::endl;
    exit(0);
}

int main(int argc, char const *argv[]){
    using namespace ServerGlobal;

    try{
        if(argc != 2) throw IncorrectArguments();
        long port = readPort(argv[1]);

        server.start(port);
        std::cout << "Server listens on PORT " << port << std::endl;

        signal(SIGINT, close);
        signal(SIGPIPE, SIG_IGN);

        int fd = epoll_create1(0);
    
        server.hookEpoll(fd);

        epoll_event ee;

        while(true){
            if(-1 == epoll_wait(fd, &ee, Settings::EPOLL_MAX_EVENTS, Settings::EPOLL_TIMEOUT) && errno!=EINTR)
                throw EpollFailed();

            ((Handler*)ee.data.ptr)->handleEvent(ee.events);
        }
    }catch(std::exception& e){
        std::cerr << e.what() << std::endl;
        close();
        return -1;
    }
}
