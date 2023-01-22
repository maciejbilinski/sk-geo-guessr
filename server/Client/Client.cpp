#include "Client.h"
#include <iterator>
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
        [fd,this](const Packet& packet){ // tutaj musi być cała logika odbioru pakietów i jakaś komunikacja z obiektem server
            packet.print();
            if (packet.action=="player_intro") {
                    for(int i=0;this->server->geoguessrGame.players_queue.size()>i;i++){
                       if(this->server->geoguessrGame.players_queue.at(i)->getFD()==fd){
                            return;
                       }
                        if(this->server->geoguessrGame.players_queue.at(i)->name==packet.content){
                            Packet packetReturn("error", "Name exists");
                            WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                            }, [this](){
                            }, packet);
                            addWriter(writer);
                            this->onRemove(true);
                            return;
                       }
                    }
                    for(int i=0;this->server->geoguessrGame.players.size()>i;i++){
                       if(this->server->geoguessrGame.players.at(i)->getFD()==fd){
                            return;
                       }
                       if(this->server->geoguessrGame.players.at(i)->name==packet.content){
                            Packet packetReturn("error", "Name exists");
                            WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                            }, [this](){
                            }, packet);
                            addWriter(writer);
                            this->onRemove(true);
                            return;
                       }
                    }
                    Packet packetReturn("player_intro", packet.content);
                    this->setName(packet.content);
                    this->server->geoguessrGame.players_queue.push_back(this);
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done npt" << std::endl;
                    }, packet);
                    addWriter(writer);
            }else{
                std::cout << "Invalid data: '" << std::regex_replace(packet.toString(), std::regex("\n"), "\\n") << '\'' << std::endl;
                this->onRemove(true);
            }
        }
    }, Handler()
{
    this->fd = fd;
    this->name ="";
    server->geoguessrGame.time_counter=93;
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
        Packet packet( "error", "disconnected");
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
    this->server->geoguessrGame.removePlayer(this->fd);
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

void Client::setName(std::string name){
    this->name=name;
}
