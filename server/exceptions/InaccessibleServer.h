#ifndef __InaccessibleServer__
#define __InaccessibleServer__
#include <exception>

class InaccessibleServer : public std::exception{
    virtual const char* what() const throw(){
        return "Server cannot be started - socket, bind or listen failed";
    }
};


#endif // __InaccessibleServer__
