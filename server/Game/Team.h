#ifndef __Team__
#define __Team__

#include <map>
#include <string>
#include <vector>
#include "../Client/Client.h"
#include "Point.h"
class Team{
    int score = 0;

    public:

    std::vector<Client*> members; //map aby było łatwiej potem wyrzucic usera
    std::map<int,Point> members_points; //map aby było łatwiej potem wyrzucic usera
    std::string team_colour;
    Team(std::string team_colour);
    void broadcast_packet(Packet &packet);
    void removeAfk();
    double calculate_points_distance(Point point); 
    void add_player(Client* client);
    void remove_player(int client_fd);
};

#endif
