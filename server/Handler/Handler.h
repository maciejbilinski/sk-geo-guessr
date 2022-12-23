#ifndef __Handler__
#define __Handler__

#include <iostream>
#include <list>
#include "../Buffer/ReadBuffer.h"

class Handler{
    protected:
        int epollFd;
        bool hooked;
        int fd;
        
        Handler();
    public:
        int getFD();
        virtual void handleEvent(unsigned int events) = 0;
        virtual void hookEpoll(int epollFd);
        virtual ~Handler();
};


#endif // __Handler__
