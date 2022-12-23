#ifndef __Packet__
#define __Packet__

#include <iostream>
#include "../enums/GameState/GameState.h"

class Packet{
   public:
        GameState state;
        std::string action;
        std::string content;

        Packet(GameState gameState, std::string action, std::string content);
        Packet(std::string str);
        std::string toString() const;
        void print() const;
        

};
#endif

