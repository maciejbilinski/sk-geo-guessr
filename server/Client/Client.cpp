#include "Client.h"
#include <sys/epoll.h>
#include "../exceptions/HandlerNotHooked.h"
Client::Client(int fd, std::function<void(Client*)> onRemove): onRemove{onRemove} , Handler(){
    this->fd = fd;
}

void Client::hookEpoll(int epollFd){
    Handler::hookEpoll(epollFd);
    epoll_event ee {EPOLLIN|EPOLLRDHUP, {.ptr=this}};
    epoll_ctl(epollFd, EPOLL_CTL_ADD, this->fd, &ee);
}

void Client::handleEvent(unsigned int events){
    if(hooked){
        if(events & ~(EPOLLIN|EPOLLOUT)) {
            this->onRemove(this);
        }
    }else throw HandlerNotHooked();

}