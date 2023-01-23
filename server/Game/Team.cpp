#include "Team.h"
#include <cmath>
Team::Team(std::string team_colour){
    this->team_colour=team_colour;
}
void Team::broadcast_packet(Packet &packet){
}

void Team::removeAfk(std::function<void(Client*)> onRemove){
    bool was=false;
    for(auto member:this->members){
        was=false;
        for(auto point:this->members_points){
            if(member->getFD()==point.first){
                was=true;
                break;
            }
        }    
        if(!was){
            this->remove_player(member->getFD());
            onRemove(member);
        }
    }
}

double Team::calculate_points_distance(Point goal){
    double x=0,y=0;
    for(auto point: this->members_points){
        x+=point.second.x;
        y+=point.second.y;
    }
    if((double)this->members_points.size()==0){
        return 0;
    }
    x=x/(double)this->members_points.size();
    y=y/(double)this->members_points.size();
    x-=goal.x;
    y-=goal.y;
    
    return std::pow(y*y+x*x,0.5);
}
void Team::add_player(Client *client){
    for(auto player:this->members){
        if(player->getFD()==client->getFD()){
            return;
        }
    }
    this->members.push_back(client);
}
void Team::remove_player(int client_fd){
    for(auto j=this->members.begin();j<this->members.end();j++){
        if((*j)->getFD()==client_fd){
            this->members.erase(j); 
            return;
        }
    }
}

