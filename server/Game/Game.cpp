#include "Game.h"
#include <cstdio>
#include <map>
#include <unistd.h>
#include <utility>
#include <vector>
#include <sys/mman.h>
#include <thread>

void Game::gameLoop(){
    
    while (true) {
        sleep(1);
        printf("Players in green: %d \nTime remain: %d \nGame state: %d \nNumber of players in queue: %lu \nNumber of players in game: %lu \n\n",this->teams.at("Green").members.size(), this->time_counter,this->currentState, this->players_queue.size(), this->players.size());
        
        if(this->currentState>1 && this->players.size()<1){
            this->currentState=0;
        }

        switch (this->currentState) {
            case 0:
                if (this->players_queue.size()>0) {
                    (this->time_counter)=60; //TODO: set from config
                    (this->currentState)=1; // wchodzimy w stan wyboru zdjecia, ludzie widza plansze wait 
                    //(this->currentState) = 2; // wchodzimy w stan admin panel dla testu; TODO: zmienić to
                }
            break;
            case 1:
            if(this->players_queue.size()>0){
                for(auto i=this->players_queue.begin();i<this->players_queue.end();i++){
                    if(this->players.size()>0){
                        for(auto j=this->players.begin();j<this->players.end();j++){
                            Packet packet("new_player", (*i)->getName());
                            WriteBuffer* writer = new WriteBuffer((*j)->getFD(), [this](const Buffer& buffer){
                                    }, [this](){
                            }, packet);
                            (*j)->addWriter(writer);
                        }
                    }
                    this->players.push_back(*i);
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
            this->mutex.lock();
            if (this->time_counter>0){
                (this->time_counter)--;
            }else{
                int max=0;
                int best=0;
                std::map<int, int> temp =  std::map<int, int>();
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
                            (this->currentState)=1; // wchodzimy w stan wyboru zdjecia, ludzie widza plansze wait
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
                                (this->currentState)=1; // wchodzimy w stan wyboru zdjecia, ludzie widza plansze wait
                                for(auto j=this->players.begin();j<this->players.end();j++){
                                    Packet packet("voting_failed","" );
                                    WriteBuffer* writer = new WriteBuffer((*j)->getFD(), [this](const Buffer& buffer){
                                            }, [this](){
                                    }, packet);
                                    (*j)->addWriter(writer);
                                }
                            
                        }
                        }
                    break;
                    case 2:
                        (this->time_counter)=60; //TODO: set from config
                        (this->currentState)=3; // wchodzimy w gre, ludzie glosuja, host widzi glosy
                    break;
                    case 3:
                        (this->time_counter)=60; //TODO: set from config
                        (this->currentState)=4; // koniec rundy, widac status, host wstawia nowe zdjecie
                    break;
                    case 4:
                        (this->time_counter)=60; //TODO: set from config
                        if(this->round<10){ // przejscie albo do kolejnej gry albo do glosowania
                            (this->currentState)=3;
                        }else{
                            (this->currentState)=1;
                        }
                    break;
                }
            }
            this->mutex.unlock();


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
    teams.insert({"Green", Team("Green")}); 
    teams.insert({"Yellow", Team("Yellow")}); 
    teams.insert({"Orange", Team("Orange")}); 
    teams.insert({"Pink", Team("Pink")}); 
    std::thread timer([this](){gameLoop();});
    timer.detach();

    printf("Hi detach\n");

}

void Game::handlePacket(Packet &packet){

}
void Game::removePlayer(int fd){
    for(auto i=this->players_queue.begin();i!=this->players_queue.end();i++){
        if((*i)->getFD()==fd){
           this->players_queue.erase(i);
           return;
        }
    }
    for(auto i=this->players.begin();i!=this->players.end();i++){
        if((*i)->getFD()==fd){
           this->players.erase(i);
           return;
        }
    }
}

void Game::newPlace(){
    this->mutex.lock();
    (this->currentState) = 3;
    (this->time_counter)=60;
    this->mutex.unlock();
}

int Game::getCurrentState(){
    return currentState;
}