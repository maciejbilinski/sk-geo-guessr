#include "Client.h"
#include <sys/epoll.h>
#include <regex>
#include "../Packet/Packet.h"
#include "../exceptions/HandlerNotHooked.h"
#include "../Server/Server.h"
#include <chrono>
#include <string> 
using namespace std::chrono;

Client::Client(int fd, Server* server): 
    server{server}, 
    readBuffer{
        fd, 
        [fd, this](const Buffer& buff){
            std::cout << "Data: '" << std::regex_replace(buff.getData(), std::regex("\n"), "\\n") << '\'' << std::endl;
            //this->onRemove(true);
            if(buff.getData() == "New player: m\n"){
                {
                    Packet packet(GameState::ERROR, "error", "add player");
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done ap" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
                {
                    Packet packet(GameState::ERROR, "error", "new player: jacek");
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done npj" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
                {
                    Packet packet(GameState::ERROR, "error", "new player: test");
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done npt" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
                {
                    Packet packet(GameState::ERROR, "error", "del player: test");
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done dpt" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
            }else if(buff.getData() == "New player: broken\n"){
                {
                    Packet packet(GameState::ERROR, "error", "waiting for game");
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done wfg" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
                
            }else if(buff.getData() == "My vote: m;My team: Green\n"){
                {
                    Packet packet(GameState::ERROR, "error", "ok vote");
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done ov" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
                {
                    Packet packet(GameState::ERROR, "error", "ranking: jacek;m;inny");
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done rjmi" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
                sleep(1);
                {
                    Packet packet(GameState::ERROR, "error", "ranking: jacek;m");
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done rjm" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
                sleep(1);
                {
                    Packet packet(GameState::ERROR, "error", "game start: host");
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done gsh" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
            }else if(buff.getData() == "My vote: jacek;My team: Green\n"){
                {
                    Packet packet(GameState::ERROR, "error", "ok vote");
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done ov" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
                {
                    Packet packet(GameState::ERROR, "error", "ranking: jacek;m;inny");
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done rjmi" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
                sleep(1);
                {
                    Packet packet(GameState::ERROR, "error", "ranking: jacek;m");
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done rjm" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
                sleep(1);
                {
                    auto ms = duration_cast< milliseconds >(
                        system_clock::now().time_since_epoch()
                    ).count() + 3000;
                    Packet packet(GameState::ERROR, "error", "game start: game;https://i.ytimg.com/vi/abUhPANjIsM/maxresdefault.jpg;" + std::to_string(ms));
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done gsg" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
                sleep(1);
                {
                    Packet packet(GameState::ERROR, "error", "new answer: random;52.40371;16.9495");
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done nar" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
                sleep(2);
                {
                    auto ms = duration_cast< milliseconds >(
                        system_clock::now().time_since_epoch()
                    ).count() + 120000;
                    Packet packet(GameState::ERROR, "error", "new round: 2;https://www.shutterstock.com/image-photo/golf-player-putting-green-hitting-260nw-155555189.jpg;" + std::to_string(ms));
                    WriteBuffer* writer = new WriteBuffer(fd, [this](const Buffer& buffer){
                        std::cout << "Error" << std::endl;
                    }, [this](){
                        std::cout << "Done nr2" << std::endl;
                    }, packet);
                    addWriter(writer);
                }
            }else{
                this->onRemove(true);
            }
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