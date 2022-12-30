#ifndef __Team__
#define __Team__

#include <map>
#include <string>
#include "../Client/Client.h"
class Team{
    int team_colour;
    int score = 0;
    std::map<int,Client*> members; //map aby było łatwiej potem wyrzucic usera
    
    public:
    Team(int team_colour);
    void broadcast_packet(Packet &packet);
    double calculate_points_distance(double target_point[2]); 
    void add_player(int client_fd);
    void remove_player(int client_fd);
};

#endif
