#include <cstring>
#include <fstream>
#include <iostream>
#include <exception>
#include <csignal>
#include "exceptions/IncorrectArguments.h"
#include "exceptions/EpollFailed.h"
#include "tools/readPort.h"
#include "Server/Server.h"
#include "Handler/Handler.h"
#include <string>
#include <sys/epoll.h>
#include <vector>
#include <chrono>
using namespace std::chrono;

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
        if(argc < 2 || argc>3) throw IncorrectArguments();
        long port = readPort(argv[1]);
        
        if (argc == 3 && strlen(argv[2])>0){
            std::ifstream file;
            file.exceptions ( std::ifstream::badbit );
            try {
                file.open (argv[2]);
                server = new Server(port,true,argv[2]);
            }catch (const std::ifstream::failure& e) {
                std::cout << "Exception opening config";
                server = new Server(port);
            }
            file.close();
        }else{
            server = new Server(port);
        }
        std::cout << "Server listens on PORT " << port << std::endl;

        signal(SIGINT, closeServer);
        signal(SIGPIPE, SIG_IGN);

        int fd = epoll_create1(0);
        server->hookEpoll(fd);

        epoll_event ee;

        auto last = duration_cast< milliseconds >(
            system_clock::now().time_since_epoch()
        ).count();
        int n;
        long long timeout;
        while(true){
            timeout = 1000 - duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
            ).count() + last;
            if(timeout < 0){ // minela ponad 1s od poprzedniego razu
                server->geoguessrGame.gameLoop();
                last = duration_cast< milliseconds >(
                    system_clock::now().time_since_epoch()
                ).count();
                timeout = 1000;
            }// else minelo mniej niz sekunda czekamy tyle, zeby co ok. 1s wykonywac

            n = epoll_wait(fd, &ee, 1, timeout); // mamy timeout na epollu zamiast drugiego wątku
            // gameLoop zostanie wywołany co około sekundę (niedokładnie, ale poprzednio przez mutexy też tak było)
            if(n == -1 && errno != EINTR){
                throw EpollFailed();
            }else if(n == 0){ // minal timeout
                server->geoguessrGame.gameLoop();
                last = duration_cast< milliseconds >(
                    system_clock::now().time_since_epoch()
                ).count();
            }else{
                ((Handler*)ee.data.ptr)->handleEvent(ee.events);
                // wykonanie tego zajmuje troche czasu, ale na pewno nie zblokuje game loopa na długo
            }
        }
    }catch(std::exception& e){
        std::cerr << "Exception" << std::endl << '\t' << e.what() << std::endl;
        closeServer(0);
    }
}
