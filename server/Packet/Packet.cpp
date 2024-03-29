#include "Packet.h"

#include "../exceptions/IncorrectPacket.h"
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
Packet::Packet(std::string str){
    size_t pos = 0;
    std::vector<std::string> tokens;
    while ((pos = str.find(";")) != std::string::npos) {
        tokens.push_back(str.substr(0, pos));
        str.erase(0, pos + 1);
    }

    if(tokens.size() != 2){
        std::cout << "Not enough ';'" << std::endl;
        throw IncorrectPacket();
    }else{
        for (auto &token : tokens){
                pos = token.find("action");
                if(pos != 0){
                    pos = token.find("content");
                    if(pos != 0){
                        std::cout << "Missing parts" << std::endl;
                        throw IncorrectPacket();
                    }else{
                        token.erase(0, 8);
                        this->content = token;
                    }
                }else{
                    token.erase(0, 7);
                    this->action = token;
                }
        }
    }
}

Packet::Packet( std::string action, std::string content){
    this->action = action;
    this->content = content;
}

std::string Packet::toString() const{
    std::stringstream ss;
    ss <<  "action:" << this->action << ";content:" << this->content << std::endl;
    return ss.str();
}

void Packet::print() const{
    std::cout << this->toString();
}
