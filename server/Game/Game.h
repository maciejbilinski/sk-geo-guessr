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
#include "Point.h"
#include <unistd.h>
#include <mutex>
#include <thread>
#include <set>
class Game{    
    std::mutex allMutex;    

    int currentState; //enum: GameState.h
    
    int round;
    
    Client *host;
    
    std::map<std::string, Team> teams;
    
    Point goal=Point(0, 0);
    
    std::vector<int> votes; //key: target player, value numer of votes

    std::vector<Client*> players;

    std::vector<Client*> players_queue;

    int time_counter;

    void _addPlayer(Client* player, bool inQueue, bool lock);

    
    void gameLoop();

    public:
    void handlePacket(Packet &packet);
    void removePlayer(int fd); //remove from team list and game list
    void newPlace();
    Game();
    void setup();
    void addPlayer(Client* player);
    bool addToTeam(Client* player, std::string color);

    void startNewRound(Client* player, const Packet& packet);
    void vote(Client* player, const Packet& packet);
    void setPlace(Client* player, const Packet& packet);
    
};
#endif

