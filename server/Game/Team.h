#ifndef __Team__
#define __Team__

#include <map>
#include <string>
#include <vector>
#include "../Client/Client.h"
class Team{
    int score = 0;
    
    public:
    std::vector<Client*> members; //map aby było łatwiej potem wyrzucic usera
    std::string team_colour;
    Team(std::string team_colour);
    void broadcast_packet(Packet &packet);
    double calculate_points_distance(double target_point[2]); 
    void add_player(Client* client);
    void remove_player(int client_fd);
};

#endif
