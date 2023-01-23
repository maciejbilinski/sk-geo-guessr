#include "Game.h"
#include <cstdio>
#include <map>
#include <unistd.h>
#include <utility>
#include <vector>
#include <sys/mman.h>
#include <thread>
#include <string>
#include <chrono>
#include <sstream>
#include <bits/stdc++.h>
using namespace std::chrono;

std::vector<std::string> sort_map(std::map<std::string, double> &M)
{
    std::vector<std::pair<std::string, double>> A;
    for (auto &it : M)
    {
        if(it.second<0)
            it.second=0;
        A.push_back(it);
    }
    std::vector<std::string> rnd;
    sort(A.begin(), A.end(), [] (const auto &x, const auto &y) {return x.second > y.second;});
    for (auto& it : A) {
        std::cout << it.first << ' '<< it.second << std::endl;
        rnd.push_back(it.first);
    }
    return rnd;
}

void Game::gameLoop()
{
    if(this->time_counter % 5 == 0)
        printf("Players in green: %d \nTime remain: %d \nGame state: %d \nNumber of players in queue: %lu \nNumber of players in game: %lu \n\n",
                this->teams.at("Green").members.size(),
                this->time_counter,
                this->currentState,
                this->players_queue.size(),
                this->players.size());

    if (this->currentState > GameState::INTRO && this->players.size() < 1)
    {
        this->currentState = GameState::INTRO; // nie ma graczy wracamy do zbierania graczy
    }

    switch (this->currentState)
    {
        case GameState::INTRO:
            if (this->players_queue.size() >= this->MIN_PLAYERS_TO_VOTE)
            {
                this->backToVoting(false);
            }
            break;
        case GameState::VOTING:
            if (this->players_queue.size() > 0)
            { // dodajemy graczy i dajemy im szanse glosowac
                for (auto i = this->players_queue.begin(); i < this->players_queue.end(); i++)
                {
                    _addPlayer(*i, true);
                    this->players_queue.erase(i);
                }
                this->time_counter += this->VOTING_NEW_TIME;
                if (this->time_counter > this->VOTING_TIME)
                    this->time_counter = this->VOTING_TIME;
            }
            break;
    }
    if (this->time_counter > 0)
    {
        (this->time_counter)--;
    }
    else
    { // skończył sie czas na jakis etap
        int max = 0;
        int best = 0;
        std::map<int, int> temp = std::map<int, int>();
        std::map<std::string, double> rank = std::map<std::string, double>();
        std::string ranking = "";
        switch (this->currentState)
        {
            case GameState::VOTING:
                (this->time_counter) = this->ADMIN_PANEL_TIME;
                (this->currentState) = GameState::ADMIN_PANEL; // wchodzimy w stan wyboru zdjecia, ludzie widza plansze wait
                this->round = 0;
                for (auto vote : this->votes)
                {
                    if (temp.find(vote) == temp.end())
                    {
                        if (temp.find(vote) == temp.end())
                        {
                            temp.insert(std::pair<int, int>(vote, 1));
                        }
                        else
                        {
                            temp[vote] = temp[vote] + 1;
                        }
                    }
                }
                for (auto player : temp)
                {
                    if (player.second > max)
                    {
                        best = player.first;
                        max = player.second;
                    }
                }
                if (best == 0)
                { // nikt nie głosował, wracamy do głosowania (wtedy mogą dołączyć nowe osoby)
                    this->backToVoting(false);
                }
                else
                {
                    for (auto player : this->players)
                    {
                        if (player->getFD() == best)
                        {
                            if(player->getTeamName().size() <= 0){ // wybrany to afk
                                this->removePlayer(player->getFD());
                                player->onRemove(true);
                            }else{
                                this->host = player;
                                this->teams.at(player->getTeamName()).remove_player(player->getFD());

                                Packet packet("host", player->getName());
                                for (auto j = this->players.begin(); j != this->players.end(); j++)
                                {
                                    WriteBuffer *writer = new WriteBuffer((*j)->getFD(), [j](const Buffer &buffer)
                                            { std::cout << "Error during sent host packet to " << (*j)->getName() << std::endl; },
                                            [j, this]()
                                            {
                                            std::cout << "Sent to " << (*j)->getName() << " info about host: " << host->getName() << std::endl;
                                            },
                                            packet);
                                    (*j)->addWriter(writer);
                                }
                            }
                            break;
                        }
                    }
                    if (this->host == nullptr)
                    { // najwięcej głosow mial ktos kogo juz nie ma
                        this->backToVoting(true);
                    }
                }
                break;
            case GameState::ADMIN_PANEL: // wyrzucamy hosta za bezczynnosc i cofamy sie do vote
                this->removePlayer(this->host->getFD());
                this->host->onRemove(true);
                this->host = nullptr;
                this->backToVoting(true);
                break;
            case GameState::GAME:
                if(this->round >= this->MAX_ROUND){
                    (this->time_counter) = this->END_TIME;
                    (this->currentState) = GameState::END; // koniec gry, dajemy czas na zobaczenie rankingu
                }else{
                    (this->time_counter) = this->ADMIN_PANEL_TIME;
                    (this->currentState) = GameState::ADMIN_PANEL;  // koniec rundy, host wstawia nowe zdjecie
                }
                {
                Packet pck("round", std::to_string(this->round+1));
                WriteBuffer *writer = new WriteBuffer(host->getFD(), [this, pck](const Buffer &buffer) {
                        std::cout << "Error during round " << pck.action << " packet to " << host->getName() << std::endl;
                        }, [this, pck]() {
                        std::cout << "Sent round " << pck.action << " packet to " << host->getName() << std::endl;
                        }, pck);
                host->addWriter(writer);
                if(currentState != GameState::END){
                    Packet pck("round", std::to_string(this->round));
                    WriteBuffer *writer = new WriteBuffer(host->getFD(), [this, pck](const Buffer &buffer) {
                            std::cout << "Error during round " << pck.content << " packet to " << host->getName() << std::endl;
                            }, [this, pck]() {
                            std::cout << "Sent round " << pck.content << " packet to " << host->getName() << std::endl;
                            }, pck);
                    host->addWriter(writer);
                }
                }

                for (auto team : this->teams)
                {
                    rank.insert_or_assign(team.first, team.second.calculate_points_distance(this->goal));
                    team.second.removeAfk([this](Client* toRemove){
                            std::cout << "Removed " << toRemove->getName() << " because was afk" << std::endl;
                            this->removePlayer(toRemove->getFD());
                            toRemove->onRemove(true);
                            });
                    team.second.members_points.clear();
                }
                

                for (auto pair : sort_map(rank))
                {
                    std::cout<<pair<<std::endl;
                    ranking += pair + " ";
                }
                {

                    if(this->round >= this->MAX_ROUND){
                        Packet packet("rankine", ranking);
                        std::cout << "Try to send ranking packet:" << std::endl << '\t';
                        packet.print();

                        for (auto j = this->players.begin(); j < this->players.end(); j++)
                        {
                            WriteBuffer *writer = new WriteBuffer((*j)->getFD(), [j](const Buffer &buffer) {
                                    std::cout << "Error during ranking packet to " << (*j)->getName() << std::endl;
                                    }, [j]() {
                                    std::cout << "Sent ranking packet to " << (*j)->getName() << std::endl;
                                    }, packet);
                            (*j)->addWriter(writer);
                        }
                        WriteBuffer *writer = new WriteBuffer(host->getFD(), [this, packet](const Buffer &buffer) {
                                std::cout << "Error during ranking packet to " << host->getName() << std::endl;
                                }, [this, packet]() {
                                std::cout << "Sent ranking packet to " << host->getName() << std::endl;
                                }, packet);
                        host->addWriter(writer);

                        for (auto team : this->teams)
                        {
                            team.second.removeAfk([this](Client* toRemove){
                                    this->removePlayer(toRemove->getFD());
                                    toRemove->onRemove(true);
                                    });
                            team.second.members_points.clear();
                        }
                    }else{
                        Packet packet("ranking", ranking);
                        std::cout << "Try to send ranking packet:" << std::endl << '\t';
                        packet.print();

                        for (auto j = this->players.begin(); j < this->players.end(); j++)
                        {
                            WriteBuffer *writer = new WriteBuffer((*j)->getFD(), [j](const Buffer &buffer) {
                                    std::cout << "Error during ranking packet to " << (*j)->getName() << std::endl;
                                    }, [j]() {
                                    std::cout << "Sent ranking packet to " << (*j)->getName() << std::endl;
                                    }, packet);
                            (*j)->addWriter(writer);
                        }

                        Packet pck("round", std::to_string(this->round));
                        WriteBuffer *writer = new WriteBuffer(host->getFD(), [this, pck](const Buffer &buffer) {
                                std::cout << "Error during round " << pck.action << " packet to " << host->getName() << std::endl;
                                }, [this, pck]() {
                                std::cout << "Sent round " << pck.action << " packet to " << host->getName() << std::endl;
                                }, pck);
                        host->addWriter(writer);

                        for (auto team : this->teams)
                        {
                            team.second.removeAfk([this](Client* toRemove){
                                    this->removePlayer(toRemove->getFD());
                                    toRemove->onRemove(true);
                                    });
                            team.second.members_points.clear();
                        }
                    }
                }
                break;
            case GameState::END:
                backToVoting(true); // wracamy do głosowania
                break;
        }
    }
}

Game::Game()
{
    this->host = nullptr;
    this->players = std::vector<Client *>();
    this->players_queue = std::vector<Client *>();
    this->votes = std::vector<int>();
    this->teams = std::map<std::string, Team>();
    this->currentState = GameState::INTRO;
    this->time_counter = 0;
    this->round = 0;


    std::cout << "Game constructed" << std::endl;
}
void Game::setup(std::string config_file_path)
{
    teams.insert({"Green", Team("Green")});
    teams.insert({"Yellow", Team("Yellow")});
    teams.insert({"Orange", Team("Orange")});
    teams.insert({"Pink", Team("Pink")});
    std::cout << "Game setup" << std::endl;
    if(config_file_path.length()>0){
        std::ifstream infile(config_file_path);
        std::string temp;
        std::string temp_line;
        int temp_line_value;
        while (std::getline(infile,temp)) {
            std::replace(temp.begin(), temp.end(), ':', ' ');
            std::stringstream ss(temp);
            ss>>temp_line;
            ss>>temp_line_value;
            if(temp_line=="VOTING_TIME"){
                this->VOTING_TIME=temp_line_value;
            }
            if(temp_line=="VOTING_NEW_TIME"){
                this->VOTING_NEW_TIME=temp_line_value;
            }
            if(temp_line=="ADMIN_PANEL_TIME"){
                this->ADMIN_PANEL_TIME=temp_line_value;
            }
            if(temp_line=="GAME_TIME"){
                this->GAME_TIME=temp_line_value;
            }
            if(temp_line=="END_TIME"){
                this->GAME_TIME=temp_line_value;
            }
            if(temp_line=="MAX_ROUND"){
                this->MAX_ROUND=temp_line_value;
            }
            if(temp_line=="MIN_PLAYERS_TO_VOTE"){
                this->MIN_PLAYERS_TO_VOTE=temp_line_value;
            }
            std::cout<<temp_line<<" "<<temp_line_value<<std::endl;
        }
    }    
}


void Game::removePlayer(int fd)
{

    std::string name = "";

    bool found = false;

    for (auto i = this->players_queue.begin(); i != this->players_queue.end(); i++)
    {
        if ((*i)->getFD() == fd)
        {
            name = (*i)->getName();
            this->players_queue.erase(i);
            found = true;
            break;
        }
    }
    if (!found)
    {
        for (auto i = this->players.begin(); i != this->players.end(); i++)
        {
            if ((*i)->getFD() == fd)
            {
                name = (*i)->getName();
                if((*i)->getTeamName().size() > 0){
                    this->teams.at((*i)->getTeamName()).remove_player((*i)->getFD());
                }
                this->players.erase(i);
                found = true;
                break;
            }
        }
    }
    if (found)
    {
        Packet packet("player_disconnected", name);
        for (auto j = this->players.begin(); j < this->players.end(); j++)
        {
            WriteBuffer *writer = new WriteBuffer((*j)->getFD(), [j, name](const Buffer &buffer) {
                    std::cout << "Error during player_disconnected packet (" << name << ") to " << (*j)->getName() << std::endl;
                    }, [j, name]() {
                    std::cout << "Sent player_disconnected packet (" << name << ") to " << (*j)->getName() << std::endl;
                    }, packet);
            (*j)->addWriter(writer);
        }
    }
}

void Game::newPlace()
{ // host wysłał nowe miejsce na czas, gracze odpowiadaaj

    (this->currentState) = GameState::GAME;
    (this->time_counter) = this->GAME_TIME;
    this->round++;
}
bool Game::checkNameExists(Client* cl){
    for (int i = 0; players_queue.size() > i; i++)
    {
        if(players_queue.at(i)->getFD() == cl->getFD()) return true;
        if (players_queue.at(i)->getName() == cl->getName())
        {
            Packet packetReturn("error", "name_exists");
            WriteBuffer *writer = new WriteBuffer(
                    cl->getFD(), [cl](const Buffer &buffer)  { std::cout << "Error during error name_exists packet to " << cl->getName() << std::endl; },
                    [cl]()
                    {
                    std::cout << "Sent error name_exists packet to " << cl->getName() << std::endl;
                    }, packetReturn);
            cl->addWriter(writer);
            cl->onRemove(true);
            return true;
        }
    }

    for (int i = 0; players.size() > i; i++)
    {
        if(players.at(i)->getFD() == cl->getFD()) return true;
        if (players.at(i)->getName() == cl->getName())
        {
            Packet packetReturn("error", "name_exists");
            WriteBuffer *writer = new WriteBuffer(
                    cl->getFD(), [cl](const Buffer &buffer)  { std::cout << "Error during error name_exists packet to " << cl->getName() << std::endl; },
                    [cl]()
                    {
                    std::cout << "Sent error name_exists packet to " << cl->getName() << std::endl;
                    }, packetReturn);
            cl->addWriter(writer);
            cl->onRemove(true);
            return true;
        }
    }
    return false;

}
void Game::_addPlayer(Client *cl, bool inQueue)
{
    if (this->currentState == GameState::INTRO)
    {
        if (this->players_queue.size() >= (this->MIN_PLAYERS_TO_VOTE-1)){
            this->backToVoting(false);
        }
    }

    if (this->currentState != GameState::VOTING)
    {
        Packet packetReturn("player_intro", "game_started");
        WriteBuffer *writer = new WriteBuffer(
                cl->getFD(), [cl](const Buffer &buffer)
                { std::cout << "Error during player_intro game_started packet to " << cl->getName() << std::endl; },
                [cl]()
                {
                std::cout << "Sent player_intro game_started packet to " << cl->getName() << std::endl;
                },
                packetReturn);
        cl->addWriter(writer);
        if (!inQueue)
        {
            if(!this->checkNameExists(cl))
                this->players_queue.push_back(cl);
        }
    }
    else
    {
        if (!inQueue)
        {
            if(this->checkNameExists(cl)) return;
        }else{
            bool found = false;
            for(auto it = this->players_queue.begin(); it != this->players_queue.end(); ++it){
                if((*it)->getFD() == cl->getFD()){
                    this->players_queue.erase(it);
                    found = true;
                    break;
                }
            }
            if(!found){
                std::cout << "Error in _addPlayer: player should be in players_queue but is not" << std::endl;
                return;
            }
        }

        this->players.push_back(cl);
        Packet packetReturn("player_intro", "ok");
        WriteBuffer *writer = new WriteBuffer(
                cl->getFD(), [cl](const Buffer &buffer)
                { std::cout << "Error during player_intro ok packet to " << cl->getName() << std::endl; },
                [cl]()
                {
                std::cout << "Sent player_intro ok packet to " << cl->getName() << std::endl;
                },
                packetReturn);
        cl->addWriter(writer);
        if (this->players.size() > 0)
        {
            Packet packet("new_player", cl->getName());

            for (auto j = this->players.begin(); j < this->players.end(); j++)
            {
                if((*j)->getFD() != cl->getFD()){
                    WriteBuffer *writer = new WriteBuffer((*j)->getFD(), [cl, j](const Buffer &buffer) {
                            std::cout << "Error during new_player " << cl->getName() << " packet to " << (*j)->getName() << std::endl;
                            }, [cl, j]() {
                            std::cout << "Sent new_player " << cl->getName() << " packet to " << (*j)->getName() << std::endl;
                            }, packet);
                    (*j)->addWriter(writer);

                    Packet packet2("new_player", (*j)->getName());
                    WriteBuffer *writer2 = new WriteBuffer(
                            cl->getFD(), [cl, j](const Buffer &buffer) {
                            std::cout << "Error during new_player " << (*j)->getName() << " packet to " << cl->getName() << std::endl;
                            }, [cl, j]() {
                            std::cout << "Sent new_player " << (*j)->getName() << " packet to " << cl->getName() << std::endl;
                            }, packet2);
                    cl->addWriter(writer2);
                }
            }
        }

    }
}

void Game::addPlayer(Client *player)
{
    this->_addPlayer(player, false);
}

bool Game::addToTeam(Client *player, std::string color)
{
    bool found = false;
    if(currentState == GameState::VOTING){
        for (int i = 0; players.size() > i; i++)
        {
            if (players.at(i)->getFD() == player->getFD())
            {
                found = true;
                break;
            }
        }

        if (found)
        {
            if (color == "Green")
            {
                this->teams.at("Green").add_player(player);
            }
            else if (color == "Pink")
            {
                this->teams.at("Pink").add_player(player);
            }
            else if (color == "Yellow")
            {
                this->teams.at("Yellow").add_player(player);
            }
            else if (color == "Orange")
            {
                this->teams.at("Orange").add_player(player);
            }
            else
            {
                found = false;
            }
        }

    }


    return found;

}



void Game::startNewRound(Client *player, const Packet &packet)
{

    std::string result_content = "ok";

    if (player->getFD() == this->host->getFD())
    {
        if (this->currentState == GameState::ADMIN_PANEL)
        {
            size_t pos = 0;
            std::vector<std::string> tokens;
            std::string content = packet.content;
            while ((pos = content.find(" ")) != std::string::npos)
            {
                tokens.push_back(content.substr(0, pos));
                content.erase(0, pos + 1);
            }
            if (tokens.size() != 3)
            {
                result_content = "error1";
            }
            else
            {
                bool ok = false;
                try{
                    this->goal = Point(std::stod(tokens[1]), std::stod(tokens[2]));
                    ok = true;
                }catch(std::exception& e){
                    result_content = "bad_format";
                }
                if(ok){
                    auto ms = duration_cast<milliseconds>(
                            system_clock::now().time_since_epoch())
                        .count() +
                        this->GAME_TIME*1000; // TODO: CONFIG
                    Packet packet("place", std::to_string(this->round+1) + " " + tokens[0] + " " + std::to_string(ms));
                    this->newPlace();
                    std::cout << "Try to send place packet" << std::endl << '\t';
                    packet.print();
                    for (int i = 0; this->players.size() > i; i++)
                    {
                        if (this->players.at(i)->getFD() != player->getFD())
                        { // skip host
                            WriteBuffer *writer = new WriteBuffer(
                                    this->players.at(i)->getFD(), [this, i](const Buffer &buffer) {
                                    std::cout << "Error during place packet to " << this->players.at(i)->getName() << std::endl;
                                    }, [this, i]() {
                                    std::cout << "Sent place packet to " << this->players.at(i)->getName() << std::endl;
                                    }, packet);
                            this->players.at(i)->addWriter(writer);
                        }
                    }
                }
            }
        }
        else
            result_content = "not_now";
    }
    else
    {
        result_content = "error2";
    }

    Packet packetReturn("host_place", result_content);
    WriteBuffer *writer = new WriteBuffer(
            player->getFD(), [result_content, player](const Buffer &buffer) {
            std::cout << "Error during host_place " << result_content << " packet to " << player->getName() << std::endl;
            }, [result_content, player]() {
            std::cout << "Sent host_place " << result_content << " packet to " << player->getName() << std::endl;
            }, packetReturn);
    player->addWriter(writer);
    Packet pck("round_time", std::to_string(this->round)+" "+std::to_string(duration_cast<milliseconds>(
                    system_clock::now().time_since_epoch())
                .count() +
                this->GAME_TIME*1000));
    WriteBuffer *writerround = new WriteBuffer(host->getFD(), [this, pck](const Buffer &buffer) {
            std::cout << "Error during round " << pck.action << " packet to " << host->getName() << std::endl;
            }, [this, pck]() {
            std::cout << "Sent round " << pck.action << " packet to " << host->getName() << std::endl;
            }, pck);
    host->addWriter(writerround);

}

void Game::vote(Client *player, const Packet &packet)
{

    bool found = false;
    for (auto i = this->players.begin(); i < this->players.end(); ++i)
    {
        if ((*i)->getName() == packet.content)
        {
            this->votes.push_back((*i)->getFD());
            Packet packetReturn("player_vote", "ok");
            WriteBuffer *writer = new WriteBuffer(
                    player->getFD(), [player](const Buffer &buffer) {
                    std::cout << "Error during player_vote ok packet to " << player->getName() << std::endl;
                    }, [player]() {
                    std::cout << "Sent player_vote ok packet to " << player->getName() << std::endl;
                    }, packetReturn);
            player->addWriter(writer);
            found = true;
            break;
        }
    }
    if (!found)
    {
        Packet packetReturn("player_vote", "not_exists");
        WriteBuffer *writer = new WriteBuffer(
                player->getFD(), [player](const Buffer &buffer) {
                std::cout << "Error during player_vote not_exists packet to " << player->getName() << std::endl;
                }, [player]() {
                std::cout << "Sent player_vote not_exists packet to " << player->getName() << std::endl;
                }, packetReturn);
        player->addWriter(writer);
    }
}

void Game::setPlace(Client *player, const Packet &packet)
{
    std::string result_content = "not_now";

    if (currentState == GameState::GAME)
    {
        bool ok = true;     

        size_t pos = 0;
        std::vector<std::string> tokens;
        std::string content = packet.content;
        while ((pos = content.find(" ")) != std::string::npos)
        {
            tokens.push_back(content.substr(0, pos));
            content.erase(0, pos + 1);
        }
        if (tokens.size() != 2)
        {
            result_content = "error1";
            ok = false;
        }

        if(ok){
            double x, y;
            try{
                x = std::stod(tokens[0]);
                y =std::stod(tokens[1]);
            }catch(std::exception& e){
                result_content = "bad_format";
                ok = false;
            }

            if(ok){
                std::cout << "Place from " << player->getName() << ": " << x << " " << y << std::endl;
                this->teams.at(player->getTeamName()).members_points.insert_or_assign(player->getFD(), Point(x, y));

                Packet packetReturn("user_set_place", player->getName() + "," + std::to_string(x) + "," + std::to_string(y));
                for (int i = 0; this->teams.at(player->getTeamName()).members.size() > i; i++)
                {
                    Client* cl = this->teams.at(player->getTeamName()).members.at(i);
                    if (cl->getFD() != player->getFD())
                    { // skip player
                        WriteBuffer *writer = new WriteBuffer(
                                cl->getFD(), [packetReturn, cl, player](const Buffer &buffer) {
                                std::cout << "Error during user_set_place " << packetReturn.content << " from " << player->getName() << " packet to " << cl->getName() << std::endl;
                                }, [packetReturn, cl, player]() {
                                std::cout << "Sent user_set_place " << packetReturn.content << " from " << player->getName() << " packet to " << cl->getName() << std::endl;
                                }, packetReturn);
                        cl->addWriter(writer);
                    }
                }

                Client* cl = this->host;
                WriteBuffer *writer = new WriteBuffer(
                        cl->getFD(), [packetReturn, cl, player](const Buffer &buffer) {
                        std::cout << "Error during user_set_place " << packetReturn.content << " from " << player->getName() << " packet to " << cl->getName() << std::endl;
                        }, [packetReturn, cl, player]() {
                        std::cout << "Sent user_set_place " << packetReturn.content << " from " << player->getName() << " packet to " << cl->getName() << std::endl;
                        }, packetReturn);
                cl->addWriter(writer);

                result_content = "ok";
            }
        }
    }

    Packet packetReturn("set_place", result_content);
    WriteBuffer *writer = new WriteBuffer(
            player->getFD(), [player, result_content](const Buffer &buffer) {
            std::cout << "Error during set_place " << result_content << " packet to " << player->getName() << std::endl;

            }, [player, result_content]() {
            std::cout << "Sent set_place " << result_content << " packet to " << player->getName() << std::endl;
            }, packetReturn);
    player->addWriter(writer);

}



int Game::getTime() { return time_counter; }

void Game::backToVoting(bool send){
    this->votes.clear();
    for (auto team : this->teams) // to jest powiązane
    {
        team.second.members.clear();
        team.second.members_points.clear();
    }
    for(auto player : this->players){
        player->removeTeam();
    }
    this->round = 0;
    this->host = nullptr;
    (this->time_counter) = this->VOTING_TIME;
    (this->currentState) = GameState::VOTING;
    if(send){
        for (auto j = this->players.begin(); j < this->players.end(); j++)
        {
            Packet packet("voting_failed", "");
            WriteBuffer *writer = new WriteBuffer((*j)->getFD(), [j](const Buffer &buffer)
                    { std::cout << "Error during sent voting_failed packet to " << (*j)->getName() << std::endl; },
                    [j]()
                    {
                    std::cout << "Sent to " << (*j)->getName() << " voting_failed packet: " << std::endl;
                    },
                    packet);
            (*j)->addWriter(writer);
        }
    }
}
