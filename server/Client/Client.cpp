#include "Client.h"
#include <iterator>
#include <sstream>
#include <sys/epoll.h>
#include <regex>
#include <string>
#include <chrono>
#include "../Packet/Packet.h"
#include "../exceptions/HandlerNotHooked.h"
#include "../Server/Server.h"
#include "../Game/Team.h"
using namespace std::chrono;

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
                        for(int i=0;this->server->geoguessrGame.players_queue.size()>i;i++){
                            if(this->server->geoguessrGame.players_queue.at(i)->getFD()==fd){
                                return;
                            }
                            if(this->server->geoguessrGame.players_queue.at(i)->name==packet.content){
                                Packet packetReturn("error", "name_exists");
                                WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                                        }, [this](){
                                        }, packetReturn);
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
                                Packet packetReturn("error", "name_exists");
                                WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                                        }, [this](){
                                        }, packetReturn);
                                addWriter(writer);
                                this->onRemove(true);
                                return;
                            }
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
                                    }, packetReturn);
                            addWriter(writer);
                            this->onRemove(true);
                            return;
                        }
                    }
                    this->setName(packet.content);
                    this->server->geoguessrGame.addPlayer(this);
                }
            } else if (packet.action=="vote") {

                for(auto i=this->server->geoguessrGame.players.begin();i<this->server->geoguessrGame.players.end();++i){
                    if((*i)->getName()==packet.content){
                        this->server->geoguessrGame.votes.push_back((*i)->getFD());
                        Packet packetReturn("player_vote", "ok");
                        WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                                }, [this](){
                                }, packetReturn);
                        addWriter(writer);
                        return;
                    }    
                }
                Packet packetReturn("player_vote", "not exists");
                WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        }, [this](){
                        }, packetReturn);
                addWriter(writer);
            }else if (packet.action=="team") {
                std::string result_content="ok";
                if(packet.content=="Green") {
                    this->server->geoguessrGame.teams.at("Green").add_player(this);
                    this->team_affilation="Green";
                }else if (packet.content=="Pink") {
                    this->server->geoguessrGame.teams.at("Pink").add_player(this);
                    this->team_affilation="Pink";
                }else if (packet.content=="Yellow") {
                    this->server->geoguessrGame.teams.at("Yellow").add_player(this);
                    this->team_affilation="Yellow";
                }else if (packet.content=="Orange") {
                    this->server->geoguessrGame.teams.at("Orange").add_player(this);
                    this->team_affilation="Orange";
                }else{
                    result_content="error";
                }
                Packet packetReturn("player_vote", result_content);
                WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        }, [this](){
                        }, packetReturn);
                addWriter(writer);
                return;
            }else if(packet.action == "host_place" && fd == this->server->geoguessrGame.host->fd){
                std::string result_content="ok";

                if(this->server->geoguessrGame.getCurrentState() == GameState::ADMIN_PANEL){
                    size_t pos = 0;
                    std::vector<std::string> tokens;
                    std::string content = packet.content;
                    while ((pos = content.find(" ")) != std::string::npos) {
                        tokens.push_back(content.substr(0, pos));
                        content.erase(0, pos + 1);
                    }
                    if(tokens.size() != 3){
                        result_content="error";
                    }else{
                        this->server->geoguessrGame.goal=Point(std::stod( tokens[2]),std::stod(tokens[1]));
                        this->server->geoguessrGame.round++;
                        auto ms = duration_cast< milliseconds >(
                                system_clock::now().time_since_epoch()
                                ).count() + 120000; // TODO: CONFIG
                        Packet packet("place",  std::to_string(this->server->geoguessrGame.round) + " " + tokens[0] + " " + std::to_string(ms));
                        for(int i=0;this->server->geoguessrGame.players.size()>i;i++){
                            if(this->server->geoguessrGame.players.at(i)->getFD()!=fd){ // skip host
                                WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                                        }, [this](){
                                        }, packet);
                                this->server->geoguessrGame.players.at(i)->addWriter(writer);
                            }
                        }
                    }
                }else result_content="not_now";

                Packet packetReturn("host_place", result_content);
                WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        }, [this](){
                        }, packetReturn);
                addWriter(writer);
                return;
            }else if(this->server->geoguessrGame.getCurrentState()==3 && packet.action == "set_place"){
                std::string temp=packet.content;
                std::replace(temp.begin(), temp.end(), ',', ' ');

                std::stringstream ss(packet.content);
                double temp2;
                ss>>temp2;
                double x = temp2;  
                ss>>temp2;
                double y=temp2;
                std::cout<<x<<" "<<y<<std::endl;
                this->server->geoguessrGame.teams.at(this->team_affilation).members_points.insert_or_assign(fd,Point(x,y));

                Packet packetReturn("user_set_place",  std::to_string(x)+","+std::to_string(y));
                for(int i=0;this->server->geoguessrGame.teams.at(this->team_affilation).members.size()>i;i++){
                    if(this->server->geoguessrGame.teams.at(this->team_affilation).members.at(i)->getFD()!=fd){ // skip host
                        WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                                }, [this](){
                                }, packetReturn);
                        this->server->geoguessrGame.teams.at(this->team_affilation).members.at(i)->addWriter(writer);
                    }
                }


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
std::string Client::getName(){
    return this->name;
}
std::string Client::getTeamName(){
    return this->team_affilation;
}
