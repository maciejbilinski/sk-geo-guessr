#include <iostream>
#include <exception>
#include <csignal>
#include "exceptions/IncorrectArguments.h"
#include "exceptions/EpollFailed.h"
#include "tools/readPort.h"
#include "Server/Server.h"
#include "Handler/Handler.h"
#include <sys/epoll.h>
#include <vector>

// TODO: move to config file
namespace Settings{
    int EPOLL_TIMEOUT = -1; // -1 == infinity
}

namespace ServerGlobal{
    Server* server = nullptr;
}


void closeServer(int signum = -1){
    delete ServerGlobal::server;
    std::cout << "Server stopped" << std::endl;
    exit(0);
}

int main(int argc, char const *argv[]){
    using namespace ServerGlobal;

    try{
        if(argc != 2) throw IncorrectArguments();
        long port = readPort(argv[1]);

        server = new Server(port);
        std::cout << "Server listens on PORT " << port << std::endl;

        signal(SIGINT, closeServer);
        signal(SIGPIPE, SIG_IGN);

        int fd = epoll_create1(0);
        server->hookEpoll(fd);

        epoll_event ee;

        while(true){
            if(-1 == epoll_wait(fd, &ee, 1, Settings::EPOLL_TIMEOUT) && errno!=EINTR)
                throw EpollFailed();
            ((Handler*)ee.data.ptr)->handleEvent(ee.events);
        }
    }catch(std::exception& e){
        std::cerr << e.what() << std::endl;
        closeServer(0);
    }
}
