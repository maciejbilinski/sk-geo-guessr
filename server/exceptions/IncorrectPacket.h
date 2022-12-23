#ifndef __IncorrectPacket__
#define __IncorrectPacket__
#include <exception>

class IncorrectPacket : public std::exception{
    virtual const char* what() const throw(){
        return "Error during packet parsing!";
    }
};


#endif // __IncorrectPacket__
