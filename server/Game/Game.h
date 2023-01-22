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
    void gameLoop();

    public:
    int round;
    Client *host;
    std::map<std::string, Team> teams;
    std::vector<int> votes; //key: target player, value numer of votes
    int getCurrentState();

    std::vector<Client*> players;
    std::vector<Client*> players_queue;
    int time_counter;
    void handlePacket(Packet &packet);
    void removePlayer(int fd); //remove from team list and game list
    void newPlace();
    Game();
    void setup();
};
#endif

