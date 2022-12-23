#include "Handler.h"

#include "../exceptions/HandlerAlreadyHooked.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

Handler::Handler() : fd{-1}, epollFd{-1}, hooked{false} {

}

int Handler::getFD(){
    return fd;
}

void Handler::hookEpoll(int epollFd){
    if(hooked) throw HandlerAlreadyHooked();
    this->epollFd = epollFd;
    hooked = true;
}
Handler::~Handler(){
    if(hooked)
        epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, nullptr);
    
    if(fd != -1){
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }

}
