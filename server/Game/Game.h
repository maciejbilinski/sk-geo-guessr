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
#include <set>
class Game{    
    

    std::mutex mutex;
    int currentState; //enum: GameState.h
    int round;
    Client *host;
    void gameLoop();

    public:
    std::map<std::string, Team> teams;
    std::vector<int> votes; //key: target player, value numer of votes

    std::vector<Client*> players;
    std::vector<Client*> players_queue;
    int time_counter;
    void handlePacket(Packet &packet);
    void removePlayer(int fd); //remove from team list and game list
    Game();
    void setup();
};
#endif

