#include "Client.h"
#include <sys/epoll.h>
#include <regex>
#include "../Packet/Packet.h"
#include "../exceptions/HandlerNotHooked.h"
#include "../Server/Server.h"

Client::Client(int fd, Server* server): 
    server{server}, 
    readBuffer{
        fd, 
        [this](const Buffer& buff){
            std::cout << "Invalid data: '" << std::regex_replace(buff.getData(), std::regex("\n"), "\\n") << '\'' << std::endl;
            this->onRemove(true);
        },
        [this](const Packet& packet){ // tutaj musi być cała logika odbioru pakietów i jakaś komunikacja z obiektem server
            packet.print();
        }
    }, Handler()
{
    this->fd = fd;
    this->name = std::nullopt;
}

void Client::hookEpoll(int epollFd){
    Handler::hookEpoll(epollFd);
    epoll_event ee {EPOLLIN|EPOLLRDHUP, {.ptr=this}};
    epoll_ctl(epollFd, EPOLL_CTL_ADD, this->fd, &ee);
}

void Client::handleEvent(unsigned int events){
    if(hooked){
        if(events & EPOLLIN ){
            readBuffer.read();
        }

        if(events & EPOLLOUT) {
            do {
                WriteBuffer* writer =  writers.front();
                if(writer->write()){
                    writers.pop_front();
                    if(writers.size() == 0){
                        waitForWrite(false);
                        break;
                    }
                    continue;
                }else{
                    break;
                }
            } while(false);
        }
    }else throw HandlerNotHooked();
}

void Client::onRemove(bool send){
    if(send){
        Packet packet(GameState::ERROR, "error", "disconnected");
        WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
            std::cout << "Error during disconnecting client" << std::endl;
            this->server->onClientRemove(this);
        }, [this](){
            this->server->onClientRemove(this);
        }, packet);
        addWriter(writer);
    }else{
        this->server->onClientRemove(this);
    }
}

void Client::waitForWrite(bool epollout) {
    epoll_event ee {EPOLLIN|EPOLLRDHUP|(epollout?EPOLLOUT:0), {.ptr=this}};
    epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ee);
}

void Client::addWriter(WriteBuffer* writer){
    if(writers.size() == 0){
        if(!writer->write()){
           this->writers.push_back(writer);
           waitForWrite(true);
        }
    }else{
        writers.push_back(writer);
    }
}