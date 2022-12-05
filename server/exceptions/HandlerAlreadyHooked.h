#ifndef __HandlerAlreadyHooked__
#define __HandlerAlreadyHooked__
#include <exception>

class HandlerAlreadyHooked : public std::exception{
    virtual const char* what() const throw(){
        return "You cannot call hook on handler multiple times!";
    }
};


#endif // __HandlerAlreadyHooked__
