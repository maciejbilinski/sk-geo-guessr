#ifndef __HandlerNotHooked__
#define __HandlerNotHooked__
#include <exception>

class HandlerNotHooked : public std::exception{
    virtual const char* what() const throw(){
        return "You cannot call handleEvent before hookEpoll!";
    }
};


#endif // __HandlerNotHooked__
