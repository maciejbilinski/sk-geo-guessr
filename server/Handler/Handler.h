#ifndef __Handler__
#define __Handler__

#include <cstdlib>
#include <cstring>
#include <list>
#include <unistd.h>
#include "../Buffer.h"
class Handler{
    
        protected:
        int epollFd;
        bool hooked;
        Buffer readBuffer;
        std::list<Buffer> dataToWrite;

    public:
        int fd;
        Handler();
        virtual void handleEvent(unsigned int events) = 0;
        virtual void hookEpoll(int epollFd);
        ~Handler();
};


#endif // __Handler__
