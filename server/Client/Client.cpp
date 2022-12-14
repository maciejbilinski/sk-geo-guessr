#include "Client.h"
#include <sys/epoll.h>
#include "../Packet/Packet.h"
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
        if(events & EPOLLIN ){
             Packet *packet = new Packet(); 
             if(packet->fromBuforToPacket(this->fd, &readBuffer)==-1)
             {
                 this->onRemove(this);
                return;
             }

             packet->print();
        }
        if(events & ~(EPOLLIN|EPOLLOUT)) {
            this->onRemove(this);
        }
    }else throw HandlerNotHooked();

}
