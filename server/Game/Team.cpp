#include "Team.h"

Team::Team(int team_colour){
    this->team_colour=team_colour;
}
void Team::broadcast_packet(Packet &packet){
    for(auto player:this->members){

    }
}

double Team::calculate_points_distance(double target_point[2]){

}
void Team::add_player(int client_fd){

}
void Team::remove_player(int client_fd){

}

