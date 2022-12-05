#ifndef __Handler__
#define __Handler__

class Handler{
    protected:
        int fd;
        int epollFd;
        bool hooked;
    public:
        Handler();
        virtual void handleEvent(unsigned int events) = 0;
        virtual void hookEpoll(int epollFd);
        ~Handler();
};


#endif // __Handler__
