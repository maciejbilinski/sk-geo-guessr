#include "Client.h"
#include <iterator>
#include <sstream>
#include <sys/epoll.h>
#include <regex>
#include "../Packet/Packet.h"
#include "../exceptions/HandlerNotHooked.h"
#include "../Server/Server.h"
#include "../Game/Team.h"

Client::Client(int fd, Server* server): 
    server{server}, 
    readBuffer{
        fd, 
        [this](const Buffer& buff){
            std::cout << "Cant parse as packet: '" << std::regex_replace(buff.getData(), std::regex("\n"), "\\n") << '\'' << std::endl;
            this->onRemove(true);
        },
        [fd,this](const Packet& packet){ // tutaj musi być cała logika odbioru pakietów i jakaś komunikacja z obiektem server
            packet.print();
            if (packet.action=="player_intro") {
                this->setName(packet.content);
                this->server->geoguessrGame.addPlayer(this);
            }else if (packet.action=="vote") {
                this->server->geoguessrGame.vote(this, packet);
            }else if (packet.action=="team") {
                std::string result_content="ok";
                if(this->server->geoguessrGame.addToTeam(this, packet.content)){
                    this->team_affilation = packet.content;
                }else{
                    result_content="error";
                }
                Packet packetReturn("player_vote", result_content);
                WriteBuffer* writer = new WriteBuffer(this->fd, [this, result_content](const Buffer& buffer){
                    std::cout << "Error during player_vote " << result_content << " packet to " << this->getName() << std::endl;
                }, [this, result_content](){
                    std::cout << "Sent during player_vote " << result_content << " packet to " << this->getName() << std::endl;
                }, packetReturn);
                addWriter(writer);
            }else if(packet.action == "host_place"){
                this->server->geoguessrGame.startNewRound(this, packet);
            }else if(packet.action == "set_place"){
                this->server->geoguessrGame.setPlace(this, packet);
            }else{
                std::cout << "Unknown packet: '" << std::regex_replace(packet.toString(), std::regex("\n"), "\\n") << '\'' << std::endl;
                this->onRemove(true);
            }
        }
    }, Handler()
{
    this->fd = fd;
    this->name ="";
}

void Client::hookEpoll(int epollFd){
    Handler::hookEpoll(epollFd);
    epoll_event ee {EPOLLIN|EPOLLRDHUP, {.ptr=this}};
    epoll_ctl(epollFd, EPOLL_CTL_ADD, this->fd, &ee);
}

void Client::handleEvent(unsigned int events){
    if(hooked){
        if(events & EPOLLOUT) {
            do {
                WriteBuffer* writer =  writers.front();
                if(writer->write()){
                    delete writer;
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

        if(events & EPOLLIN ){
            readBuffer.read();
        }

    }else throw HandlerNotHooked();
}
void Client::removeTeam(){
    this->team_affilation = "";
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
        }else{
            delete writer;
        }
    }else{
        writers.push_back(writer);
    }
}

void Client::setName(std::string name){
    this->name=name;
}
std::string Client::getName(){
    return this->name;
}
std::string Client::getTeamName(){
    return this->team_affilation;
}

Client::~Client(){
    for(auto it=writers.begin(); it!=writers.end(); ++it){
        delete (*it);
    }
    Handler::~Handler();
}