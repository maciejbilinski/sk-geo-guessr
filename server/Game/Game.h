#ifndef __Game__
#define __Game__
#include <exception>
#include <functional>
#include <map>
#include <optional>
#include <vector>
#include "../Packet/Packet.h"
#include "../Client/Client.h"
#include "Team.h"
#include <unistd.h>
#include <mutex>
#include <thread>

class Game{    
    std::vector<Client> players;
    std::map<int, Team> teams;

    std::map<int,int> votes; //key: target player, value numer of votes


    std::mutex mutex;
    int currentState; //enum: GameState.h
    int round;
    Client *host;
    void gameLoop();

    public:
    int time_counter;
    void handlePacket(Packet &packet);
    void removePlayer(int fd); //remove from team list and game list
    Game();
    void setup();
};
#endif

