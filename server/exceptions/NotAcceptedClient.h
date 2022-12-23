#ifndef __NotAcceptedClient__
#define __NotAcceptedClient__
#include <exception>

class NotAcceptedClient : public std::exception{
    virtual const char* what() const throw(){
        return "Client was not accepted but should!";
    }
};


#endif // __NotAcceptedClient__
