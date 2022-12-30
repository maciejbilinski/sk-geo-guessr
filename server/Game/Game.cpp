#include "Game.h"
#include <cstdio>
#include <unistd.h>
#include <vector>
#include <sys/mman.h>

void Game::gameLoop(){
    while (true) {
        sleep(1);
        printf("%d %d \n", this->time_counter,this->currentState);
        switch (this->currentState) {
            case 1:
            case 2:
            case 3:
            case 4:
                this->mutex.lock();
            if (this->time_counter>0){
                (this->time_counter)--;
            }else{
               switch (this->currentState) {
                case 1:
                    (this->time_counter)=60; //TODO: set from config
                    (this->currentState)=2; // wchodzimy w stan wyboru zdjecia, ludzie widza plansze wait
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
            break;
            }
        }

}

Game::Game(){
    this->host=nullptr;
    this->players = std::vector<Client>();
    this->votes = std::map<int,int>();
    this->teams = std::map<int, Team>();

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
    for(int i=0;i<4;i++){
        teams.insert({i, Team(i)}); 
    }
    std::thread timer([this](){gameLoop();});
    timer.detach();

    printf("Hi detach\n");

}

void Game::handlePacket(Packet &packet){

}
void Game::removePlayer(int fd){}

