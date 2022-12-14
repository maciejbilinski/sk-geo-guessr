#include "Server.h"

#include "../exceptions/InaccessibleServer.h"
#include "../exceptions/EpollFailed.h"
#include "../exceptions/NotAcceptedClient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sys/epoll.h>
#include "../exceptions/HandlerNotHooked.h"

Server::Server(bool log) : log{log}, isStarted{false}, Handler() {
}

Server::~Server(){
    for(Client* client : clients)
        delete client;
    this->close();
    Handler::~Handler();
}

bool Server::start(long port){
    if(!this->isStarted){
        this->fd = socket(AF_INET, SOCK_STREAM, 0);
        if(this->fd == -1) throw InaccessibleServer();

        int res;
        const int one = 1;
        res = setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
        if(res) throw InaccessibleServer();

        sockaddr_in serverAddr;
        serverAddr.sin_family=AF_INET;
        serverAddr.sin_port=htons((short)port);
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        res = bind(this->fd, (sockaddr*) &serverAddr, sizeof(serverAddr));
        if(res) throw InaccessibleServer();
        
        res = listen(this->fd, 1);
        if(res) throw InaccessibleServer();
        this->isStarted = true;
        return true;
    }
    return false;
}

bool Server::close(){
    if(isStarted){
        ::close(this->fd);
        this->fd = -1;
        isStarted = false;
        return true;
    }
    return false;
}

void Server::hookEpoll(int epollFd){
    Handler::hookEpoll(epollFd);
    epoll_event ee {EPOLLIN, {.ptr=this}};
    epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ee);
}

void Server::handleEvent(unsigned int events){
    if(hooked){
        if(events & EPOLLIN){
            sockaddr_in clientAddr{};
            socklen_t clientAddrSize = sizeof(clientAddr);
            
            auto clientFd = accept(fd, (sockaddr*) &clientAddr, &clientAddrSize);
            if(clientFd == -1) throw NotAcceptedClient();
            
            if(this->log){
                std::cout << "New client from: " << inet_ntoa(clientAddr.sin_addr) << ':' << ntohs(clientAddr.sin_port) << std::endl;
            }
            Client* client = new Client(clientFd, [this](Client* cl){
                char packet[] = "state:-1;action:error;content:disconnected;";
                write(cl->fd, packet, sizeof(packet));
                this->clients.erase(cl);
                delete cl;
                if(log){
                    std::cout << "Disconnected client" << std::endl;
                }
            });
            client->hookEpoll(this->epollFd);
            clients.insert(client);
        }
        if(events & ~EPOLLIN){
            throw EpollFailed();
        }
    }else throw HandlerNotHooked();
}
