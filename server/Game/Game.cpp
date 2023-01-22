#include "Game.h"
#include <cstdio>
#include <map>
#include <unistd.h>
#include <utility>
#include <vector>
#include <sys/mman.h>
#include <thread>
#include <string>
#include <chrono>
#include <sstream>
#include <bits/stdc++.h>
using namespace std::chrono;

bool cmp(std::pair<std::string, double>& a,std::pair<std::string, double>& b){
    return a.second > b.second;
}
void sort_map(std::map<std::string, double>& M)
{
    std::vector<std::pair<std::string,double>>A;
    for (auto& it : M) {A.push_back(it);}
    sort(A.begin(), A.end(), cmp);
}

void Game::gameLoop(){

    while (true) {
        sleep(1);
        allMutex.lock();

        printf("Players in green: %d \nTime remain: %d \nGame state: %d \nNumber of players in queue: %lu \nNumber of players in game: %lu \n\n",
            this->teams.at("Green").members.size(), 
            this->time_counter,
            this->currentState, 
            this->players_queue.size(), 
            this->players.size()
        );

        if(this->currentState>1 && this->players.size()<1){
            this->currentState=0;
        }

        switch (this->currentState) {
            case 0:
                if (this->players_queue.size()>0) {
                    (this->time_counter)=60; //TODO: set from config
                    (this->currentState)=1; // wchodzimy w stan wyboru zdjecia, ludzie widza plansze wait 
                }
                break;
            case 1:
                if(this->players_queue.size()>0){
                    for(auto i=this->players_queue.begin();i<this->players_queue.end();i++){
                        _addPlayer(*i, true, false);
                        this->players_queue.erase(i);
                    }
                    //this->time_counter+=15; //TODO: czas
            }
            break;
            case 2:
            case 3:
            case 4:
            break;
        }
        if (this->time_counter>0){
            (this->time_counter)--;
        }else{
            int max=0;
            int best=0;
            std::map<int, int> temp =  std::map<int, int>();
            std::map<std::string,double> rank=std::map<std::string,double>();
            switch (this->currentState) {
                case 1:
                    (this->time_counter)=60; //TODO: set from config
                    (this->currentState)=2; // wchodzimy w stan wyboru zdjecia, ludzie widza plansze wait
                    for(auto vote :this->votes){
                        if (temp.find(vote)==temp.end()){
                            temp.insert(std::pair<int,int>(vote,1));
                        }else{
                            temp[vote]=temp[vote]+1;
                        }
                    }
                    
                    for(auto player:temp){
                        if(player.second>max){
                            best=player.first;
                        }
                    }
                    if(best==0){
                        (this->time_counter)=60; //TODO: set from config
                        (this->currentState)=1; // wracamy do stanu wybierania hosta
                    }else{
                        for(auto player :this->players){
                            if(player->getFD()==best){
                                this->host=player;
                                this->teams.at(player->getTeamName()).remove_player(player->getFD());
                        
                                Packet packet("host",player->getName());
                                for(auto j=this->players.begin();j<this->players.end();j++){
                                        WriteBuffer* writer = new WriteBuffer((*j)->getFD(), [this](const Buffer& buffer){
                                                }, [this](){
                                        }, packet);
                                        (*j)->addWriter(writer);
                                }
                            }
                        }
                        if(this->host==nullptr){
                            (this->time_counter)=60; //TODO: set from config
                            (this->currentState)=1; // wracamy do stanu wybierania hosta
                            for(auto j=this->players.begin();j<this->players.end();j++){
                                Packet packet("voting_failed","" );
                                WriteBuffer* writer = new WriteBuffer((*j)->getFD(), [](const Buffer& buffer){}, [](){}, packet);
                                (*j)->addWriter(writer);
                            }
                        }
                    }
                break;           
                case 2://wyrzucamy hosta za bezczynnosc i cofamy sie do vote
                    this->host->onRemove(true);
                    this->host=nullptr;
                    this->time_counter=60; //TODO: set from config
                    this->currentState=1; // wchodzimy w gre, ludzie glosuja, host widzi glosy
                    for(auto j=this->players.begin();j<this->players.end();j++){
                                Packet packet("voting_failed","" );
                                WriteBuffer* writer = new WriteBuffer((*j)->getFD(), [this](const Buffer& buffer){
                                        }, [this](){
                                        }, packet);
                                (*j)->addWriter(writer);
                    }
                    break;
                case 3:
                    (this->time_counter)=60; //TODO: set from config
                    (this->currentState)=4; // koniec rundy, widac status, host wstawia nowe zdjecie

                    for(auto team:this->teams){
                        rank.insert_or_assign(team.first, team.second.calculate_points_distance(this->goal));
                    }
                    sort_map(rank);                                 
                    best=0;
                    for(auto& team:rank){
                        best++;
                        for(auto j=this->players.begin();j<this->players.end();j++){
                            Packet packet("ranking_"+std::to_string(best),team.first);
                            WriteBuffer* writer = new WriteBuffer((*j)->getFD(), [this](const Buffer& buffer){
                                    }, [this](){
                                    }, packet);
                            (*j)->addWriter(writer);
                        }
                    }
                    
                    for(auto team:this->teams)
                    {
                        team.second.removeAfk();
                        team.second.members_points.clear();
                    }
                    break;
                case 4:
                    // czym się rozni 2 od 4?
                    (this->time_counter)=60; //TODO: set from config
                    if(this->round<10){ // przejscie albo do kolejnej gry albo do glosowania
                        (this->currentState)=2;

                    }else{
                        (this->currentState)=1;
                    }
                    break;
            }
        }
        allMutex.unlock();
    }
}

Game::Game(){
    this->host=nullptr;
    this->players = std::vector<Client*>();
    this->players_queue = std::vector<Client*>();
    this->votes = std::vector<int>();
    this->teams = std::map<std::string, Team>();
    this->currentState =0;  
    //(int*)mmap(NULL, sizeof (int) , PROT_READ | PROT_WRITE,
    //MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    this->time_counter =  0;
    //(int*)mmap(NULL, sizeof (int) , PROT_READ | PROT_WRITE,
    //MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    this->round =  0;
    //(int*)mmap(NULL, sizeof (int) , PROT_READ | PROT_WRITE,
    //MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    printf("Hi start\n");
    //(*this->time_counter)=0;
    //(*this->currentState)=0;
    //(*this->round)=0;


}

void Game::setup(){
    allMutex.lock();
    teams.insert({"Green", Team("Green")}); 
    teams.insert({"Yellow", Team("Yellow")}); 
    teams.insert({"Orange", Team("Orange")}); 
    teams.insert({"Pink", Team("Pink")}); 
    allMutex.unlock();
    std::thread timer([this](){gameLoop();});
    timer.detach();

    printf("Hi detach\n");

}

void Game::handlePacket(Packet &packet){}

void Game::removePlayer(int fd){
    this->allMutex.lock();
    std::string name="";


    bool found = false;

    for(auto i=this->players_queue.begin();i!=this->players_queue.end();i++){
        if((*i)->getFD()==fd){
           this->players_queue.erase(i);
           name = (*i)->getName();
           found = true;
           break;
        }
    }
    if(!found){
        for(auto i=this->players.begin();i!=this->players.end();i++){
            if((*i)->getFD()==fd){
                this->players.erase(i);
                name = (*i)->getName();
                found = true;
                break;
            }
        }
    }
    if(found){
      Packet packet("player_disconnected", name);
      for(auto j=this->players.begin();j<this->players.end();j++){
        WriteBuffer* writer = new WriteBuffer((*j)->getFD(), [](const Buffer& buffer){}, [](){}, packet);
        (*j)->addWriter(writer);
      }
  }
  this->allMutex.unlock();
}

void Game::newPlace(){
    this->allMutex.lock();

    (this->currentState) = 3;
    (this->time_counter)=60;

    this->allMutex.unlock();
}

void Game::_addPlayer(Client* cl, bool inQueue, bool lock){
    if(lock){
        this->allMutex.lock();
    }

    if(this->currentState != 1){
        Packet packetReturn("player_intro", "game_started");
        WriteBuffer* writer = new WriteBuffer(cl->getFD(), [](const Buffer& buffer){
            std::cout << "Error" << std::endl;
        }, [](){
            std::cout << "Sent game_started" << std::endl;
        }, packetReturn);
        cl->addWriter(writer);
        if(!inQueue){
            this->players_queue.push_back(cl);
        }
    }else{
        if(this->currentState == 0){
            (this->time_counter)=60; //TODO: set from config
            (this->currentState)=1; // wchodzimy w stan wyboru hosta
        }

        bool found = false;
        if(!inQueue){
            for(int i=0;players_queue.size()>i;i++){
                if(players_queue.at(i)->getFD()==cl->getFD()){
                    break;
                }
                if(players_queue.at(i)->getName()==cl->getName()){
                    Packet packetReturn("error", "name_exists");
                    WriteBuffer* writer = new WriteBuffer(cl->getFD(), [](const Buffer& buffer){}, [](){}, packetReturn);
                    cl->addWriter(writer);
                    cl->onRemove(true);
                    found = true;
                    break;
                }
            }
            if(!found){
                for(int i=0;players.size()>i;i++){
                    if(players.at(i)->getFD()==cl->getFD()){
                        break;
                    }
                    if(players.at(i)->getName() == cl->getName()){
                        Packet packetReturn("error", "name_exists");
                        WriteBuffer* writer = new WriteBuffer(cl->getFD(), [](const Buffer& buffer){}, [](){}, packetReturn);
                        cl->addWriter(writer);
                        cl->onRemove(true);
                        break;
                    }
                }
            }
        }

        if(!found){
            Packet packetReturn("player_intro", "ok");
            WriteBuffer* writer = new WriteBuffer(cl->getFD(), [](const Buffer& buffer){
                std::cout << "Error" << std::endl;
            }, [](){
                std::cout << "Sent player_intro ok" << std::endl;
            }, packetReturn);
            cl->addWriter(writer);
            if(this->players.size()>0){
                for(auto j=this->players.begin();j<this->players.end();j++){
                    Packet packet("new_player", cl->getName());
                    WriteBuffer* writer = new WriteBuffer((*j)->getFD(), [](const Buffer& buffer){}, [](){}, packet);
                    (*j)->addWriter(writer);

                    Packet packet2("new_player", (*j)->getName());
                    WriteBuffer* writer2 = new WriteBuffer(cl->getFD(), [](const Buffer& buffer){}, [](){}, packet2);
                    cl->addWriter(writer2);
                }
            }
            this->players.push_back(cl);
        }
    }

    if(lock){
        this->allMutex.unlock();
    }
}

void Game::addPlayer(Client* player){
    this->_addPlayer(player, false, true);
}

bool Game::addToTeam(Client* player, std::string color){
    this->allMutex.lock();

    bool found = false;

    for(int i=0;players.size()>i;i++){
        if(players.at(i)->getFD()==player->getFD()){
            found = true;
            break;
        }
    }

    if(found){
        if(color=="Green") {
            this->teams.at("Green").add_player(player);
        }else if (color=="Pink") {
            this->teams.at("Pink").add_player(player);
        }else if (color=="Yellow") {
            this->teams.at("Yellow").add_player(player);
        }else if (color=="Orange") {
            this->teams.at("Orange").add_player(player);
        }else{
            found = false;
        }
    }

    this->allMutex.unlock();
    return found;
}

void Game::startNewRound(Client* player, const Packet& packet){
    this->allMutex.lock();

    std::string result_content="ok";

    if(player->getFD() == this->host->getFD()){
        if(this->currentState == GameState::ADMIN_PANEL){
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
                this->round++;
                this->goal=Point(std::stod( tokens[2]),std::stod(tokens[1]));
                auto ms = duration_cast< milliseconds >(
                    system_clock::now().time_since_epoch()
                ).count() + 120000; // TODO: CONFIG
                Packet packet("place",  std::to_string(this->round) + " " + tokens[0] + " " + std::to_string(ms));
                for(int i=0;this->players.size()>i;i++){
                    if(this->players.at(i)->getFD()!=player->getFD()){ // skip host
                        WriteBuffer* writer = new WriteBuffer(this->players.at(i)->getFD(), [](const Buffer& buffer){}, [](){}, packet);
                        this->players.at(i)->addWriter(writer);
                    }
                }
            }
        }else result_content="not_now";
    }else{
        result_content = "error";
    }

    Packet packetReturn("host_place", result_content);
    WriteBuffer* writer = new WriteBuffer(player->getFD(), [](const Buffer& buffer){}, [](){}, packetReturn);
    player->addWriter(writer);
    this->allMutex.unlock();
}

void Game::vote(Client* player, const Packet& packet){
    this->allMutex.lock();
    bool found = false;
    for(auto i=this->players.begin();i<this->players.end();++i){
        if((*i)->getName()==packet.content){
            this->votes.push_back((*i)->getFD());
            Packet packetReturn("player_vote", "ok");
            WriteBuffer* writer = new WriteBuffer(player->getFD(), [](const Buffer& buffer){}, [](){}, packetReturn);
            player->addWriter(writer);
            found = true;
            break;
        }    
    }
    if(!found){
        Packet packetReturn("player_vote", "not_exists");
        WriteBuffer* writer = new WriteBuffer(player->getFD(), [](const Buffer& buffer){}, [](){}, packetReturn);
        player->addWriter(writer);
    }
    this->allMutex.unlock();
}

void Game::setPlace(Client* player, const Packet& packet){
    this->allMutex.lock();

    if(currentState==3){
        std::string temp=packet.content;
        std::replace(temp.begin(), temp.end(), ',', ' ');

        std::stringstream ss(packet.content);
        double temp2;
        ss>>temp2;
        double x = temp2;  
        ss>>temp2;
        double y=temp2;
        std::cout<<x<<" "<<y<<std::endl;
        this->teams.at(player->getTeamName()).members_points.insert_or_assign(player->getFD(),Point(x,y));

        Packet packetReturn("user_set_place",  std::to_string(x)+","+std::to_string(y));
        for(int i=0;this->teams.at(player->getTeamName()).members.size()>i;i++){
            if(this->teams.at(player->getTeamName()).members.at(i)->getFD()!=player->getFD()){ // skip player
                WriteBuffer* writer = new WriteBuffer(this->teams.at(player->getTeamName()).members.at(i)->getFD(), [](const Buffer& buffer){}, [](){}, packetReturn);
                this->teams.at(player->getTeamName()).members.at(i)->addWriter(writer);
            }
        }
    }else{
        Packet packetReturn("set_place", "not_now");
        WriteBuffer* writer = new WriteBuffer(player->getFD(), [](const Buffer& buffer){}, [](){}, packetReturn);
        player->addWriter(writer);
    }


    this->allMutex.unlock();
}