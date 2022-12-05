#ifndef __EpollFailed__
#define __EpollFailed__
#include <exception>

class EpollFailed : public std::exception{
    virtual const char* what() const throw(){
        return "Epoll failed during epoll_wait function!";
    }
};


#endif // __EpollFailed__
