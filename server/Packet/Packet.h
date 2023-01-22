#ifndef __Packet__
#define __Packet__

#include <iostream>
#include "../enums/GameState/GameState.h"

class Packet{
   public:
        std::string action;
        std::string content;

        Packet(std::string action, std::string content);
        Packet(std::string str);
        std::string toString() const;
        void print() const;
};
#endif

